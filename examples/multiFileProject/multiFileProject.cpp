/****************************************************************************************************************************
  multiFileProject.cpp
  For ESP8266 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#include "multiFileProject.h"

ESP8266Timer ITimer;
ESP8266_ISR_Timer ISR_Timer;

void IRAM_ATTR TimerHandler()
{
  ISR_Timer.run();
  Serial.println("TimerHandler triggered");
}

void doingSomething0()
{
  Serial.println("doingSomething0 triggered");
}

void setupISR()
{
  if (ITimer.attachInterruptInterval(1000 * 1000, TimerHandler))
  {
    Serial.print(F("Starting ITimer OK"));
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  ISR_Timer.setInterval(3000, doingSomething0);
}
