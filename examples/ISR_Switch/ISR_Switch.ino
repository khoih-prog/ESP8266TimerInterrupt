/******************************************************************************
 * examples/ISR_Switch.ino
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP32TimerInterrupt
 * Licensed under MIT license
 * Version: v1.0.1
 * 
 * Notes:
 * Special design is necessary to share data between interrupt code and the rest of your program.
 * Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume 
 * variable can not spontaneously change. Because your function may change variables while your program is using them, 
 * the compiler needs this hint. But volatile alone is often not enough.
 * When accessing shared variables, usually interrupts must be disabled. Even with volatile, 
 * if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly. 
 * If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled 
 * or the entire sequence of your code which accesses the data.
 *
 * Version Modified By   Date      Comments
 * ------- -----------  ---------- -----------
 *  1.0.1   K Hoang      25/11/2019 New release fixing compiler error 
 ********************************************************************************/
 /* ISR_Switch demontrates the use of ISR to avoid being blocked by other CPU-monopolizing task
  *  
  * In this complex example: CPU is connecting to WiFi, Internet and finally Blynk service (https://docs.blynk.cc/)
  * Many important tasks are fighting for limited CPU resource in this no-controlled single-tasking environment.
  * In certain period, mission-critical tasks (you name it) could be deprived of CPU time and have no chance
  * to be executed. This can lead to disastrous results at critical time.
  * We hereby will use interrupt to detect whenever the SW is active, then switch ON/OFF a sample relay (lamp)
  * We'll see this ISR-based operation will have highest priority, preempts all remaining tasks to assure its
  * functionality.
  */
 
//These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
#define TIMER_INTERRUPT_DEBUG      1

#define BLYNK_PRINT Serial
//#define BLYNK_DEBUG true

#include <ESP8266WiFi.h>

//#define USE_BLYNK_WM   true
#define USE_BLYNK_WM   false

#define USE_SSL     false

#if USE_BLYNK_WM
  #if USE_SSL
    #include <BlynkSimpleEsp8266_SSL_WM.h>        //https://github.com/khoih-prog/Blynk_WM
  #else
    #include <BlynkSimpleEsp8266_WM.h>            //https://github.com/khoih-prog/Blynk_WM
  #endif
#else
  #if USE_SSL
    #include <BlynkSimpleEsp8266_SSL.h>
    #define BLYNK_HARDWARE_PORT     9443
  #else
    #include <BlynkSimpleEsp8266.h>
    #define BLYNK_HARDWARE_PORT     8080   
  #endif
#endif

#if !USE_BLYNK_WM
  #define USE_LOCAL_SERVER    true
  
  // If local server
  #if USE_LOCAL_SERVER
    char blynk_server[]   = "yourname.duckdns.org";
    //char blynk_server[]   = "192.168.2.110";
  #else
    char blynk_server[]   = "";
  #endif

char auth[]     = "***";
char ssid[]     = "***";
char pass[]     = "***";

#endif

#define DEBOUNCE_TIME               25
#define LONG_BUTTON_PRESS_TIME_MS   100
#define DEBUG_ISR                   0

#define VPIN                        V1
#define LAMPSTATE_PIN               V2

//Blynk Color in format #RRGGBB
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

WidgetLED  LampStatus(LAMPSTATE_PIN);

volatile unsigned long  lastDebounceTime  = 0;
volatile bool           buttonPressed     = false;
volatile bool           alreadyTriggered  = false;

volatile bool LampState    = false;
volatile bool SwitchReset  = true;

#define RELAY_PIN     D1
#define BUTTON_PIN    D3

BlynkTimer Timer;

unsigned int myWiFiTimeout        =  3200L;  //  3.2s WiFi connection timeout   (WCT)
unsigned int buttonInterval       =  511L;  //   0.5s update button state

void ICACHE_RAM_ATTR Falling();
void ICACHE_RAM_ATTR Rising();

void ICACHE_RAM_ATTR lightOn();
void ICACHE_RAM_ATTR lightOff();
void ICACHE_RAM_ATTR ButtonCheck();
void ICACHE_RAM_ATTR ToggleRelay();

BLYNK_CONNECTED()
{
  LampStatus.on();
  Blynk.setProperty(LAMPSTATE_PIN, "color", LampState? BLYNK_RED : BLYNK_GREEN );
  Blynk.syncVirtual(VPIN);
}

// Make this a autoreleased pushbutton
BLYNK_WRITE(VPIN) 
{   
  if (param.asInt())
    ToggleRelay();
}

void ICACHE_RAM_ATTR Rising()
{
  unsigned long currentTime  = millis();
  unsigned long TimeDiff;

  TimeDiff = currentTime - lastDebounceTime;
  if ( digitalRead(BUTTON_PIN) && (TimeDiff > DEBOUNCE_TIME) )
  {
    buttonPressed = false;
    ButtonCheck();          
    lastDebounceTime = currentTime;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), Falling, FALLING);    
  }
}

void ICACHE_RAM_ATTR Falling()
{
  unsigned long currentTime  = millis();
     
  if ( !digitalRead(BUTTON_PIN) && (currentTime > lastDebounceTime + DEBOUNCE_TIME))
  {
    lastDebounceTime = currentTime;
    ButtonCheck();    
    buttonPressed = true;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), Rising, RISING);                                                
  }     
}

void heartBeatPrint(void)
{
  static int num = 1;
  
  Serial.print("B");
  if (num == 80) 
  {
    Serial.println();
    num = 1;
  }
  else if (num++ % 10 == 0) 
  {
    Serial.print(" ");
  }
} 

void checkButton()
{
  heartBeatPrint();
  
  if (LampState)
    Blynk.setProperty(LAMPSTATE_PIN, "color", BLYNK_RED);
  else
    Blynk.setProperty(LAMPSTATE_PIN, "color", BLYNK_GREEN);
}

void ICACHE_RAM_ATTR ButtonCheck() 
{
  boolean SwitchState = (digitalRead(BUTTON_PIN));
  
  if (!SwitchState && SwitchReset) 
  {
    ToggleRelay();
    SwitchReset = false;
  }
  else if (SwitchState) 
  {
    SwitchReset = true;
  }
}

void ICACHE_RAM_ATTR ToggleRelay() 
{
  if (LampState) 
    lightOff();
  else 
    lightOn();  
}

void ICACHE_RAM_ATTR lightOn() 
{
  digitalWrite(RELAY_PIN, HIGH);
  LampState = true;
}

void ICACHE_RAM_ATTR lightOff() 
{
  digitalWrite(RELAY_PIN, LOW);
  LampState = false;
}

void setup() 
{
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  digitalWrite(RELAY_PIN, LOW);
  LampState = false;

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), Falling, FALLING);
  
  #if USE_BLYNK_WM
    Blynk.begin();
  #else
    unsigned long startWiFi = millis();
    
    WiFi.begin(ssid, pass);
    
    do
    {
      delay(200);
      if ( (WiFi.status() == WL_CONNECTED) || (millis() > startWiFi + myWiFiTimeout) )
        break;
    } while (WiFi.status() != WL_CONNECTED);
    
    Blynk.config(auth, blynk_server, BLYNK_HARDWARE_PORT);
    Blynk.connect();

    if (Blynk.connected())
      Serial.println("Blynk connected");
    else
      Serial.println("Blynk not connected yet");  
  #endif

  Timer.setInterval(buttonInterval, checkButton);  
}

void loop() 
{
  Blynk.run();
  Timer.run();
}

