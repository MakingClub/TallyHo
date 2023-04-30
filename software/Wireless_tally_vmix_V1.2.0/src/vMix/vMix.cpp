#include "vmix.h"
#include "../main.h"
#include "../config.h"

#include "../LED/led.h"


// Connect to vMix instance
bool vMixConnect(void){
#ifdef VMIXDEBUG
    Serial.println(F("+--------------------+"));
    Serial.println(F("| Connecting to vMix |"));
    Serial.println(F("+--------------------+"));
    Serial.print(F("| Connecting to "));
    Serial.println(setting.hostName);
#endif
    tallySetConnecting();
    if (client.connect(setting.hostName, vMixPort)){  // setting.hostName
        #ifdef VMIXDEBUG
        Serial.println(F("| Connected  to vMix"));

        Serial.println(F("+---------------------"));
        Serial.println(F(""));

        Serial.println(F("+--------------------+"));
        Serial.println(F("|  vMix Message Log  |"));
        Serial.println(F("+--------------------+"));
        #endif
        LedOffAll();

        // Subscribe to the tally events
        client.println(F("SUBSCRIBE TALLY"));
        return true;
    }
    else{  // Vimx 服务器连接失败
        Var.vmixcon = 0;
        Var.currentState = '3';
        client.stopAll();
        LedOffAll();
        tallySetNoConnect();
        #ifdef VMIXDEBUG
        Serial.println(F("| vMix Server not found!"));
        Serial.println(F("+---------------------"));
        Serial.println(F(""));
        #endif
        return false;
    }
}

// Handle incoming data
void vMixDataHandle(String data){
    // Check if server data is tally data
    if (data.indexOf("TALLY OK") == 0){
        Var.vmixcon = 1;
        char newState = data.charAt(setting.tallyNumber + 8);

        // Check if tally state has changed
        if (Var.currentState != newState)
        {
            Var.currentState = newState;

            switch (Var.currentState)
            {
            case '0':
                tallySetOff();
                break;
            case '1':
                tallySetProgram();
                break;
            case '2':
                tallySetPreview();
                break;
            default:
                tallySetOff();
                break;
            }
        }
    }
    else{
        Var.vmixcon = 1;
        Serial.print(F("| Response from vMix: "));
        Serial.println(data);
    }
}