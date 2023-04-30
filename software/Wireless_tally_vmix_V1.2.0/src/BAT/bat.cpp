#include "bat.h"
#include "../main.h"
#include "../config.h"

#define BATEN(x) x?digitalWrite(BvEnPin, LOW):digitalWrite(BvEnPin, HIGH)
/**---------------------------------------------------------------------------
 * @brief  : This is BAT init function
 * @param  : None
 * @retval : None
 *-----------------------------------------------------------------------------*/
void BatInit(void){
    pinMode(BvEnPin, OUTPUT);
    BATEN(0);
}
/**---------------------------------------------------------------------------
 * @brief  : This is BAT get function
 * @param  : None
 * @retval : None
 *-----------------------------------------------------------------------------*/
uint8_t BatGet(void){
    uint8_t BatPrecent;
    BATEN(1);
    delay(100);
    BatPrecent = 420.0/((analogRead(BvPin)/1023.0)*BvGain);
    BATEN(0);
    return BatPrecent;
}
