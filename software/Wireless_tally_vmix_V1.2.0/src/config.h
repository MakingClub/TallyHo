#ifndef CONFIG_H
#define CONFIG_H

#include <ESP8266WiFi.h>

#define Vers "1.2.1"
// debug 
//#define VMIXDEBUG
//#define SMARTCONFIGDEBUG
//#define INFODEBUG
#define WifiConnect     0
#define WaitvMixConnect 1
#define vMixConnecting  2
#define vMixConnected   3
#define vMixApConnect   4
#define vMixPort 8099

#define UdpPort 2333  // udp端口号
// WIFI SMART CONFIG
#define SMARTCONFIGLOOP 20
#define SMARTCONFIGTIME 1000


// LED Pin Define
#define RGBLEDMode 0
#define WS2812Mode 1
#if RGBLEDMode
#define GreenPin 13
#define RedPin   12
#define BluePin  14
#elif WS2812Mode
#define WS2812Pin 14
#define WS2812Num 1
#endif

#define KeyPin   4

#define BvEnPin  16
#define BvPin    17  // A0
#define BvGain   6.1  // 510K+100K/100K


#endif /* CONFIG_H */
