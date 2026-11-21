#ifndef DRAGON_H
#define DRAGON_H

#include "led.h"
#include "motor.h"
#include "mp3_module.h"

struct Dragon {
    Motor wingsTop;         // Opening of the wing
    Motor wingsBottom;      // Up/down wing movement
    Motor headPan;          // Left/right rotation
    Motor headTilt;         // Up/down nod
    Motor headRoll;         // Side to side twist (see camera_movements.jpg)
    Motor eyelids;
    Motor ears;
    Motor mouth;
 
    Led eyes;
    Led breath;
    Led smoke;              // Trigger for 2 or 3 seconds max

    Mp3Module mp3;

    Motor* allMotors[8];
    Led*   allLeds[3];

    static constexpr int kNumMotors = sizeof(allMotors) / sizeof(allMotors[0]);
    static constexpr int kNumLeds = sizeof(allLeds) / sizeof(allLeds[0]);

    Dragon()
      : wingsTop        ("wings_top", -1, -1, -1, 100.0, 14.0),
        wingsBottom     ("wings_bot", -1, -1, -1, 150.0, 14.0),
        headPan         ("pan",        8, 38, 46, 100.0, 5.5),  // Needs adjustment!
        headTilt        ("tilt",       2, 30, 22, 300.0, 14.0),
        headRoll        ("roll",       9, 39, 47, 50.0, 13.0),
        eyelids         ("eyelids",    4, 32, 24, 50.0, 14.0),  // This is just a guess
        ears            ("ears",      10, 40, 48, 50.0, 14.0),  // Also a guess
        mouth           ("mouth",      3, 31, 23, 50.0, 14.0),

        eyes            ("eyes",   A8),
        breath          ("breath", A0),
        smoke           ("smoke",  A1),

        mp3             (Serial1)
    {
        allMotors[0] = &wingsTop;
        allMotors[1] = &wingsBottom;
        allMotors[2] = &headPan;
        allMotors[3] = &headTilt;
        allMotors[4] = &headRoll;
        allMotors[5] = &eyelids;
        allMotors[6] = &ears;
        allMotors[7] = &mouth;

        allLeds[0] = &eyes;
        allLeds[1] = &breath;
        allLeds[2] = &smoke;
    }

    void begin() {
        for (int i = 0; i < kNumMotors; i++) {
            allMotors[i]->begin();
        }

        for (int i = 0; i < kNumLeds; i++) {
            allLeds[i]->begin();
        }

        mp3.begin();
    }

    void update() {
        for (int i = 0; i < kNumMotors; i++) {
            allMotors[i]->update();
        }

        for (int i = 0; i < kNumLeds; i++) {
            allLeds[i]->update();
        }
    }

    void stop() {
        Serial.println(F("WARNING: Initated emergency stop."));
        for (int i = 0; i < kNumMotors; i++) {
            allMotors[i]->stop();
        }
    }

    bool isAnyMotorMoving() {
        for (int i = 0; i < kNumMotors; i++) {
            if (allMotors[i]->isMoving()) return true;
        }
        return false;
    }

    void waitUntilFinished() {
        while (isAnyMotorMoving()) {
            update();
        }
    }

    void home() {
        stop();

        Serial.println(F("INFO: Initiating homing sequence. Waiting..."));
        for (int i = 0; i < kNumMotors; i++) {
            allMotors[i]->home();
        }

        waitUntilFinished();
        Serial.println(F("Homing done. Graoully is ready."));
    }

    void reset() {
        stop();
        headTilt.moveTo(0.3);
        headPan.moveTo(0.7);
        headRoll.moveTo(0.5);
        mouth.moveTo(0);
        ears.moveTo(1);
        eyelids.moveTo(1);
        eyes.off();
        breath.off();
        waitUntilFinished();
    }
};

#endif
