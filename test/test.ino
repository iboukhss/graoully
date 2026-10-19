
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

    Motor()
      : pinEna(-1), pinIn1(-1), pinIn2(-1), strokeMm(0), speedMmS(0) {}

    Motor(int ena, int in1, int in2)
      : pinEna(ena), pinIn1(in1), pinIn2(in2) {}

    void begin() {
        if (pinEna == -1) return;
        pinMode(pinEna, OUTPUT);
        pinMode(pinIn1, OUTPUT);
        pinMode(pinIn2, OUTPUT);
    }

    unsigned long getSafeTravelTimeMs() {
        float theoreticalSeconds = strokeMm / speedMmS;
        return (unsigned long) ((theoreticalSeconds + 2.0) * 1000.0);
    }

    void extend() {
        if (pinEna == -1) return;
        digitalWrite(pinIn1, HIGH);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 255);
        delay(getSafeTravelTimeMs());
        brake();
    }

    void retract() {
        if (pinEna == -1) return;
        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
        analogWrite(pinEna, 255);
        delay(getSafetravelTimeMs());
        brake();
    }

    void brake() {
        if (pinEna == -1) return;
        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 0);
    }
};

struct Dragon {
    Motor leftWingTop;
    Motor leftWingBottom;
    Motor rightWingTop;
    Motor rightWingBottom;
    Motor headPan;
    Motor headLift;
    Motor headTilt;
    Motor leftEyelid;
    Motor rightEyelid;
    Motor leftEar;
    Motor rightEar;
    Motor mouth;

    Dragon()
      : leftWingTop     (-1, -1, -1),
        leftWingBottom  (-1, -1, -1),
        rightWingTop    (-1, -1, -1),
        rightWingBottom (-1, -1, -1),
        headPan         (-1, -1, -1),
        headLift        (-1, -1, -1),
        headTilt        (-1, -1, -1),
        leftEyelid      (-1, -1, -1),
        rightEyelid     (-1, -1, -1),
        leftEar         (-1, -1, -1),
        rightEar        (-1, -1, -1),
        mouth           (2, 22, 30, 50, 14)
    {}

    void begin() {
        leftWingTop.begin();
        leftWingBottom.begin();
        rightWingTop.begin();
        rightWingBottom.begin();
        headPan.begin();
        headTilt.begin();
        headLift.begin();
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
        headTilt.brake();
        headLift.brake();
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
        headTilt.retract();
        headLift.retract();
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
