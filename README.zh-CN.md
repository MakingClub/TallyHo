
# TallyHo
当作为一个口头语或口号时，“Ho”这个词通常用于表示热情或兴奋的感觉。这个词源于狩猎活动中用来表示发现了猎物的信号，后来逐渐演变为一个热情的口号或鼓舞士气的口号。在这个项目中，使用“Tally-Ho”作为项目名称可以反映对视频录制和直播等工作的热情和兴奋。

![Exploded](https://github.com/MakingClub/TallyHo/blob/main/images/Exploded.jpg)

## 内容列表
- [项目背景](#项目背景)
- [硬件设计](#硬件设计)
- [固件编译](#固件编译)
- [QT软件](#QT软件)


## 项目背景
在直播或录制视频时，Tally灯用于视频录制和直播中的拍摄提示和状态指示，通过灯光颜色和亮度的变化来指示当前拍摄状态，以提高视频录制和直播的效率和质量。本项目旨在设计一种基于乐鑫ESP8266EX芯片和Arduino框架的无线Tally灯。

![Frame](https://github.com/MakingClub/TallyHo/blob/main/images/Frame.png)

## 硬件设计

硬件使用立创EDA进行开发，二层板，双面均有器件，可以先使用加热台焊接正面（带有ESP8266EX芯片面），再使用热风枪焊接背面。

相关硬件已经在立创开源广场开源，项目链接为：[征集令 | 无线Tally灯](https://oshwhub.com/lc717863696/wu-xiantally-deng )

## 固件编译 
这个项目基于[vTally - Wireless Tally and VISCA converter for vMix](https://github.com/wasn-eu/vTally)，在此基础上进行了进一步开发，编译平台使用platformio，请确保你已经能够成功编译ESP8266芯片程序bin文件。

### WEB配置

TallyHo的参数支持通过web进行配置，通过访问设备的IP地址即可进行相关参数设置。

![webview](https://github.com/MakingClub/TallyHo/blob/main/images/webview.jpg)

### OTA升级

TallyHo现已支持网页进行OTA升级，因此无需打开3D模型即可进行固件更新。

![otaview](https://github.com/MakingClub/TallyHo/blob/main/images/otaview.jpg)

## QT软件

QT程序通过Qt Creator 8.0.1构建，与TallyHo经过udp进行通讯，Tally根据不同的udp消息产生不同的回复。

![qtview](https://github.com/MakingClub/TallyHo/blob/main/images/qtview.jpg)

