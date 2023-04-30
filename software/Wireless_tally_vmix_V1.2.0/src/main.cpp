#include "BAT/bat.h"
#include "KEY/key.h"
#include "LED/led.h"
#include "web/web.h"
#include "vMix/vMix.h"
#include "Udp/QtUdp.h"
#include "EEPROM/ConfigData.h"
#include "SmartConfig/smartconfig.h"

#include "main.h"
#include "config.h"

#include <Ticker.h>

 
Ticker ticker;// 建立Ticker用于实现定时功能

AsyncUDP UDP;  // UDP协议与上位机通讯
// TCP协议物联网通讯
WiFiClient client;  // 与Vimx服务器尝试通信

Variables Var;

Settings defaultSetting = {
    "WirelessTally",
    "12345687",
    "Wireless_Tally",
    "192.168.137.1",
    1,  
    100,  // intensFull
    0,  
    0,  // prgred
    254,
    0,
    254,  // prvred
    0,
    0,
    0,  // offred
    0,
    0,
    0,  // infored
    0,
    254
};

Settings setting;

void BatInfo(void);

void setup() {
    // put your setup code here, to run once:
    //Start Serial
    Serial.begin(115200);

    // infomation outputs
    VersionInfo();

    // load Config
    ConfigDataInit();

    // Init bat measure
    BatInit();

    // Init pins for LED
    LedInit();
    LedOffAll();
    
    // Init pins for KEY
    KeyInit();
    
    // web server init
    webInit();

    // Udp start
    UdpInit();
    BatInfo();  // 获取一次电池信息
    // Start Wifi
    if (!AutoConfig()){
        SmartConfig();
    }
    Var.NowState = WaitvMixConnect;

    // // connect vmix
    // if (WiFi.status() == WL_CONNECTED) {
    //     if(vMixConnect())
    //         Var.NowState = vMixConnected;
    //     else {
    //         LedOffAll();
    //         Var.NowState = WaitvMixConnect;
    //     }
    // }

}

// Time measure
#define interval 10000
unsigned long lastCheck = 0;
void BlinkLed(void);
void KeyControl(void);

void loop() {
    webLoop();  // web server
    KeyLoop();  // key measure
    KeyControl();  

    switch (Var.NowState)
    {
    // try connect wifi
    case WifiConnect:
        // Start Wifi
        if (!AutoConfig()){
            SmartConfig();
        }
        Var.NowState = WaitvMixConnect;
        break;

    // wait start connect vimx 
    case WaitvMixConnect:
        if(Var.blinkerEnable) {  // 启动定时器
            Var.blinkerEnable = 0;
            ticker.attach(1, BlinkLed);
            LedClearTog();    
        }
        
    break;
    // try connect vimx before double press key
    case vMixConnecting:
        webClose();
        ticker.detach();  // 删除定时器
        BatInfo();  // Get BatV and rssi
        if (!client.connected() && !Var.apEnabled && millis() > lastCheck + interval){
            if(vMixConnect())
                Var.NowState = vMixConnected;
            lastCheck = millis();
        }

        if(WiFi.status() != WL_CONNECTED)
            Var.NowState = WifiConnect;
        break;
    
    // connect success
    case vMixConnected:
        ticker.attach(10, BatInfo);  // 启动定时器 任务2
        while (client.available()) {  // tally server 
            String data = client.readStringUntil('\r\n');
            vMixDataHandle(data);
        }   
        if(!client.connected())
            Var.NowState = vMixConnecting;

        if(WiFi.status() != WL_CONNECTED)
            Var.NowState = WifiConnect;
        break;

    default:
        break;
    }
}
void KeyControl(void) {
    switch (Var.keyState)
    {
    case OneClick:
        LedClearTog();
        if (!client.connected()) {
            for (uint8_t i = 0; i < setting.tallyNumber*2 ; i++) {
                LedTog(setting.infored, setting.infogreen, setting.infoblue, setting.intensDim);
                delay(500);
            }
        }
        
        break;
    case DoubleClick:
        if(Var.NowState == WaitvMixConnect)
            Var.NowState = vMixConnecting;
        break;
    case LongClick:
    ticker.detach();  // 删除定时器
    Var.NowState = vMixApConnect;
    if (Var.apEnabled) {
            Restart();
        }
        ApStart();
        break;
    
    default:
        break;
    }
    Var.keyState = NoneClick;
}
void BlinkLed(void) {
    Var.BlinkTime ++;
    LedTog(setting.infored, setting.infogreen, setting.infoblue, setting.intensDim);  // 闪烁
    if (Var.BlinkTime > setting.tallyNumber*2) {
        Var.BlinkTime = 0;
        LedOffAll();
        ticker.detach();  // 删除定时器
    }
    
}

void BatInfo(void) {
        Var.WiFiRSSI = WiFi.RSSI();
        Var.BatPrecent = BatGet();
#ifdef INFODEBUG
    Serial.println(F("+--------------------+"));
    Serial.println(F("| Bat and RSSI info  |"));
    Serial.println(F("+--------------------+"));
    Serial.print(F("| Bat"));
    Serial.print(Var.BatPrecent);
    Serial.print(F("  %"));

    Serial.print(F("| RSSI"));
    Serial.print(Var.WiFiRSSI);
    Serial.print(F("  |"));
#endif
}

void Restart(void) {
#ifdef INFODEBUG
  Serial.println(F(""));
  Serial.println(F("+--------------------+"));
  Serial.println(F("|       RESTART      |"));
  Serial.println(F("+--------------------+"));
  Serial.println(F(""));
#endif
  ESP.restart();
}

// Start access point
void ApStart(void) {
    IPAddress local_IP(192, 168, 4, 2);  // 手动设置的开启的网络的ip地址
    IPAddress gateway(192, 168, 4, 1);  // 手动设置的网关IP地址
    IPAddress subnet(255, 255, 255, 0);  // 手动设置的子网掩码

    String ApSsid = "";
    ApSsid.concat(setting.tallyName);
    ApSsid.concat("_");
    ApSsid.concat(String(setting.tallyNumber));

    String ApPass = setting.tallyName+String(setting.tallyNumber);
#ifdef INFODEBUG
    Serial.println(F("(+--------------------+"));
    Serial.println(F("(|       AP Start     |"));
    Serial.println(F("(+--------------------+"));
    Serial.print(F("| AP SSID         : "));
    Serial.println(ApSsid);
    Serial.print(F("| AP password     : "));
    Serial.println(ApPass);
#endif
    //配置接入点的IP，网关IP，子网掩码
    WiFi.softAPConfig(local_IP, gateway, subnet);

    WiFi.mode(WIFI_AP);
    WiFi.hostname(setting.tallyName);
    WiFi.softAP(ApSsid, ApPass);
    delay(100);
    IPAddress myIP = WiFi.softAPIP();
#ifdef INFODEBUG
    Serial.print(F("| IP address      : "));
    Serial.println(myIP);
    Serial.println(F("+---------------------"));
#endif
    Var.apEnabled = true;
}

void VersionInfo(void) {
#ifdef INFODEBUG
    Serial.println(F(""));
    Serial.println(F(""));
    Serial.println(F("+--------------------+"));
    Serial.print(F("|   vTally  V:"));
    Serial.print(String(Vers));
    Serial.println(F("  |"));
    Serial.println(F("| (c)2022 by VID-PRO |"));
    Serial.println(F("| (c)2023 by Liu-Alex|"));
    Serial.println(F("+--------------------+"));
    Serial.println(F(""));
#endif
}
