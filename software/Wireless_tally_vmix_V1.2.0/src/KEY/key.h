#ifndef KEY_H
#define KEY_H

#include <Arduino.h>

#define NoneClick 0
#define OneClick 1
#define DoubleClick 2
#define LongClick 3

void KeyInit(void);
void KeyLoop(void);

#endif /* KEY_H */
