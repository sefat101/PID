#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

void motorsInit();
void setMotorPWM(int rightPwm, int leftPwm); // signed: + forward, - reverse
void stopMotors();

#endif
