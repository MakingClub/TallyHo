#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESPAsyncUDP.h>
#include <OneButton.h>


struct Variables {
    int vmixcon;
    char currentState;
    char oldState;

    bool apEnabled;
    bool blinkerEnable;
    uint8_t BlinkTime;
    uint8_t keyState;
    uint8_t BatPrecent;
    int32_t WiFiRSSI;

    uint8_t NowState;
};

//Define sturct for holding tally settings (mostly to simplify EEPROM read and write, in order to persist settings)
struct Settings {
    char ssid[24];
    char pass[24];
    char tallyName[32] = "";
    char hostName[24];
    uint8_t tallyNumber;
    uint8_t intensFull;
    uint8_t intensDim;
    // 直播中色值
    uint8_t prgred;
    uint8_t prggreen;
    uint8_t prgblue;
    // 预选色值
    uint8_t prvred;
    uint8_t prvgreen;
    uint8_t prvblue;
    // 关闭色值
    uint8_t offred;
    uint8_t offgreen;
    uint8_t offblue;
    // 指示色值
    uint8_t infored;
    uint8_t infogreen;
    uint8_t infoblue;
};
extern Variables Var;
extern Settings defaultSetting;
extern Settings setting;

extern AsyncUDP UDP;
extern WiFiClient client;

void ApStart(void);
void Restart(void);
void VersionInfo(void);

#endif
