
// Motor 1

// Stroke:     50mm
// Max load:   1000N (100kg)
// Speed:      14mm/s
// Duty cycle: 10%

// Approx 3.57 seconds to fully extend/retract

struct Motor {
    int pinEna;
    int pinIn1;
    int pinIn2;

    float strokeMm;
    float speedMmS;

    unsigned long theoreticalTravelTimeMs = 0;
    unsigned long expectedTravelTimeMs = 0;
    unsigned long startTimeMs = 0;

    bool isMoving = false;

    Motor(int ena, int in1, int in2, float stroke, float speed)
      : pinEna(ena), pinIn1(in1), pinIn2(in2), strokeMm(stroke), speedMmS(speed)
    {
        if (speedMmS > 0) {
            theoreticalTravelTimeMs = (strokeMm / speedMmS) * 1000.0;
        }
    }

    void begin() {
        if (pinEna == -1) return;

        pinMode(pinEna, OUTPUT);
        pinMode(pinIn1, OUTPUT);
        pinMode(pinIn2, OUTPUT);
    }

    void update() {
        if (!isMoving) return;

        if (millis() - startTimeMs >= expectedTravelTimeMs) {
            brake();
        }
    }

    void extend() {
        if (pinEna == -1 || isMoving) return;

        // NOTE(isma): We add a flat 30% safety margin, should be more than enough..
        // This value needs to be adjusted with real world testing.
        expectedTravelTimeMs = theoreticalTravelTimeMs * 1.3;

        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 255);

        startTimeMs = millis();
        isMoving = true;
    }

    void extendHalf() {
        if (pinEna == -1 || isMoving) return;

        expectedTravelTimeMs = theoreticalTravelTimeMs / 2;

        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 255);

        startTimeMs = millis();
        isMoving = true;
    }

    void retract() {
        if (pinEna == -1 || isMoving) return;

        expectedTravelTimeMs = theoreticalTravelTimeMs * 1.3;

        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
        analogWrite(pinEna, 255);

        startTimeMs = millis();
        isMoving = true;
    }

    void retractHalf() {
        if (pinEna == -1 || isMoving) return;

        expectedTravelTimeMs = theoreticalTravelTimeMs / 2;

        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
        analogWrite(pinEna, 255);

        startTimeMs = millis();
        isMoving = true;
    }

    void brake() {
        if (pinEna == -1) return;

        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 0);

        isMoving = false;
    }
};

struct Led {
    int pin;

    Led(int p) : pin(p) {}

    void begin() {
        if (pin == -1) return;

        pinMode(pin, OUTPUT);
    }

    void setBrightness(int level) {
        if (pin == -1) return;

        int safeLevel = constrain(level, 0, 255);
        analogWrite(pin, safeLevel);
    }

    void on() {
        setBrightness(255);
    }

    void off() {
        setBrightness(0);
    }

    void fade() {
        for (int i = 0; i <= 255; i += 5) {
            setBrightness(i);
            delay(30);
        }

        delay(1000);

        for (int i = 255; i >= 0; i -= 5) {
            setBrightness(i);
            delay(30);
        }

        delay(1000);
    }
};

struct Dragon {
    Motor leftWingTop;      // Opening of the wing
    Motor leftWingBottom;   // Up/down wing movement
    Motor rightWingTop;     // See above
    Motor rightWingBottom;  // See above
    Motor headPan;          // Left/right rotation
    Motor headTilt;         // Up/down nod
    Motor headRoll;         // Side to side twist (see camera_movements.jpg)
    Motor leftEyelid;
    Motor rightEyelid;
    Motor leftEar;
    Motor rightEar;
    Motor mouth;
 
    Led eyes;
    Led breath;

    int smokeMachinePin;

    Motor* allMotors[12];

    Dragon()
      : leftWingTop     (-1, -1, -1, 100.0, 14.0),
        leftWingBottom  (-1, -1, -1, 150.0, 14.0),
        rightWingTop    (-1, -1, -1, 100.0, 14.0),
        rightWingBottom (-1, -1, -1, 150.0, 14.0),
        headPan         (6, 26, 34, 100.0, 14.0),
        headTilt        (13, 43, 51, 300.0, 14.0),
        headRoll        (12, 42, 50, 50.0, 14.0),
        leftEyelid      (-1, -1, -1, 0.0, 0.0),
        rightEyelid     (-1, -1, -1, 0.0, 0.0),
        leftEar         (-1, -1, -1, 0.0, 0.0),
        rightEar        (-1, -1, -1, 0.0, 0.0),
        mouth           (7, 27, 35, 50.0, 14.0),

        eyes            (2),
        breath          (8),

        smokeMachinePin (-1)
    {
        allMotors[0]  = &leftWingTop;
        allMotors[1]  = &leftWingBottom;
        allMotors[2]  = &rightWingTop;
        allMotors[3]  = &rightWingBottom;
        allMotors[4]  = &headPan;
        allMotors[5]  = &headTilt;
        allMotors[6]  = &headRoll;
        allMotors[7]  = &leftEyelid;
        allMotors[8]  = &rightEyelid;
        allMotors[9]  = &leftEar;
        allMotors[10] = &rightEar;
        allMotors[11] = &mouth;
    }

    void begin() {
        for (int i = 0; i < 12; i++) {
            allMotors[i]->begin();
        }
    }

    void update() {
        for (int i = 0; i < 12; i++) {
            allMotors[i]->update();
        }
    }

    void extendAll() {
        for (int i = 0; i < 12; i++) {
            allMotors[i]->extend();
        }
    }

    void retractAll() {
        for (int i = 0; i < 12; i++) {
            allMotors[i]->retract();
        }
    }

    void stop() {
        for (int i = 0; i < 12; i++) {
            allMotors[i]->brake();
        }
    }

    bool isAnyMotorMoving() {
        for (int i = 0; i < 12; i++) {
            if (allMotors[i]->isMoving) return true;
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
        retractAll();
        waitUntilFinished();
        headRoll.extendHalf();
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

    void extendAndRetractOnce(Motor& actuator) {
        actuator.retract();
        waitUntilFinished();
        actuator.extend();
        waitUntilFinished();
        actuator.retract();
        waitUntilFinished();
    }
};

Dragon graoully;

void setup()
{
    Serial.begin(115200);
    graoully.begin();

    while (1) {
        graoully.eyes.fade();
    }
}

void loop()
{
    graoully.update();
}
