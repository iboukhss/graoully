
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

    unsigned long safeTravelTimeMs = 0;
    unsigned long travelTimeMs = 0;
    unsigned long startTimeMs = 0;

    bool isMoving = false;

    Motor(int ena, int in1, int in2, float stroke, float speed)
      : pinEna(ena), pinIn1(in1), pinIn2(in2), strokeMm(stroke), speedMmS(speed)
    {
        if (speedMmS > 0) {
            safeTravelTimeMs = (unsigned long) ((strokeMm / speedMmS + 2.0) * 1000.0);
        }
    }

    void begin() {
        if (pinEna == -1) return;
        pinMode(pinEna, OUTPUT);
        pinMode(pinIn1, OUTPUT);
        pinMode(pinIn2, OUTPUT);
    }

    void extend() {
        if (pinEna == -1) return;
        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 255);

        travelTimeMs = safeTravelTimeMs;
        startTimeMs = millis();
        isMoving = true;
    }

    void retract() {
        if (pinEna == -1) return;
        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
        analogWrite(pinEna, 255);

        travelTime = safeTravelTimeMs;
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

    void update() {
        if (!isMoving) return;
        if (millis() - startTimeMs >= travelTimeMs) {
            brake();
        }
    }
};

struct Led {
    int pin;

    Led() : pin(-1) {}

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
};
spot led 12v rgb
struct Dragon {
    Motor leftWingTop;      // Opening of the wing
    Motor leftWingBottom;   // Up/down wing movement
    Motor rightWingTop;
    Motor rightWingBottom;
    Motor headPan;  // Left/right rotation
    Motor headTilt; // Up/down nod
    Motor headRoll; // Side to side twist (see camera_movements.jpg)
    Motor leftEyelid;
    Motor rightEyelid;
    Motor leftEar;
    Motor rightEar;
    Motor mouth;

    Led leftEye;
    Led rightEye;
    Led breath;

    int smokeMachinePin;

    Dragon()
      : leftWingTop     (-1, -1, -1, 100.0, 14.0),
        leftWingBottom  (-1, -1, -1, 150.0, 14.0),
        rightWingTop    (-1, -1, -1, 100.0, 14.0),
        rightWingBottom (-1, -1, -1, 150.0, 14.0),
        headPan         (-1, -1, -1, 100.0, 14.0),
        headTilt        (-1, -1, -1, 300.0, 14.0),
        headRoll        (-1, -1, -1, 50.0, 14.0),
        leftEyelid      (-1, -1, -1),
        rightEyelid     (-1, -1, -1),
        leftEar         (-1, -1, -1),
        rightEar        (-1, -1, -1),
        mouth           (2, 22, 30, 50.0, 14.0)
    {}

    void begin() {
        leftWingTop.begin();
        leftWingBottom.begin();
        rightWingTop.begin();
        rightWingBottom.begin();
        headPan.begin();
        headRoll.begin();
        headTilt.begin();
        leftEyelid.begin();
        rightEyelid.begin();
        leftEar.begin();
        rightEar.begin();
        mouth.begin();
    }

    void stop() {
        leftWingTop.brake();
        leftWingBottom.brake();
        rightWingTop.brake();
        rightWingBottom.brake();
        headPan.brake();
        headRoll.brake();
        headTilt.brake();
        leftEyelid.brake();
        rightEyelid.brake();
        leftEar.brake();
        rightEar.brake();
        mouth.brake();
    }

    void reset() {
        leftWingTop.retract();
        leftWingBottom.retract();
        rightWingTop.retract();
        rightWingBottom.retract();
        headPan.retract();
        headRoll.retract();
        headTilt.retract();
        leftEyelid.retract();
        rightEyelid.retract();
        leftEar.retract();
        rightEar.retract();
        mouth.retract();
        delay(5000);
        stop();
    }

    void roar() {
        mouth.extend();
        reset();
    }
};

Dragon graoully;

void setup()
{
    graoully.begin();
}

void loop()
{
    graoully.mouth.extend();
    graoully.mouth.retract();
}
