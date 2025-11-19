#include <Arduino.h>
#include "config.h"
#include "eeprom_utils.h"
#include "motors.h"
#include "sensors.h"
#include "pid.h"
#include "turns.h"

// Button debounce
unsigned long lastBtnTime = 0;
bool btnState = false;
bool lastBtnState = false;

// state
enum RobotState { STARTUP_CAL, READY_RUN, RUNNING, RECOVERING };
RobotState state = STARTUP_CAL;

void setup() {
    Serial.begin(115200);

    pinMode(BTN_PIN, INPUT_PULLUP);

    motorsInit();
    sensorsInit();
    pidInit();

    Serial.println(F("=== LineFollower MUX12 ==="));
    Serial.println(F("Loading previous calibration from EEPROM (if any)."));
    loadCalibrationFromEEPROM();

    Serial.println(F("Auto-calibrating sensors for 3 seconds (rotating)."));
    calibrateSensorsAuto();   // rotates for 3s and saves to EEPROM
    loadCalibrationFromEEPROM();
    Serial.println(F("Calibration saved."));

    // initial PID tuning (edit config.h defaults manually if needed)
    pidSetTunings(DEFAULT_KP, DEFAULT_KI, DEFAULT_KD);
    pidSetBaseSpeed(DEFAULT_BASE_SPEED);

    state = READY_RUN;
}

void performRecovery() {
    Serial.println(F("Recovery: button pressed, searching line..."));
    stopMotors();
    delay(100);

    // Back off
    setMotorPWM(-100, -100);
    delay(250);
    stopMotors();
    delay(100);

    // pivot search both directions
    bool found = false;
    unsigned long t0 = millis();
    int attempts = 0;
    while (millis() - t0 < 4000UL && attempts < 12) {
        // clockwise pivot
        setMotorPWM(100, -100);
        unsigned long s = millis();
        while (millis() - s < 250UL) {
            int norm[SENSOR_COUNT];
            getNormalized(norm);
            int active = 0;
            for (int i=0;i<SENSOR_COUNT;i++) if (norm[i] > 500) active++;
            if (active >= 1) { found = true; break; }
        }
        stopMotors();
        if (found) break;
        delay(60);

        // ccw pivot
        setMotorPWM(-100, 100);
        s = millis();
        while (millis() - s < 250UL) {
            int norm[SENSOR_COUNT];
            getNormalized(norm);
            int active = 0;
            for (int i=0;i<SENSOR_COUNT;i++) if (norm[i] > 500) active++;
            if (active >= 1) { found = true; break; }
        }
        stopMotors();
        if (found) break;
        delay(60);
        attempts++;
    }

    if (!found) {
        // small spiral forward searching
        unsigned long s0 = millis();
        while (millis() - s0 < 3000UL) {
            setMotorPWM(110, 90);
            int norm[SENSOR_COUNT];
            getNormalized(norm);
            int active = 0;
            for (int i=0;i<SENSOR_COUNT;i++) if (norm[i] > 500) active++;
            if (active > 0) { found = true; break; }
        }
        stopMotors();
    }

    if (found) {
        Serial.println(F("Line found. Resuming."));
    } else {
        Serial.println(F("Recovery failed."));
    }
}

void loop() {
    // read button (debounced simple)
    bool rawBtn = digitalRead(BTN_PIN) == LOW;
    if (rawBtn != lastBtnState) {
        lastBtnTime = millis();
    }
    if ((millis() - lastBtnTime) > 50) {
        if (rawBtn != btnState) {
            btnState = rawBtn;
            if (btnState == true) {
                // button pressed action
                performRecovery(); // doesn't recalibrate; uses stored calibration
            }
        }
    }
    lastBtnState = rawBtn;

    // Run PID at ~50Hz
    static unsigned long lastPid = 0;
    if (millis() - lastPid >= 20) {
        lastPid = millis();

        // let turns handler have priority for strong patterns
        detectAndHandleTurns();

        pidRunStep();

        // for debugging: print error every 200ms
        static unsigned long lastDbg = 0;
        if (millis() - lastDbg > 200) {
            int pos = computeWeightedPosition();
            Serial.print("ERR=");
            Serial.print(pos);
            Serial.print(" KP=");
            Serial.print(KP);
            Serial.print(" KI=");
            Serial.print(KI);
            Serial.print(" KD=");
            Serial.print(KD);
            Serial.print(" SPD=");
            Serial.println(BASE_SPEED);
            lastDbg = millis();
        }
    }
}
