#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// ---------- MUX ----------
#define MUX_S0 2
#define MUX_S1 3
#define MUX_S2 4
#define MUX_S3 5
#define MUX_SIG A0

// ---------- BUTTON ----------
#define BTN_PIN 6   // use INPUT_PULLUP (press = LOW)

// ---------- MOTOR PINS ----------
#define ENA 9     // PWM Right Motor (ENA)
#define ENB 10    // PWM Left Motor  (ENB)

#define IN1 7     // Right motor dir 1
#define IN2 8     // Right motor dir 2
#define IN3 12    // Left motor dir 1
#define IN4 13    // Left motor dir 2

// ---------- SENSORS ----------
#define SENSOR_COUNT 12

// ---------- PID defaults (edit manually) ----------
#define DEFAULT_KP 13.0
#define DEFAULT_KI 0.0
#define DEFAULT_KD 0.0

// Base speed (edit manually)
#define DEFAULT_BASE_SPEED 50

// Motor PWM limits
#define MAX_PWM 100
#define MIN_PWM 0

// Dead-zone PWM compensation (adjust experimentally)
#define RIGHT_DEAD_PWM 40
#define LEFT_DEAD_PWM 40

// MUX settle time
#define MUX_SETTLE_US 10

#endif
