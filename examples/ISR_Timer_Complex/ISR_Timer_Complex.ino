/****************************************************************************************************************************
 * examples/ISR_Timer_Complex.ino
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP32TimerInterrupt
 * Licensed under MIT license
 * Version: v1.0.2
 * 
 * The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
 * The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
 * The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
 * Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!
 * 
 * Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long miliseconds)
 * The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
 * Therefore, their executions are not blocked by bad-behaving functions / tasks.
 * This important feature is absolutely necessary for mission-critical tasks. 
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
*****************************************************************************************************************************/

/****************************************************************************************************************************
 * This example will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs.
 * Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet
 * and Blynk services. You can also have many (up to 16) timers to use.
 * This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 
 * You'll see blynkTimer is blocked while connecting to WiFi / Internet / Blynk, and elapsed time is very unaccurate
 * In this super simple example, you don't see much different after Blynk is connected, because of no competing task is
 * written
*****************************************************************************************************************************/
 
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

#include "ESP8266TimerInterrupt.h"
#include "ESP8266_ISR_Timer.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
#endif

#define HW_TIMER_INTERVAL_MS        50

#define WIFI_TIMEOUT      20000L

volatile uint32_t lastMillis = 0;

// Init ESP32 timer 0
ESP8266Timer ITimer;

// Init BlynkTimer
ESP8266_ISR_Timer ISR_Timer;

BlynkTimer blynkTimer;

void ICACHE_RAM_ATTR TimerHandler(void)
{
  static bool toggle = false;
  static bool started = false;

  static int timeRun      = 0;

  ISR_Timer.run();

  // Toggle LED every 50 x 100 = 5000ms = 5s
  if (++timeRun == 100)
  { 
    timeRun = 0;

    if (!started)
    {
      started = true;
      pinMode(LED_BUILTIN, OUTPUT);
    }
  
    #if (TIMER_INTERRUPT_DEBUG > 0)
    Serial.println("Delta ms = " + String(millis() - lastMillis));
    lastMillis = millis();
    #endif
    
    //timer interrupt toggles pin LED_BUILTIN
    digitalWrite(LED_BUILTIN, toggle);
    toggle = !toggle;
  }
}

void ICACHE_RAM_ATTR doingSomething2s()
{
  static unsigned long previousMillis = lastMillis;
  Serial.println("doingSomething2s: Delta ms = " + String(millis() - previousMillis));
  previousMillis = millis();
}

void ICACHE_RAM_ATTR doingSomething5s()
{
  static unsigned long previousMillis = lastMillis;
  Serial.println("doingSomething5s: Delta ms = " + String(millis() - previousMillis));
  previousMillis = millis();
}

void ICACHE_RAM_ATTR doingSomething10s()
{
  static unsigned long previousMillis = lastMillis;
  Serial.println("doingSomething10s: Delta ms = " + String(millis() - previousMillis));
  previousMillis = millis();
}

void ICACHE_RAM_ATTR doingSomething50s()
{
  static unsigned long previousMillis = lastMillis;
  Serial.println("doingSomething50s: Delta ms = " + String(millis() - previousMillis));
  previousMillis = millis();
}

#define BLYNK_TIMER_MS        2000L
  
void blynkDoingSomething2s()
{
  static unsigned long previousMillis = lastMillis;
  Serial.println("blynkDoingSomething2s: Delta programmed ms = " + String(BLYNK_TIMER_MS) + ", actual = " + String(millis() - previousMillis));
  previousMillis = millis();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("\nStarting");
  
  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.println("Starting  ITimer OK, millis() = " + String(lastMillis));
  }
  else
    Serial.println("Can't set ITimer correctly. Select another freq. or interval");

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  ISR_Timer.setInterval(2000L, doingSomething2s);  
  ISR_Timer.setInterval(5000L, doingSomething5s);  
  ISR_Timer.setInterval(10000L, doingSomething10s);  
  ISR_Timer.setInterval(50000L, doingSomething50s);

  // You need this timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary.
  blynkTimer.setInterval(BLYNK_TIMER_MS, blynkDoingSomething2s);  


  #if USE_BLYNK_WM
    Blynk.begin();
  #else
    unsigned long startWiFi = millis();
    
    WiFi.begin(ssid, pass);
    
    do
    {
      delay(200);
      if ( (WiFi.status() == WL_CONNECTED) || (millis() > startWiFi + WIFI_TIMEOUT) )
        break;
    } while (WiFi.status() != WL_CONNECTED);
    
    Blynk.config(auth, blynk_server, BLYNK_HARDWARE_PORT);
    Blynk.connect();

    if (Blynk.connected())
      Serial.println("Blynk connected");
    else
      Serial.println("Blynk not connected yet");  
  #endif

}

#define BLOCKING_TIME_MS      3000L

void loop()
{
  static unsigned long previousMillis = lastMillis;
  
  Blynk.run();

  // This unadvised blocking task is used to demonstrate the blocking effects onto the execution and accuracy to Software timer
  // You see the time elapse of ISR_Timer still accurate, whereas very unaccurate for Software Timer
  // The time elapse for 2000ms software timer now becomes 3000ms (BLOCKING_TIME_MS)
  // While that of ISR_Timer is still prefect.
  delay(BLOCKING_TIME_MS);
  
  // You need this Software timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary
  // You don't need to and never call ISR_Timer.run() here in the loop(). It's already handled by ISR timer.
  blynkTimer.run();
}
