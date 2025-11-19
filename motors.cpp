#include "motors.h"
#include "config.h"
#include <Arduino.h>

void motorsInit() {
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
}

void setMotorPWM(int rightPwm, int leftPwm) {
    rightPwm = constrain(rightPwm, -MAX_PWM, MAX_PWM);
    leftPwm  = constrain(leftPwm,  -MAX_PWM, MAX_PWM);

    // Right motor (IN1/IN2 -> ENA)
    if (rightPwm >= 0) {
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        analogWrite(ENA, rightPwm);
    } else {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        analogWrite(ENA, -rightPwm);
    }

    // Left motor (IN3/IN4 -> ENB)
    if (leftPwm >= 0) {
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        analogWrite(ENB, leftPwm);
    } else {
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        analogWrite(ENB, -leftPwm);
    }
}

void stopMotors() {
    analogWrite(ENA, 0);
    analogWrite(ENB, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
}
