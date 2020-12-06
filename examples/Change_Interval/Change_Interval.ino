/****************************************************************************************************************************
   Change_Interval.ino
   For NRF52 boards using mbed-RTOS such as Nano-33-BLE
   Written by Khoi Hoang

   Built by Khoi Hoang https://github.com/khoih-prog/NRF52_MBED_TimerInterrupt
   Licensed under MIT license

   Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
   unsigned long miliseconds), you just consume only one NRF52 timer and avoid conflicting with other cores' tasks.
   The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
   Therefore, their executions are not blocked by bad-behaving functions / tasks.
   This important feature is absolutely necessary for mission-critical tasks.

   Based on SimpleTimer - A timer library for Arduino.
   Author: mromani@ottotecnica.com
   Copyright (c) 2010 OTTOTECNICA Italy

   Based on BlynkTimer.h
   Author: Volodymyr Shymanskyy

   Version: 1.0.2

   Version Modified By   Date      Comments
   ------- -----------  ---------- -----------
   1.0.1   K Hoang      22/11/2020 Initial coding and sync with NRF52_TimerInterrupt
   1.0.2   K Hoang      23/11/2020 Add and optimize examples
*****************************************************************************************************************************/

/*
   Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.
*/

#if !( ARDUINO_ARCH_NRF52840 && TARGET_NAME == ARDUINO_NANO33BLE )
  #error This code is designed to run on nRF52-based Nano-33-BLE boards using mbed-RTOS platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "NRF52TimerInterrupt.h"
// For Nano33-BLE, don't use Serial.print() in ISR as system will definitely hang.
#define NRF52_MBED_TIMER_INTERRUPT_DEBUG      0

#include "NRF52_MBED_TimerInterrupt.h"

//#ifndef LED_BUILTIN
//  #define LED_BUILTIN         D13
//#endif

#ifndef LED_BLUE_PIN
  #define LED_BLUE_PIN          D7
#endif

#ifndef LED_RED_PIN
  #define LED_RED_PIN           D8
#endif

#define TIMER0_INTERVAL_MS        500   //1000
#define TIMER1_INTERVAL_MS        2000

volatile uint32_t Timer0Count = 0;
volatile uint32_t Timer1Count = 0;

// Depending on the board, you can select NRF52 Hardware Timer from NRF_TIMER_1,NRF_TIMER_3,NRF_TIMER_4 (1,3 and 4)
// If you select the already-used NRF_TIMER_0 or NRF_TIMER_2, it'll be auto modified to use NRF_TIMER_1

// Init NRF52 timer NRF_TIMER1
NRF52_MBED_Timer ITimer0(NRF_TIMER_4);

// Init NRF52 timer NRF_TIMER3
NRF52_MBED_Timer ITimer1(NRF_TIMER_3);

void printResult(uint32_t currTime)
{
  Serial.printf("Time = %ld, Timer0Count = %lu, , Timer1Count = %lu\n", currTime, Timer0Count, Timer1Count);
}

void TimerHandler0(void)
{
  static bool toggle0 = false;

  // Flag for checking to be sure ISR is working as SErial.print is not OK here in ISR
  Timer0Count++;

  //timer interrupt toggles pin LED_BUILTIN
  digitalWrite(LED_BUILTIN, toggle0);
  toggle0 = !toggle0;
}

void TimerHandler1(void)
{
  static bool toggle1 = false;

  // Flag for checking to be sure ISR is working as Serial.print is not OK here in ISR
  Timer1Count++;
  
  //timer interrupt toggles outputPin
  digitalWrite(LED_BLUE_PIN, toggle1);
  toggle1 = !toggle1;
}

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_BLUE_PIN, OUTPUT);
  
  Serial.begin(115200);
  while (!Serial);

  delay(100);

  Serial.printf("\nStarting Change_Interval on %s\n", BOARD_NAME);
  Serial.printf("Version : v%s\n", NRF52_MBED_TIMER_INTERRUPT_VERSION);
 
  // Interval in microsecs
  if (ITimer0.attachInterruptInterval(TIMER0_INTERVAL_MS * 1000, TimerHandler0))
  {
    Serial.printf("Starting  ITimer0 OK, millis() = %ld\n", millis());
  }
  else
    Serial.println("Can't set ITimer0. Select another freq. or timer");

  // Interval in microsecs
  if (ITimer1.attachInterruptInterval(TIMER1_INTERVAL_MS * 1000, TimerHandler1))
  {
    Serial.printf("Starting  ITimer1 OK, millis() = %ld\n", millis());
  }
  else
    Serial.println("Can't set ITimer1. Select another freq. or timer");
}

#define CHECK_INTERVAL_MS     10000L
#define CHANGE_INTERVAL_MS    20000L

void loop()
{
  static uint32_t lastTime = 0;
  static uint32_t lastChangeTime = 0;
  static uint32_t currTime;
  static uint32_t multFactor = 0;

  currTime = millis();

  if (currTime - lastTime > CHECK_INTERVAL_MS)
  {
    printResult(currTime);
    lastTime = currTime;

    if (currTime - lastChangeTime > CHANGE_INTERVAL_MS)
    {
      //setInterval(unsigned long interval, timerCallback callback)
      multFactor = (multFactor + 1) % 2;
      
      ITimer0.setInterval(TIMER0_INTERVAL_MS * 1000 * (multFactor + 1), TimerHandler0);
      ITimer1.setInterval(TIMER1_INTERVAL_MS * 1000 * (multFactor + 1), TimerHandler1);

      Serial.printf("Changing Interval, Timer0 = %lu,  Timer1 = %lu\n", TIMER0_INTERVAL_MS * (multFactor + 1), TIMER1_INTERVAL_MS * (multFactor + 1));
      
      lastChangeTime = currTime;
    }
  }
}
