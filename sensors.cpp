#include "sensors.h"
#include "config.h"
#include "motors.h"
#include "eeprom_utils.h"
#include <Arduino.h>

int sensorRaw[SENSOR_COUNT];
int sensorMin[SENSOR_COUNT];
int sensorMax[SENSOR_COUNT];

void sensorsInit() {
    pinMode(MUX_S0, OUTPUT);
    pinMode(MUX_S1, OUTPUT);
    pinMode(MUX_S2, OUTPUT);
    pinMode(MUX_S3, OUTPUT);
    pinMode(MUX_SIG, INPUT);

    // init arrays
    for (int i = 0; i < SENSOR_COUNT; i++) {
        sensorMin[i] = 1023;
        sensorMax[i] = 0;
        sensorRaw[i] = 0;
    }
}

int readMuxChannel(int ch) {
    digitalWrite(MUX_S0, bitRead(ch, 0));
    digitalWrite(MUX_S1, bitRead(ch, 1));
    digitalWrite(MUX_S2, bitRead(ch, 2));
    digitalWrite(MUX_S3, bitRead(ch, 3));
    delayMicroseconds(MUX_SETTLE_US);
    return analogRead(MUX_SIG);
}

void calibrateSensorsAuto() {
    // rotate in-place slowly to expose sensors to both line and background
    unsigned long start = millis();
    // gentle spin: right forward, left backward (pivot)
    setMotorPWM(100, -100);
    while (millis() - start < 3000UL) {
        for (int ch = 0; ch < SENSOR_COUNT; ch++) {
            int v = readMuxChannel(ch);
            if (v < sensorMin[ch]) sensorMin[ch] = v;
            if (v > sensorMax[ch]) sensorMax[ch] = v;
        }
    }
    stopMotors();

    // fallback defaults
    for (int i = 0; i < SENSOR_COUNT; i++) {
        if (sensorMax[i] <= sensorMin[i]) {
            sensorMin[i] = 0;
            sensorMax[i] = 600;
        }
    }

    saveCalibrationToEEPROM();
}

void saveCalibrationToEEPROM() {
    int baseMin = ADDR_CAL_MIN;
    int baseMax = ADDR_CAL_MAX;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        eepromWriteInt(baseMin + i*2, sensorMin[i]);
        eepromWriteInt(baseMax + i*2, sensorMax[i]);
    }
}

void loadCalibrationFromEEPROM() {
    int baseMin = ADDR_CAL_MIN;
    int baseMax = ADDR_CAL_MAX;
    bool anyZero = true;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        int mn = eepromReadInt(baseMin + i*2);
        int mx = eepromReadInt(baseMax + i*2);
        if (mn != 0 || mx != 0) anyZero = false;
        sensorMin[i] = (mn == 0 && mx == 0) ? 0 : mn;
        sensorMax[i] = (mn == 0 && mx == 0) ? 600 : mx;
        if (sensorMax[i] <= sensorMin[i]) {
            sensorMin[i] = 0;
            sensorMax[i] = 600;
        }
    }
    // if EEPROM empty fallback to sensible defaults (already set)
    if (anyZero) {
        for (int i=0;i<SENSOR_COUNT;i++){
            sensorMin[i] = 0;
            sensorMax[i] = 600;
        }
    }
}

void getNormalized(int outNorm[]) {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        int raw = readMuxChannel(i);
        sensorRaw[i] = raw;
        int mn = sensorMin[i];
        int mx = sensorMax[i];
        if (mx == mn) mx = mn + 1;
        int norm = map(raw, mn, mx, 0, 1000);
        norm = constrain(norm, 0, 1000);
        outNorm[i] = norm;
    }
}

void readRawOnce() {
    for (int i = 0; i < SENSOR_COUNT; i++) {
        sensorRaw[i] = readMuxChannel(i);
    }
}

int computeWeightedPosition() {
    // symmetric weights across 12 sensors
    const int weights[SENSOR_COUNT] = {-5500,-4500,-3500,-2500,-1500,-500,500,1500,2500,3500,4500,5500};
    int norm[SENSOR_COUNT];
    getNormalized(norm);
    long num = 0;
    long den = 0;
    for (int i = 0; i < SENSOR_COUNT; i++) {
        num += (long)weights[i] * norm[i];
        den += norm[i];
    }
    if (den == 0) return 0; // lost line => 0
    int pos = (int)(num / den); // -5500..+5500
    return pos;
}
