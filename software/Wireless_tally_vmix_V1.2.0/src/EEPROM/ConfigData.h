#ifndef ConfigData_H
#define ConfigData_H

#include "../main.h"
#include "../config.h"

void ConfigDataInit(void);
void SavaConfig(Settings config);
void SaveWiFiInfo(String SSID, String psk);

#endif /*ConfigData_H*/