#include <ESP8266WiFi.h>
#include "../main.h"
#include "../LED/led.h"
#include "../config.h"
#include "EEPROM/ConfigData.h"

// 正在执行SmartConfig配网
void SmartConfigDoingCallback(void){
#ifdef SMARTCONFIGDEBUG
    Serial.print(".");
#endif
    LedTog(setting.infored, setting.infogreen, setting.infoblue, setting.intensDim);
}
// SmartConfig配网执行完毕
void SmartConfigDoneCallback(void){
#ifdef SMARTCONFIGDEBUG
    Serial.println("SmartConfig Success");
    Serial.printf("SSID:%s\r\n", WiFi.SSID().c_str());
    Serial.printf("PSW:%s\r\n", WiFi.psk().c_str());
#endif
    
    SaveWiFiInfo(WiFi.SSID(), WiFi.psk());
    LedOffAll();
}
// AutoConfig联网成功
void AutoConfigConnectCallback(void){
#ifdef SMARTCONFIGDEBUG
    Serial.println("WIFI SmartConfig Success");
    Serial.printf("SSID:%s", WiFi.SSID().c_str());
    Serial.printf(", PSW:%s\r\n", WiFi.psk().c_str());
    Serial.print("LocalIP:");
    Serial.print(WiFi.localIP());
    Serial.print(" ,GateIP:");
    Serial.println(WiFi.gatewayIP());
#endif

    LedOffAll();
}
// AutoConfig等待联网
void AutoConfigWaitCallback(int wstatus){
#ifdef SMARTCONFIGDEBUG
    Serial.print("WIFI AutoConfig Waiting......");
    Serial.println(wstatus);
#endif
    LedTog(setting.infored, setting.infogreen, setting.infoblue, setting.intensDim);
}
// AutoConfig联网失败
void AutoConfigFailedCallback(void){
    
#ifdef SMARTCONFIGDEBUG
    Serial.println("WIFI AutoConfig Faild!" );
#endif
}
// 自动配网程序
void SmartConfig(void){
    WiFi.mode(WIFI_STA);
    #ifdef SMARTCONFIGDEBUG
    Serial.println("\r\nWait for Smartconfig...");
    #endif
    WiFi.beginSmartConfig();
    while (1)
    {
        SmartConfigDoingCallback();
        delay(500);                   // wait for a second
        if (WiFi.smartConfigDone())
        {
            SmartConfigDoneCallback();
            break;
        }
    }
}
// 自动联网程序
bool AutoConfig(void){
    WiFi.begin(setting.ssid, setting.pass);
    //如果觉得时间太长可改
    for (int i = 0; i < SMARTCONFIGLOOP; i++)
    {
        int wstatus = WiFi.status();
        if (wstatus == WL_CONNECTED){
            AutoConfigConnectCallback();
            // String Name = "";
            // Name.concat(setting.tallyName);
            // Name.concat("_");
            // Name.concat(String(setting.tallyNumber));

            // WiFi.hostname(Name);
	        return true;
        }
        else{
            AutoConfigWaitCallback(wstatus);
	        delay(SMARTCONFIGTIME);
        }
    }
    AutoConfigFailedCallback();
    return false;
}

