/****************************************************************************************************************************
  multiFileProject.h
  For ESP8266 boards
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/ESP8266TimerInterrupt
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#pragma once

// These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
// _TIMERINTERRUPT_LOGLEVEL_ from 0 to 4
// Don't define _TIMERINTERRUPT_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
// Don't define TIMER_INTERRUPT_DEBUG > 2. Only for special ISR debugging only. Can hang the system.
#define TIMER_INTERRUPT_DEBUG         2
#define _TIMERINTERRUPT_LOGLEVEL_     0

// Select a Timer Clock
#define USING_TIM_DIV1                false           // for shortest and most accurate timer
#define USING_TIM_DIV16               false           // for medium time and medium accurate timer
#define USING_TIM_DIV256              true            // for longest timer but least accurate. Default

// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "ESP8266TimerInterrupt.h"
#include "ESP8266_ISR_Timer.hpp"

extern ESP8266_ISR_Timer ISR_Timer;  // declaration of the global variable ISRTimer

void setupISR();
