/************************************************
 * examples/RPM_Measure.ino
 * For ESP8266 boards
 * Written by Khoi Hoang
 * 
 * Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
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
 ************************************************/
//These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.

#define TIMER_INTERRUPT_DEBUG      1

#include "ESP8266TimerInterrupt.h"

#define BUILTIN_LED     2       // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED

volatile bool statusLed = false;
volatile uint32_t lastMillis = 0;

#define TIMER_INTERVAL_MS       1000

// Init ESP8266 timer 0
ESP8266Timer ITimer;

//=======================================================================
void ICACHE_RAM_ATTR TimerHandler()
{
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(BUILTIN_LED,OUTPUT);
  }
  
  digitalWrite(BUILTIN_LED, statusLed);  //Toggle LED Pin
  statusLed = !statusLed;

  #if (TIMER_INTERRUPT_DEBUG > 0)
  Serial.println("Delta ms = " + String(millis() - lastMillis));
  lastMillis = millis();
  #endif
}
//=======================================================================
//                               Setup
//=======================================================================
void setup()
{
  Serial.begin(115200);
  Serial.println("\nStarting, F_CPU = " + String(F_CPU));
  
  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.println("Starting  ITimer OK, millis() = " + String(lastMillis));
  }
  else
    Serial.println("Can't set ITimer correctly. Select another freq. or interval");
  
}
//=======================================================================
//                MAIN LOOP
//=======================================================================
void loop()
{
}
