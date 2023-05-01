# TallyHo
=========

English | [中文](./README.zh-CN.md)

- [Background](#Background of the project)
- [Hardware](#Hardware design)
- [Compile](#Compile the firmware)
- [QT software](#QT software)

This project is a wireless Tally lamp based on Espressif's ESP8266EX chip, with automatic switching between power and battery power, WS2812 for indicator light and LC4054 chip for charging chip.

![Exploded](https://github.com/MakingClub/TallyHo/blob/main/images/Exploded.jpg)

## Background of the project

When live broadcasting or recording video, the Tally light is used for shooting prompts and status indicators in video recording and live broadcasting, indicating the current shooting status through changes in light color and brightness, so as to provide high-definition video recording and live broadcasting efficiency and quality. This project is early in the design of a wireless Tally light based on the Espressif ESP8266EX chip and the Arduino framework.

![Frame](https://github.com/MakingClub/TallyHo/blob/main/images/Frame.png)

## Hardware design

The hardware is developed using Easy EDA. The two-layer board has devices on both sides. You can first use the heating table to weld the front side (with the ESP8266EX chip side), and then use the heat gun to weld the back side.

The relevant hardware has been open sourced in Easy Open Source Square, and the project link is:[征集令 | 无线Tally灯](https://oshwhub.com/lc717863696/wu-xiantally-deng )

## Compile the firmware

This project is based on [vTally - Wireless Tally and VISCA converter for vMix](https://github.com/wasn-eu/vTally), further developed on this basis, the compilation platform uses platformio, please make sure you have been able to succeed Compile the ESP8266 chip program bin file.

### web config

The parameters of TallyHo can be configured through the web, and relevant parameter settings can be made by accessing the IP address of the device.

![webview](https://github.com/MakingClub/TallyHo/blob/main/images/webview.jpg)

### ota update

TallyHo now supports webpage for OTA upgrades, so firmware updates can be performed without opening the 3D model.

![otaview](https://github.com/MakingClub/TallyHo/blob/main/images/otaview.jpg)

## QT software

The QT program is built by Qt Creator 8.0.1, and communicates with TallyHo through udp, and Tally generates different replies according to different udp messages.

![qtview](https://github.com/MakingClub/TallyHo/blob/main/images/qtview.jpg)
