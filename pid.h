#ifndef PID_H
#define PID_H

#include <Arduino.h>
#include "config.h"

void pidInit();
void pidSetTunings(float kp, float ki, float kd);
void pidSetBaseSpeed(int spd);
void pidRunStep(); // call frequently
void pidStop();

extern volatile float KP, KI, KD;
extern volatile int BASE_SPEED;
extern volatile int LAST_ERROR;

#endif
