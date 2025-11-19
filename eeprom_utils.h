#ifndef EEPROM_UTILS_H
#define EEPROM_UTILS_H

#include <Arduino.h>
#include "config.h"

void eepromWriteFloat(int addr, float val);
float eepromReadFloat(int addr);
void eepromWriteInt(int addr, int val);
int eepromReadInt(int addr);

// Address layout (bytes)
// We'll store SENSOR_COUNT mins and SENSOR_COUNT max as 2-byte ints each.
// Then store Kp/Ki/Kd floats (4 bytes each) and base speed (2 bytes)
#define ADDR_CAL_MIN 0
#define ADDR_CAL_MAX (ADDR_CAL_MIN + SENSOR_COUNT * 2)
#define ADDR_PID_BASE (ADDR_CAL_MAX + SENSOR_COUNT * 2)
#define ADDR_KP (ADDR_PID_BASE)
#define ADDR_KI (ADDR_KP + 4)
#define ADDR_KD (ADDR_KI + 4)
#define ADDR_BASESPD (ADDR_KD + 4) // store as 2-byte int

#endif
