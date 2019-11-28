/******************************************************************************
 * examples/ISR_Timer_Switches.ino
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
 /* ISR_Timer_Switches demontrates the use of ISR combining with Timer Interrupt to avoid being blocked by 
  * other CPU-monopolizing task
  *  
  * In this complex example: CPU is connecting to WiFi, Internet and finally Blynk service (https://docs.blynk.cc/)
  * Many important tasks are fighting for limited CPU resource in this no-controlled single-tasking environment.
  * In certain period, mission-critical tasks (you name it) could be deprived of CPU time and have no chance
  * to be executed. This can lead to disastrous results at critical time.
  * We hereby will use interrupt to detect whenever a SW is active, then use a hardware timer to poll and switch 
  * ON/OFF a corresponding sample relay (lamp)
  * We'll see this ISR-based operation will have highest priority, preempts all remaining tasks to assure its
  * functionality.
  */

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

#define TIMER_INTERRUPT_DEBUG      0

#include "ESP8266TimerInterrupt.h"

// Init ESP8266 timer
ESP8266Timer ITimer;
#define TIMER_INTERVAL_MS         100

#if !USE_BLYNK_WM
  #define USE_LOCAL_SERVER    true
  
  // If local server
  #if USE_LOCAL_SERVER
    char blynk_server[]   = "yourname.duckdns.org";
    //char blynk_server[]   = "192.168.2.110";
  #else
    char blynk_server[]   = "";
  #endif

char auth[]     = "****";
char ssid[]     = "****";
char pass[]     = "****";

#endif

#define DEBOUNCE_TIME               25
#define LONG_BUTTON_PRESS_TIME_MS   100
#define DEBUG_ISR                   0

// Lamp 1
#define VPIN                        V71     //V1
#define LAMPSTATE_PIN               V77     //V2
// Lamp 2
#define V2PIN                       V70     //V3
#define LAMP2STATE_PIN              V76     //V4

//Blynk Color in format #RRGGBB
#define BLYNK_GREEN     "#23C48E"
#define BLYNK_RED       "#D3435C"

// Lamp1 and Relay1
WidgetLED  LampStatus(LAMPSTATE_PIN);

volatile unsigned long  lastDebounceTime  = 0;
volatile bool           buttonPressed     = false;
volatile bool           alreadyTriggered  = false;

volatile bool LampState    = false;
volatile bool SwitchReset  = true;

#define RELAY_PIN     D2
#define BUTTON_PIN    D4

// Lamp2 and Relay2
WidgetLED  Lamp2Status(LAMP2STATE_PIN);

volatile unsigned long  lastDebounceTime2  = 0;
volatile bool           buttonPressed2     = false;
volatile bool           alreadyTriggered2  = false;

volatile bool Lamp2State    = false;
volatile bool Switch2Reset  = true;

#define RELAY2_PIN     D1
#define BUTTON2_PIN    D3

unsigned int myWiFiTimeout        =  3200L;  //  3.2s WiFi connection timeout   (WCT)
unsigned int buttonInterval       =  511L;   //  0.5s update button state

// Lamp1
void ICACHE_RAM_ATTR Falling();
void ICACHE_RAM_ATTR Rising();

void ICACHE_RAM_ATTR lightOn();
void ICACHE_RAM_ATTR lightOff();
void ICACHE_RAM_ATTR ButtonCheck();
void ICACHE_RAM_ATTR ToggleRelay();

// Lamp2
void ICACHE_RAM_ATTR Falling2();
void ICACHE_RAM_ATTR Rising2();

void ICACHE_RAM_ATTR light2On();
void ICACHE_RAM_ATTR light2Off();
void ICACHE_RAM_ATTR ButtonCheck2();
void ICACHE_RAM_ATTR ToggleRelay2();

BlynkTimer Timer;

BLYNK_CONNECTED()
{
  // Lamp 1
  LampStatus.on();
  Blynk.setProperty(LAMPSTATE_PIN, "color", LampState? BLYNK_RED : BLYNK_GREEN );

    // Lamp 2
  Lamp2Status.on();
  Blynk.setProperty(LAMP2STATE_PIN, "color", Lamp2State? BLYNK_RED : BLYNK_GREEN );

  Blynk.syncVirtual(VPIN);
}

// Lamp1
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
    buttonPressed = true;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), Rising, RISING);                                                
  }     
}

// Lamp2
// Make this a autoreleased pushbutton
BLYNK_WRITE(V2PIN) 
{   
  if (param.asInt())
    ToggleRelay2();
}

void ICACHE_RAM_ATTR Rising2()
{
  unsigned long currentTime  = millis();
  unsigned long TimeDiff;

  TimeDiff = currentTime - lastDebounceTime2;
  if ( digitalRead(BUTTON2_PIN) && (TimeDiff > DEBOUNCE_TIME) )
  {
    buttonPressed2 = false;
    lastDebounceTime2 = currentTime;
    attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), Falling2, FALLING);    
  }
}

void ICACHE_RAM_ATTR Falling2()
{
  unsigned long currentTime  = millis();
     
  if ( !digitalRead(BUTTON2_PIN) && (currentTime > lastDebounceTime2 + DEBOUNCE_TIME))
  {
    lastDebounceTime2 = currentTime;
    buttonPressed2 = true;
    attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), Rising2, RISING);                                                
  }     
}

// Share for both buttons
void ICACHE_RAM_ATTR HWCheckButton()
{
  // Button 1
  if (!alreadyTriggered && buttonPressed)
  {
    alreadyTriggered = true;
    ButtonCheck();
  
  }
  else if (!buttonPressed)
  {
    // Reset flag when button released to avoid triggered repeatedly
    alreadyTriggered = false;
    ButtonCheck();
  }

  // Button 2
  if (!alreadyTriggered2 && buttonPressed2)
  {
    alreadyTriggered2 = true;
    ButtonCheck2();
  
  }
  else if (!buttonPressed2)
  {
    // Reset flag when button released to avoid triggered repeatedly
    alreadyTriggered2 = false;
    ButtonCheck2();
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

  // Lamp 1
  if (LampState)
    Blynk.setProperty(LAMPSTATE_PIN, "color", BLYNK_RED);
  else
    Blynk.setProperty(LAMPSTATE_PIN, "color", BLYNK_GREEN);

  // Lamp2
  if (Lamp2State)
    Blynk.setProperty(LAMP2STATE_PIN, "color", BLYNK_RED);
  else
    Blynk.setProperty(LAMP2STATE_PIN, "color", BLYNK_GREEN);    
}

//Lamp 1
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

// Lamp2
void ICACHE_RAM_ATTR ButtonCheck2() 
{
  boolean SwitchState = (digitalRead(BUTTON2_PIN));
  
  if (!SwitchState && Switch2Reset) 
  {
    ToggleRelay2();
    Switch2Reset = false;
  }
  else if (SwitchState) 
  {
    Switch2Reset = true;
  }
}

void ICACHE_RAM_ATTR ToggleRelay2() 
{
  if (Lamp2State) 
    light2Off();
  else 
    light2On();  
}

void ICACHE_RAM_ATTR light2On() 
{
  digitalWrite(RELAY2_PIN, HIGH);
  Lamp2State = true;
}

void ICACHE_RAM_ATTR light2Off() 
{
  digitalWrite(RELAY2_PIN, LOW);
  Lamp2State = false;
}

void setup() 
{
  Serial.begin(115200);
  Serial.println("\nStarting");

  // Lamp 1
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), Falling, FALLING);

  // Lamp 2
  pinMode(RELAY2_PIN, OUTPUT);
  pinMode(BUTTON2_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(BUTTON2_PIN), Falling2, FALLING);

  // Interval in microsecs
  // Interval in microsecs, so MS to multiply by 1000
  // Be sure to place this HW Timer well ahead blocking calls, because it needs to be initialized.
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, HWCheckButton))
    Serial.println("Starting  ITimer OK, millis() = " + String(millis()));
  else
    Serial.println("Can't set ITimer. Select another freq. or interval");
    
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
