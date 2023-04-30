#ifndef LED_H
#define LED_H

#include <Arduino.h>

void LedInit(void);
void LedOnAll(void);
void LedOffAll(void);
void LedClearTog(void);
void LedTog(uint8_t R, uint8_t G, uint8_t B, uint8_t Brightness);
void LedSet(uint8_t R, uint8_t G, uint8_t B, uint8_t Brightness);
    
void tallySetOff(void);
void tallySetProgram(void);
void tallySetPreview(void);
void tallySetNoConnect(void);
void tallySetConnecting(void);
#endif /* LED_H */
