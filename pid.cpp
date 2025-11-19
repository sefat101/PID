#include "pid.h"
#include "sensors.h"
#include "motors.h"
#include "eeprom_utils.h"
#include "config.h"
#include <Arduino.h>

volatile float KP = DEFAULT_KP;
volatile float KI = DEFAULT_KI;
volatile float KD = DEFAULT_KD;
volatile int BASE_SPEED = DEFAULT_BASE_SPEED;
volatile int LAST_ERROR = 0;

static float integral = 0.0;
static unsigned long lastTime = 0;

void pidInit() {
    // Load PID from EEPROM if you want; currently we keep defaults in code.
    KP = DEFAULT_KP;
    KI = DEFAULT_KI;
    KD = DEFAULT_KD;
    BASE_SPEED = DEFAULT_BASE_SPEED;
    integral = 0;
    lastTime = millis();
}

void pidSetTunings(float kp, float ki, float kd) {
    KP = kp; KI = ki; KD = kd;
}

void pidSetBaseSpeed(int spd) {
    BASE_SPEED = constrain(spd, 0, MAX_PWM);
}

void pidStop() {
    stopMotors();
}

void pidRunStep() {
    unsigned long now = millis();
    float dt = (now - lastTime) / 1000.0;
    if (dt <= 0) dt = 0.01;

    int pos = computeWeightedPosition(); // -5500..+5500
    int error = pos; // setpoint = 0

    integral += error * dt;
    // anti-windup
    if (integral > 1e6) integral = 1e6;
    if (integral < -1e6) integral = -1e6;

    float derivative = (error - LAST_ERROR) / dt;

    float output = KP * error + KI * integral + KD * derivative;

    // Scale output to PWM differential. divisor controls steering intensity.
    const float STEER_SCALE = 40.0; // tune this if steering too weak/strong
    int leftPWM = (int)(BASE_SPEED - (output / STEER_SCALE));
    int rightPWM = (int)(BASE_SPEED + (output / STEER_SCALE));

    // dead-zone compensation
    if (leftPWM > 0 && leftPWM < LEFT_DEAD_PWM) leftPWM = LEFT_DEAD_PWM;
    if (rightPWM > 0 && rightPWM < RIGHT_DEAD_PWM) rightPWM = RIGHT_DEAD_PWM;

    setMotorPWM(rightPWM, leftPWM); // right, left

    LAST_ERROR = error;
    lastTime = now;
}
