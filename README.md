## Introduction
There are two players in this tiny game, each player competes by repeatedly clicking a button to advance their position on a circular track represented by 16 colorful LEDs (WS2812). Each player’s position is visually indicated by a distinct LED color, which moves forward with every button press. The objective is to outpace the opponent by completing more laps around the LED circle.
If a player manages to move faster than their opponent by more than one full loop, the game concludes, and the LEDs display the winning player’s color, celebrating their victory. This dynamic and visually stimulating game tests both speed and strategy, providing an exciting challenge for players. This game can be further upgrade to be a sport game by using pedometer instead of clicking button. Free free to enjoy this game :)

[![License: GPL v3](https://img.shields.io/badge/License-GPL_v3-blue.svg)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/LICENSE)

<a href="https://www.buymeacoffee.com/teamprof" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 28px !important;width: 108px !important;" ></a>


---
## Hardware
The following components are required for this project:
1. [Seeed Studio XIAO ESP32C3](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html)
2. [NeoPixel Ring - 16 x RGB LED (WS2812B)](https://www.adafruit.com/product/1463)
3. [Three big buttons module](https://www.aliexpress.com/item/1005005637908638.html?spm=a2g0o.productlist.main.43.545c4a8fnFXp6i&algo_pvid=662588ed-2e06-4555-a694-3d790b5860e3&algo_exp_id=662588ed-2e06-4555-a694-3d790b5860e3-21&pdp_npi=4%40dis%21HKD%217.20%217.20%21%21%210.92%210.92%21%4021015b7d17251967918484648ebeea%2112000033841134088%21sea%21HK%210%21ABX&curPageLogUid=lfjMxpXyBuAb&utparam-url=scene%3Asearch%7Cquery_from%3A)

---
## Software 
1. Install [Arduino IDE 2.0+ for Arduino](https://www.arduino.cc/en/Main/Software)
2. Install [ArduProf lib 1.3+](https://www.arduino.cc/reference/en/libraries/arduprof/)
3. Install [Arduino DebugLog lib](https://www.arduino.cc/reference/en/libraries/debuglog/)
4. Install [Arduino FastLED lib](https://www.arduino.cc/reference/en/libraries/fastled/)
5. Clone this github-esp32c3-tiny-game code by "git clone https://github.com/teamprof/github-esp32c3-tiny-game"

---


## Build and run firmware on XIAO ESP32C3
1. Clone this repository by "git clone https://github.com/teamprof/github-esp32c3-tiny-game"
2. Launch Arduino and open the project
3. Wiring XIAO ESP32C3, LED and buttons as below  
[![schematic](/doc/image/schematic.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/schematic.png)
4. Build and upload the firmware in Arduino IDE.  
   If everythong goes smooth, you should see the following in "Monitor"
 [![running](/doc/image/run.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/run.png)  
    Picture of Game stop
 [![game-stop](/doc/image/game-stop.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/game-stop.png)  
5. Click button "Game" to start a new game  
   Picture of Game start  [![game-start](/doc/image/game-start.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/game-start.png)  
6. Each player clicks its corresponding button. The LEDs labeled ‘Green’ and ‘Blue’ represent player 1 and player 2, respectively.
7. Once a player wins, the LED shows its corresponding color.   
   Picture of Game player1 win
 [![game-win-player1](/doc/image/game-win-player1.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/game-win-player1.png)  
   Picture of Game player2 win
 [![game-win-player2](/doc/image/game-win-player2.png)](https://github.com/teamprof/github-esp32c3-tiny-game/blob/main/doc/image/game-win-player2.png)  

---

## LED colors on different state
```
    +------------------+------------+------------------------------+
    | LED color        | state      | status                       |
    +------------------+------------+------------------------------+
    | red on (all)     | game stop  | no winner                    |
    | red blink (all)  | game pause | no winner                    |
    | green on (all)   | game stop  | player1 win                  |
    | green on         | game pause | player1 position             |
    | blue on (all)    | game stop  | player2 win                  |
    | blue on          | game pause | player2 position             |
    | green/blue blink | game start | player1 and player2 position |
    +------------------+------------+------------------------------+
```

---

## Code explanation in high level
There are two tasks in this project. The first one is "QueueMain" and the second one is "ThreadGame".  
"QueueMain" handles hardware events, such as when a player clicks a button, while "ThreadGame" handles game events, such as advancing a player’s position. "RoundLed" is responsible for LEd indication.

### Please refer to source code for details

---

## Demo
Video demo is available on [esp32c3-tiny-game](https://youtu.be/DdHr8qefJhs)  


---
### Debug
Enable or disable log be defining/undefining macro on "src/app/AppLog.h"

Debug is disabled by "#undef DEBUG_LOG_LEVEL"
Enable trace debug by "#define DEBUG_LOG_LEVEL Debug"

Example of AppLog.h
```
// enable debug log by defining the following macro
#define DEBUG_LOG_LEVEL Debug
// disable debug log by comment out the macro DEBUG_LOG_LEVEL 
// #undef DEBUG_LOG_LEVEL
```
---
### Troubleshooting
If you get compilation errors, more often than not, you may need to install a newer version of the coralmicro.

Sometimes, the project will only work if you update the board core to the latest version because I am using newly added functions.

---
### Issues
Submit issues to: [arduino-esp32c3-tiny-game issues](https://github.com/teamprof/github-esp32c3-tiny-game/issues) 

---
### TO DO
1. Search for bug and improvement.
---

### Contributions and Thanks
Many thanks to the following authors who have developed great software and libraries.
1. [Seeed Studio XIAO ESP32C3](https://www.seeedstudio.com/Seeed-XIAO-ESP32C3-p-5431.html)
2. [DebugLog](https://github.com/hideakitai/DebugLog)
3. [Arduino FastLED lib](https://www.arduino.cc/reference/en/libraries/fastled/)

#### Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this project.
---

### Contributing
If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library
---

### License
- The project is licensed under GNU GENERAL PUBLIC LICENSE Version 3
---

### Copyright
- Copyright 2024 teamprof.net@gmail.com. All rights reserved.



