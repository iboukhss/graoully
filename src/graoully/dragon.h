#ifndef DRAGON_H
#define DRAGON_H

#include <DFRobot_DF1201S.h>

#include "led.h"
#include "motor.h"

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
    Led smoke;  // Trigger for 2 or 3 seconds max

    DFRobot_DF1201S mp3;

    Motor* allMotors[8];
    Led*   allLeds[3];

    static constexpr int kNumMotors = sizeof(allMotors) / sizeof(allMotors[0]);
    static constexpr int kNumLeds = sizeof(allLeds) / sizeof(allLeds[0]);

    Dragon()
      : wingsTop        ("wings_top", -1, -1, -1, 100.0, 14.0),
        wingsBottom     ("wings_bot", -1, -1, -1, 150.0, 14.0),
        headPan         ("pan",        8, 38, 46, 100.0, 5.5),  // Needs adjustment!
        headTilt        ("tilt",       2, 30, 22, 300.0, 14.0),
        headRoll        ("roll",       9, 39, 47, 50.0, 14.0),
        eyelids         ("eyelids",    4, 32, 24, 50.0, 14.0),  // This is just a guess
        ears            ("ears",      10, 40, 48, 50.0, 14.0),  // Also a guess
        mouth           ("mouth",      3, 31, 23, 50.0, 14.0),

        eyes            ("eyes",   A8),
        breath          ("breath", A0),
        smoke           ("smoke",  A1)
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

        Serial1.begin(115200);
        int retries = 5;

        while (!mp3.begin(Serial1)) {
            delay(1000);
            retries++;
            if (retries > 5) {
                Serial.println(F("ERROR: MP3 module not responding on Serial1. Continuing without audio."));
                break;
            }
        }

        mp3.setVol(15);
        mp3.switchFunction(mp3.MUSIC);
        delay(500);
        mp3.setPlayMode(mp3.SINGLE);
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
        for (int i = 0; i < kNumMotors; i++) {
            allMotors[i]->home();
        }
        waitUntilFinished();
        Serial.println(F("Homing done. Graoully is ready."));
    }

    void reset() {
        stop();
        headTilt.moveTo(0.5);
        headPan.moveTo(0.5);
        headRoll.moveTo(0.5);
        mouth.moveTo(0);
        ears.moveTo(1);
        eyelids.moveTo(1);
        eyes.on();
        breath.off();
        waitUntilFinished();
    }
};

#endif
