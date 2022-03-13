# ESP8266TimerInterrupt Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/ESP8266TimerInterrupt.svg?)](https://www.ardu-badge.com/ESP8266TimerInterrupt)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/ESP8266TimerInterrupt.svg)](https://github.com/khoih-prog/ESP8266TimerInterrupt/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/ESP8266TimerInterrupt/blob/master/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/ESP8266TimerInterrupt.svg)](http://github.com/khoih-prog/ESP8266TimerInterrupt/issues)

---
---

* [Changelog](#changelog)
  * [Releases v1.6.0](#releases-v160)
  * [Releases v1.5.0](#releases-v150)
  * [Releases v1.4.1](#releases-v141)
  * [Releases v1.4.0](#releases-v140)
  * [Releases v1.3.0](#releases-v130)
  * [Releases v1.2.0](#releases-v120)
  * [Releases v1.1.1](#releases-v111)
  * [Releases v1.1.0](#releases-v110)
  * [Releases v1.0.3](#releases-v103)
  * [Releases v1.0.2](#releases-v102)

---
---

## Changelog

### Releases v1.6.0

1. Add example [ISR_16_Timers_Array_OneShot](examples/ISR_16_Timers_Array_OneShot) to demo how to use `one-shot ISR-based timer`
2. Add example [multiFileProject](examples/multiFileProject) to demo for multiple-file project
3. Optimize code by using passing by `reference` instead of by `value`

### Releases v1.5.0

1. Fix `multiple-definitions` linker error. Drop `src_cpp` and `src_h` directories
2. Add feature to select among highest, medium or lowest accuracy for Timers for shortest, medium or longest time
3. Fix reattachInterrupt() bug. Check [bugfix: reattachInterrupt() pass wrong frequency value to setFrequency() #19](https://github.com/khoih-prog/ESP8266TimerInterrupt/pull/19)
4. Update examples

### Releases v1.4.1

1. Examples modified and tested with core v3.0.2
2. Add instructions in `README.md`
3. Add `changelog.md`
4. Delete Blynk-related examples

### Releases v1.4.0

1. Fix compiler errors due to conflict to some libraries.
2. Add complex examples.

### Releases v1.3.0

1. Update to match new ESP8266 core v3.0.0

### Releases v1.2.0

1. Add better debug feature.
2. Optimize code and examples to reduce RAM usage

### Releases v1.1.1

1. Add [**Change_Interval**](examples/Change_Interval) example to show how to change TimerInterval on-the-fly
2. Add Version String

### Releases v1.1.0

1. Restore cpp code besides Impl.h code to use if Multiple-Definition linker error.
2. Update examples.
3. Enhance README.

### Releases v1.0.3

1. Restructure code.
2. Fix example.
3. Enhance README.

### Releases v1.0.2

1. Basic hardware timers for ESP8266.
2. Fix compatibility issue causing compiler error while using Arduino IDEs before 1.8.10 and ESP8266 cores 2.5.2 and before
3. More hardware-initiated software-enabled timers
4. Longer time interval

