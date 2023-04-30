#include "ConfigData.h"
#include "EEPROM.h"

// Print setting
void settingInfo(void) {
#if INFODEBUG
    Serial.println();
    Serial.print(F("| SSID            : "));
    Serial.println(setting.ssid);
    Serial.print(F("| SSID Password   : "));
    Serial.println(setting.pass);
    Serial.print(F("| vMix Hostname/IP: "));
    Serial.println(setting.hostName);
    Serial.print(F("| Tally number    : "));
    Serial.println(setting.tallyNumber);
    Serial.print(F("| Intensity Full  : "));
    Serial.println(setting.intensFull);
    Serial.print(F("| Intensity Dim   : "));
    Serial.println(setting.intensDim);
    Serial.print(F("| Program-Color   : "));
    Serial.print(setting.prgred);
    Serial.print(F(","));
    Serial.print(setting.prggreen);
    Serial.print(F(","));
    Serial.println(setting.prgblue);
    Serial.print(F("| Preview-Color   : "));
    Serial.print(setting.prvred);
    Serial.print(F(","));
    Serial.print(setting.prvgreen);
    Serial.print(F(","));
    Serial.println(setting.prvblue);
    Serial.print(F("| info-Color       : "));
    Serial.print(setting.infored);
    Serial.print(F(","));
    Serial.print(setting.infogreen);
    Serial.print(F(","));
    Serial.println(setting.infoblue);
    Serial.print(F("| Off-Color       : "));
    Serial.print(setting.offred);
    Serial.print(F(","));
    Serial.print(setting.offgreen);
    Serial.print(F(","));
    Serial.println(setting.offblue);
#endif
}

void ConfigDataInit(void) {
    EEPROM.begin(sizeof(setting)+2); //开启EEPROM，开辟Setting大小空间
    if (EEPROM.read(sizeof(setting)+1) == 1) {
        EEPROM.get(0, setting);
    }
    else {
        EEPROM.put(0, defaultSetting);
        EEPROM.write(sizeof(setting)+1, 1);
        EEPROM.commit();
        delay(200);
        EEPROM.get(0, setting);
    }
    settingInfo();
}

void SaveWiFiInfo(String SSID, String psk) {
    strcpy(setting.ssid, SSID.c_str());
    strcpy(setting.pass, psk.c_str());
    EEPROM.put(0, setting.ssid);
    EEPROM.put(sizeof(setting.ssid), setting.pass);
    EEPROM.commit();
}

void SavaConfig(Settings config){
    settingInfo();
    EEPROM.put(0, config);
    EEPROM.commit();
}