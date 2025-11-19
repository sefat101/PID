#include "eeprom_utils.h"
#include <EEPROM.h>

void eepromWriteFloat(int addr, float val) {
    byte *p = (byte*)(void*)&val;
    for (unsigned int i = 0; i < sizeof(float); i++) {
        EEPROM.update(addr + i, *p++);
    }
}

float eepromReadFloat(int addr) {
    float val = 0.0;
    byte *p = (byte*)(void*)&val;
    for (unsigned int i = 0; i < sizeof(float); i++) {
        *p++ = EEPROM.read(addr + i);
    }
    return val;
}

void eepromWriteInt(int addr, int val) {
    // store 2 bytes (int16)
    byte lo = val & 0xFF;
    byte hi = (val >> 8) & 0xFF;
    EEPROM.update(addr, lo);
    EEPROM.update(addr + 1, hi);
}

int eepromReadInt(int addr) {
    byte lo = EEPROM.read(addr);
    byte hi = EEPROM.read(addr + 1);
    int val = (hi << 8) | lo;
    return val;
}
