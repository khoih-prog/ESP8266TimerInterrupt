# ESP8266 TimerInterrupt Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266TimerInterrupt.svg?)](https://www.ardu-badge.com/ESP8266TimerInterrupt)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP8266TimerInterrupt.svg)](https://github.com/khoih-prog/ESP8266TimerInterrupt/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP8266TimerInterrupt/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP8266TimerInterrupt.svg)](http://github.com/khoih-prog/ESP8266TimerInterrupt/issues)

---
---

### Releases v1.0.3

1. Restructure code.
2. Fix example.
3. Enhance README.

### Releases v1.0.2

1. Basic hardware timers for ESP8266.
2. Fix compatibility issue causing compiler error while using Arduino IDEs before 1.8.10 and ESP8266 cores 2.5.2 and before
3. More hardware-initiated software-enabled timers
4. Longer time interval

---
---

## Features

This library enables you to use Interrupt from Hardware Timers on an ESP8266-based board.

#### Why do we need this Hardware Timer Interrupt?

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware timers, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software timers using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software timers, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

#### Important Notes:

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisite

1. [`Arduino IDE 1.8.13+`](https://www.arduino.cc/en/Main/Software)
2. [`ESP8266 core 2.7.4+`](https://github.com/esp8266/Arduino#installing-with-boards-manager) for ESP8266 boards.

---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for `ESP8266TimerInterrupt`, then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266TimerInterrupt.svg?)](https://www.ardu-badge.com/ESP8266TimerInterrupt) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [ESP8266TimerInterrupt](https://github.com/khoih-prog/ESP8266TimerInterrupt) page.
2. Download the latest release `ESP8266TimerInterrupt-master.zip`.
3. Extract the zip file to `ESP8266TimerInterrupt-master` directory 
4. Copy whole `ESP8266TimerInterrupt-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO:

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install **ESP8266TimerInterrupt** library by using [Library Manager](https://docs.platformio.org/en/latest/librarymanager/). Search for ESP8266TimerInterrupt in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)

---
---

## More useful Information

The ESP8266 timers are **badly designed**, using only 23-bit counter along with maximum 256 prescaler. They're only better than UNO / Mega.
The ESP8266 has two hardware timers, but timer0 has been used for WiFi and it's not advisable to use. Only timer1 is available.
The timer1's 23-bit counter terribly can count only up to 8,388,607. So the timer1 maximum interval is very short.
Using 256 prescaler, maximum timer1 interval is only **26.843542 seconds !!!**

The timer1 counters can be configured to support automatic reload.

## New from v1.0.2

Now with these new **`16 ISR-based timers`**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds).

The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers. Therefore, their executions are not blocked by bad-behaving functions / tasks.

This important feature is absolutely necessary for mission-critical tasks. 

The [ISR_Timer_Complex example](examples/ISR_Timer_Complex) will demonstrate the nearly perfect accuracy compared to software timers by printing the actual elapsed millisecs of each type of timers.

Being ISR-based timers, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet and Blynk services. You can also have many `(up to 16)` timers to use. This non-being-blocked important feature is absolutely necessary for mission-critical tasks. 

You'll see blynkTimer Software is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---
---

### HOWTO Use PWM analogWrite() with ESP8266 running Timer1 Interrupt

Please have a look at [ESP8266TimerInterrupt Issue 8: **ESP8266Timer and PWM --> wdt reset**](https://github.com/khoih-prog/ESP8266TimerInterrupt/issues/8) to have more detailed description and solution of the issue.

#### 1. ESP8266 has only 2 hardware timers, named Timer0 and Timer1

#### 2. ESP8266 hardware timers' functions

- Timer0 has been used for WiFi and it's not advisable to use while using WiFi (if not using WiFi, why select ESP8266 ??)
- Timer1 is used by this [**ESP8266TimerInterrupt Library**](https://github.com/khoih-prog/ESP8266TimerInterrupt)

#### 3. How to use PWM analogWrite() functions while using this library

  1. If possible, use software timer instead of [**ESP8266TimerInterrupt Hardware Timer1**](https://github.com/khoih-prog/ESP8266TimerInterrupt) 
  2. If using [**ESP8266TimerInterrupt Hardware Timer1**](https://github.com/khoih-prog/ESP8266TimerInterrupt) is a must, you can either

    - use external DAC such as AD5662, AD5667, AD5696.
    - use software PWM such as mentioned in [**ESP8266 PWM REVISITED (AND REIMPLEMENTED)**](https://lurchi.wordpress.com/2016/06/29/esp8266-pwm-revisited-and-reimplemented/)

---
---

### Examples: 

 1. [Argument_None](examples/Argument_None)
 2. [ISR_RPM_Measure](examples/ISR_RPM_Measure)
 3. [ISR_Switch](examples/ISR_Switch) 
 4. [ISR_Timer_4_Switches](examples/ISR_Timer_4_Switches) 
 5. [ISR_Timer_Complex](examples/ISR_Timer_Complex)
 6. [ISR_Timer_Switch](examples/ISR_Timer_Switch)
 7. [ISR_Timer_Switches](examples/ISR_Timer_Switches) 
 8. [RPM_Measure](examples/RPM_Measure)
 9. [SwitchDebounce](examples/SwitchDebounce)
10. [TimerInterruptTest](examples/TimerInterruptTest)

---

### Supported Boards

- ESP8266

### Example [Argument_None](examples/Argument_None)

```cpp
#if !defined(ESP8266)
#error This code is designed to run on ESP8266 and ESP8266-based boards! Please check your Tools->Board setting.
#endif

//These define's must be placed at the beginning before #include "ESP8266TimerInterrupt.h"
#define TIMER_INTERRUPT_DEBUG      1

#include "ESP8266TimerInterrupt.h"

#ifndef LED_BUILTIN
#define LED_BUILTIN       2         // Pin D4 mapped to pin GPIO2/TXD1 of ESP8266, NodeMCU and WeMoS, control on-board LED
#endif

volatile uint32_t lastMillis = 0;

void ICACHE_RAM_ATTR TimerHandler(void)
{
  static bool toggle = false;
  static bool started = false;

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

#define TIMER_INTERVAL_MS        1000

// Init ESP32 timer 0
ESP8266Timer ITimer;


void setup()
{
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("\nStarting Argument_None");

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(TIMER_INTERVAL_MS * 1000, TimerHandler))
  {
    lastMillis = millis();
    Serial.println("Starting  ITimer OK, millis() = " + String(lastMillis));
  }
  else
    Serial.println("Can't set ITimer correctly. Select another freq. or interval");

}

void loop()
{

}

```

---


The following is the sample terminal output when running example [ISR_Timer_Complex](examples/ISR_Timer_Complex) to demonstrate the accuracy of ISR Hardware Timer, **especially when system is very busy**.  The ISR timer is **programmed for 2s, is activated exactly after 2.000s !!!**

While software timer, **programmed for 2s, is activated after 4.258s !!!**

```
Starting ISR_Timer_Complex
ESP8266TimerInterrupt: _fre = 312500.00, _count = 15625
Starting  ITimer OK, millis() = 64
doingSomething2s: Delta ms = 2000   // ISR_Timer, 2000ms interval, still working even if blocked by WiFi/Blynk connecting tasks
doingSomething2s: Delta ms = 2000
doingSomething5s: Delta ms = 5000
Delta ms = 5000                     //ISR_Timer, 5000ms interval, blynking an LED
doingSomething2s: Delta ms = 2000
[6159] 
    ___  __          __
   / _ )/ /_ _____  / /__
  / _  / / // / _ \/  '_/
 /____/_/\_, /_//_/_/\_\
        /___/ v0.6.1 on NodeMCU

[6161] Protocol connect: timeout =9000
[6164] Connecting to ****.duckdns.org:8080
[6255] Ready (ping: 5ms).
Blynk connected
doingSomething2s: Delta ms = 2000  <=== ISR Timer still very accurate while system busy
blynkDoingSomething2s: Delta programmed ms = 2000, actual = 4258  //Software Timer, 2000ms interval, blocked by WiFi/Blynk connecting tasks
doingSomething2s: Delta ms = 2000
doingSomething5s: Delta ms = 5000
doingSomething10s: Delta ms = 5000
Delta ms = 5000
doingSomething2s: Delta ms = 2000
blynkDoingSomething2s: Delta programmed ms = 2000, actual = 3000  //Software Timer, 2000ms interval, blocked by delay() in loop() to demonstrate bad task.
doingSomething2s: Delta ms = 2000
doingSomething5s: Delta ms = 5000
Delta ms = 5000
blynkDoingSomething2s: Delta programmed ms = 2000, actual = 3001 <=== Software Timer not accurate when system busy
doingSomething2s: Delta ms = 2000
doingSomething2s: Delta ms = 2000
blynkDoingSomething2s: Delta programmed ms = 2000, actual = 3000 <=== Software Timer not accurate when system busy
doingSomething2s: Delta ms = 2000
doingSomething5s: Delta ms = 5000
doingSomething10s: Delta ms = 10000
```
---
---

### Releases v1.0.3

1. Restructure code.
2. Fix example.
3. Enhance README.

### Releases v1.0.2

1. Basic hardware timers for ESP8266.
2. Fix compatibility issue causing compiler error while using Arduino IDEs before 1.8.10 and ESP8266 cores 2.5.2 and before
3. More hardware-initiated software-enabled timers
4. Longer time interval

### Releases v1.0.1

1. Fixing compiler error

### Releases v1.0.0

1. Initial coding

---
---

### Issues ###

Submit issues to: [ESP8266TimerInterrupt issues](https://github.com/khoih-prog/ESP8266TimerInterrupt/issues)

---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards such as SAMD21, SAMD51, SAM-DUE, nRF52

## DONE

1. Similar features for Arduino (UNO, Mega, etc...) and ESP32 

---

### Contributions and thanks

1. Thanks to [Holger Lembke](https://github.com/holgerlembke) to report [ESP8266TimerInterrupt Issue 8: **ESP8266Timer and PWM --> wdt reset**](https://github.com/khoih-prog/ESP8266TimerInterrupt/issues/8), leading to the [HOWTO Use PWM analogWrite() with ESP8266 running Timer1 Interrupt](https://github.com/khoih-prog/ESP8266TimerInterrupt#howto-use-pwm-analogwrite-with-esp8266-running-timer1-interrupt) notes.


<table>
  <tr>
    <td align="center"><a href="https://github.com/holgerlembke"><img src="https://github.com/holgerlembke.png" width="100px;" alt="holgerlembke"/><br /><sub><b>Holger Lembke</b></sub></a><br /></td>
  </tr> 
</table>


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/ESP8266TimerInterrupt/blob/master/LICENSE)

---

## Copyright
Copyright 2019- Khoi Hoang
