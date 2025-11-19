#ifndef SENSORS_H
#define SENSORS_H

#include <Arduino.h>
#include "config.h"

void sensorsInit();
int readMuxChannel(int ch);
void calibrateSensorsAuto(); // rotate and capture min/max, then save to EEPROM
void saveCalibrationToEEPROM();
void loadCalibrationFromEEPROM();
void getNormalized(int outNorm[]); // 0..1000 normalized
int computeWeightedPosition(); // returns -5500..+5500
void readRawOnce(); // helper to update sensorRaw

extern int sensorRaw[SENSOR_COUNT];
extern int sensorMin[SENSOR_COUNT];
extern int sensorMax[SENSOR_COUNT];

#endif
