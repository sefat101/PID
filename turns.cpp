#include "turns.h"
#include "sensors.h"
#include "motors.h"
#include "config.h"
#include <Arduino.h>
#include "eeprom_utils.h"
#include "pid.h"

// Simple heuristics to detect sharp turns or junctions and perform safe maneuvers.
// These are intentionally simple and time-based because exact degrees require calibration.

void detectAndHandleTurns() {
    // Read normalized values quickly
    int norm[SENSOR_COUNT];
    getNormalized(norm);

    // Count active sensors on sides
    int leftOuter = norm[0] + norm[1] + norm[2];
    int rightOuter = norm[9] + norm[10] + norm[11];
    int center = norm[5] + norm[6];

    // T-junction or wide intersection: many sensors active
    int activeCount = 0;
    for (int i=0;i<SENSOR_COUNT;i++) if (norm[i] > 500) activeCount++;

    if (activeCount >= 10) {
        // likely intersection: go straight a bit, then prefer default (straight)
        setMotorPWM(BASE_SPEED, BASE_SPEED);
        delay(120);
        return;
    }

    // Very sharp left: outer left sensors strong and center weak
    if (leftOuter > rightOuter * 2 && center < 300) {
        // perform a sharper left pivot
        setMotorPWM(-120, 120); // pivot approx
        delay(150); // adjust ms for desired angle
        return;
    }

    // Very sharp right
    if (rightOuter > leftOuter * 2 && center < 300) {
        setMotorPWM(120, -120);
        delay(150);
        return;
    }

    // gentle correction for 15-60 deg:
    if (leftOuter > rightOuter * 1.4) {
        // soft left
        setMotorPWM(BASE_SPEED - 40, BASE_SPEED + 40);
        delay(30);
        return;
    }
    if (rightOuter > leftOuter * 1.4) {
        // soft right
        setMotorPWM(BASE_SPEED + 40, BASE_SPEED - 40);
        delay(30);
        return;
    }

    // otherwise, do nothing here; PID will handle normal steering
}
