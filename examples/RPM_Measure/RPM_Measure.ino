/****************************************************************************************************************************
  RPM_Measure.ino
  For ESP8266 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
  Licensed under MIT license

  The ESP8266 timers are badly designed, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
  The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
  The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
  Using 256 prescaler, maximum timer1 interval is only 26.843542 seconds !!!

  Now with these new 16 ISR-based timers, the maximum interval is practically unlimited (limited only by unsigned long milliseconds)
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

/* Notes:
   Special design is necessary to share data between interrupt code and the rest of your program.
   Variables usually need to be "volatile" types. Volatile tells the compiler to avoid optimizations that assume
   variable can not spontaneously change. Because your function may change variables while your program is using them,
   the compiler needs this hint. But volatile alone is often not enough.
   When accessing shared variables, usually interrupts must be disabled. Even with volatile,
   if the interrupt changes a multi-byte variable between a sequence of instructions, it can be read incorrectly.
   If your data is multiple variables, such as an array and a count, usually interrupts need to be disabled
   or the entire sequence of your code which accesses the data.

   RPM Measuring uses high frequency hardware timer 1Hz == 1ms) to measure the time from of one rotation, in ms
   then convert to RPM. One rotation is detected by reading the state of a magnetic REED SW or IR LED Sensor
   Assuming LOW is active.
   For example: Max speed is 600RPM => 10 RPS => minimum 100ms a rotation. We'll use 80ms for debouncing
   If the time between active state is less than 8ms => consider noise.
   RPM = 60000 / (rotation time in ms)

   You can also use interrupt to detect whenever the SW is active, set a flag
   then use timer to count the time between active state
*/
#if !defined(ESP8266)
  #error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         2
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

#include "ESP8266TimerInterrupt.h"

#define PIN_D1                    5         // Pin D1 mapped to pin GPIO5 of ESP8266

unsigned int SWPin = PIN_D1;

#define TIMER_INTERVAL_MS         1
#define DEBOUNCING_INTERVAL_MS    80

#define LOCAL_DEBUG               1

// Init ESP8266 timer 1
ESP8266Timer ITimer;

volatile unsigned long rotationTime = 0;

float RPM       = 0.00;
float avgRPM    = 0.00;

volatile int debounceCounter;

void IRAM_ATTR TimerHandler()
{
  static bool started = false;

  if (!started)
  {
    started = true;
    pinMode(SWPin, INPUT_PULLUP);
  }

  if ( !digitalRead(SWPin) && (debounceCounter >= DEBOUNCING_INTERVAL_MS / TIMER_INTERVAL_MS ) )
  {
    //min time between pulses has passed
    RPM = (float) ( 60000.0f / ( rotationTime * TIMER_INTERVAL_MS ) );

    avgRPM = ( 2 * avgRPM + RPM) / 3,

#if (LOCAL_DEBUG > 0)
    Serial.print("RPM = ");
    Serial.print(avgRPM);
    Serial.print(", rotationTime ms = ");
    Serial.println(rotationTime * TIMER_INTERVAL_MS);
#endif

    rotationTime = 0;
    debounceCounter = 0;
  }
  else
  {
    debounceCounter++;
  }

  if (rotationTime >= 5000)
  {
    // If idle, set RPM to 0, don't increase rotationTime
    RPM = 0;

#if (LOCAL_DEBUG > 0)
    Serial.print("RPM = ");
    Serial.print(RPM);
    Serial.print(", rotationTime = ");
    Serial.println(rotationTime);
#endif

    rotationTime = 0;
  }
  else
  {
    rotationTime++;
  }
}

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(500);

  Serial.print(F("\nStarting RPM_Measure on "));
  Serial.println(ARDUINO_BOARD);
  Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);
  Serial.print(F("CPU Frequency = "));
  Serial.print(F_CPU / 1000000);
  Serial.println(F(" MHz"));

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK, millis() = "));
    Serial.println(millis());
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));
}

void loop()
{

}
