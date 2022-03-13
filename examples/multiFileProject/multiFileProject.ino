/****************************************************************************************************************************
  multiFileProject.ino
  For ESP8266 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#ifndef ESP8266
  #error This code is designed to run on ESP8266 platform! Please check your Tools->Board setting.
#endif

#define ESP8266_TIMER_INTERRUPT_VERSION_MIN_TARGET      "ESP8266TimerInterrupt v1.6.0"
#define ESP8266_TIMER_INTERRUPT_VERSION_MIN             1006000

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "ESP8266TimerInterrupt.h"
#include "ESP8266_ISR_Timer.h"


void setup() 
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStart multiFileProject");
  Serial.println(ESP8266_TIMER_INTERRUPT_VERSION);

#if defined(ESP8266_TIMER_INTERRUPT_VERSION_MIN)
  if (ESP8266_TIMER_INTERRUPT_VERSION_INT < ESP8266_TIMER_INTERRUPT_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(ESP8266_TIMER_INTERRUPT_VERSION_MIN_TARGET);
  }
#endif
}

void loop() 
{
  // put your main code here, to run repeatedly:
}
