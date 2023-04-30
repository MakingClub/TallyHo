#include "led.h"
#include "../main.h"
#include "../config.h"
#if WS2812Mode
#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip(WS2812Num, WS2812Pin, NEO_GRB + NEO_KHZ800);
#endif

/**---------------------------------------------------------------------------
 * @brief  : This is led init function
 * @param  : None
 * @retval : None
 *-----------------------------------------------------------------------------*/
void LedInit(void)
{
    strip.begin();
}
/**---------------------------------------------------------------------------
 * @brief  : This is led init function
 * @param  : None
 * @retval : None
 *-----------------------------------------------------------------------------*/
void LedSet(uint8_t R, uint8_t G, uint8_t B, uint8_t Brightness)
{
    strip.setPixelColor(0, R, G, B, Brightness);
    strip.show();
}
/**---------------------------------------------------------------------------
 * @brief  : This is led init function
 * @param  : None
 * @retval : None
 *-----------------------------------------------------------------------------*/
bool LedStates = 0;
void LedTog(uint8_t R, uint8_t G, uint8_t B, uint8_t Brightness)
{
    if (LedStates == 0)
    {
        LedOffAll();
    }
    else
    {
        strip.setPixelColor(0, R, G, B, Brightness);
        strip.show();
    }
    LedStates = 1 - LedStates;
}

void LedClearTog(void)
{
    LedStates = 0;
}

void LedOnAll(void)
{
    strip.setPixelColor(0, 255, 255, 255, 128);
    strip.show();
}

void LedOffAll(void)
{
    strip.setPixelColor(0, 0, 0, 0, 0);
    strip.show();
}
// Set tally to off
void tallySetOff(void)
{
    LedOffAll();
    LedSet(setting.offred, setting.offgreen, setting.offblue, 0);
}

// Set tally to program
void tallySetProgram(void)
{
    // Serial.println(F("| Tally program"));
    // Draw L(ive) with LED's
    // ledSetProgram();
    // send_visca(callLedOn);
    LedOffAll();
    LedSet(setting.prgred, setting.prggreen, setting.prgblue, setting.intensFull);
}

// Set tally to preview
void tallySetPreview(void)
{
    // Serial.println(F("| Tally preview"));

    // ledSetPreview();
    // send_visca(callLedBlink);
    LedOffAll();
    LedSet(setting.prvred, setting.prvgreen, setting.prvblue, setting.intensFull);
}

// Set tally to not connect
void tallySetNoConnect(void)
{
    // Serial.println(F("| Tally program"));
    // ledSetConnecting();

    LedOffAll();
    LedSet(setting.infored, setting.infogreen, setting.infoblue, setting.intensDim);
}

// Set tally to connecting
void tallySetConnecting(void)
{
    // Serial.println(F("| Tally program"));
    // ledSetConnecting();

    LedOffAll();
    LedSet(255, 255, 255, 0);
}