#ifndef DRAGON_H
#define DRAGON_H

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

    Motor* allMotors[8];
    Led*   allLeds[3];

    Dragon()
      : wingsTop        ("wings_top", -1, -1, -1, 100.0, 14.0),
        wingsBottom     ("wings_bot", -1, -1, -1, 150.0, 14.0),
        headPan         ("pan",        8, 38, 46, 100.0, 5.5),  // Needs adjustment!
        headTilt        ("tilt",       2, 30, 22, 300.0, 14.0),
        headRoll        ("roll",       9, 39, 47, 50.0, 14.0),
        eyelids         ("eyelids",    4, 24, 32, 50.0, 14.0),  // This is just a guess
        ears            ("ears",      10, 40, 48, 50.0, 14.0),  // Also a guess
        mouth           ("mouth",      3, 31, 23, 50.0, 14.0),

        eyes            ("eyes",   A8),
        breath          ("breath", -1),
        smoke           ("smoke",  A0)
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
        for (int i = 0; i < sizeof(allMotors) / sizeof(allMotors[0]); i++) {
            allMotors[i]->begin();
        }
    }

    void update() {
        for (int i = 0; i < sizeof(allMotors) / sizeof(allMotors[0]); i++) {
            allMotors[i]->update();
        }
    }

    void stop() {
        for (int i = 0; i < sizeof(allMotors) / sizeof(allMotors[0]); i++) {
            allMotors[i]->stop();
        }
    }

    bool isAnyMotorMoving() {
        for (int i = 0; i < sizeof(allMotors) / sizeof(allMotors[0]); i++) {
            if (allMotors[i]->isMoving()) return true;
        }
        return false;
    }

    void waitUntilFinished() {
        while (isAnyMotorMoving()) {
            update();
        }
    }

    void reset() {
        stop();
        waitUntilFinished();
    }

    void roar() {
        // 0. Mouth closed
        // 1. Tilt forward
        // 2. Open eyes
        // 3. Slight twist to the side
        // 4. Eyes pulse
        // 5. Tilt back
        // 6. Mouth open
        // 7. Smoke machine
        // 8. Eyes flash
    }
};

#endif
