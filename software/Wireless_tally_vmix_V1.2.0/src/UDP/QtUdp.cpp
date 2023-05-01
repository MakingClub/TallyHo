#include "../main.h"
#include "../config.h"
#include "EEPROM/ConfigData.h"
#include "string.h"

void onPacketCallBack(AsyncUDPPacket packet)
{
#if UDPDebug
    Serial.print("UDP pack: ");
    Serial.println(packet.isBroadcast() ? "Broadcast:" : (packet.isMulticast() ? "All" : "One"));
    Serial.print("remotePort: ");
    Serial.print(packet.remoteIP());
    Serial.print(":");
    Serial.println(packet.remotePort());
    Serial.print("localIP: ");
    Serial.print(packet.localIP());
    Serial.print(":");
    Serial.println(packet.localPort());
    Serial.print("Data length: ");
    Serial.println(packet.length());
    Serial.print("Data data: ");
    Serial.write(packet.data(), packet.length());
    Serial.println();
#endif
    bool saveFlag = 0;
    // 申请回复数组
    char* ReplyData = (char*)malloc(128);
    // static char ReplyData[128];
    memset(ReplyData, 0, strlen(ReplyData));
    // 申请数据转换数组
    char* tmpStr = (char*)malloc(packet.length() + 1);
    memcpy(tmpStr, packet.data(), packet.length());
    tmpStr[packet.length()] = '\0';
    // int i = strcmp(tmpStr, "vMix");
    // int n = strcmp(tmpStr, "vMix\0");
    // sprintf(ReplyData, "send: %d, rev: %s, num1 %d, num2 %d",packet.data() , tmpStr, i, n);
    // 数据匹配
    if(strcmp(tmpStr, "vMix") == 0) {  // 搜索消息
        sprintf(ReplyData, "{\"ip\":\"%s\",\"id\":%d,\"power\":%d,\"vip\":\"%s\",\"RSSI\":%d}",
                WiFi.localIP().toString().c_str(), setting.tallyNumber, Var.BatPrecent, 
                setting.hostName, Var.WiFiRSSI);
    }
    else if (strcmp(tmpStr, "Blink") == 0) {  // 设备闪烁消息
       Var.blinkerEnable = 1;
    }
    else if(strstr(tmpStr, "id")) {  // 配置设备id消息
        saveFlag = 1;
        char* idStr = (char*)malloc(3);
        uint8_t newid = 0;
        idStr[2] = '\0';
        strncpy(idStr, tmpStr+2, packet.length()-2);
        for(int i=0;i<(strlen(idStr)-1);i++){
            newid = newid*10+(idStr[i]-'0');
        }
        setting.tallyNumber = newid;
        sprintf(ReplyData, "{\"newid\": %d}", setting.tallyNumber);
        free(idStr);
    }
    else if(strstr(tmpStr, "ip")) {  // 配置设备ip消息 ip1
        saveFlag = 1;
        char* ipStr = (char*)malloc(16);
        ipStr[packet.length()-2] = '\0';
        strncpy(ipStr, tmpStr+2, packet.length()-2);
        // setting.tallyNumber = newid;
        strcpy(setting.hostName, ipStr);
        sprintf(ReplyData, "{\"newip\": %s}", setting.hostName);
        free(ipStr);
    }
    if (saveFlag)
    {
        saveFlag = 0;
        SavaConfig(setting);
    }
    
    // 消息回复
    if (strlen(ReplyData) != 0) {
        packet.print(ReplyData);
        packet.remotePort();
    }
    free(tmpStr);  // 释放内存
    free(ReplyData);  // 释放内存
    //   broadcastPort = packet.remotePort();
}


void UdpInit(void)
{
    while(!UDP.listen(UdpPort));  // 等待udp监听设置成功
    UDP.onPacket(onPacketCallBack);  // 注册收到数据包事件
}