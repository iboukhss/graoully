#include <assert.h>

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

    float currentPositionMm = 0.0f;
    float targetPositionMm = 0.0f;

    unsigned long travelTimeMs = 0;
    unsigned long startTimeMs = 0;

    bool isMoving = false;

    Motor(int ena, int in1, int in2, float stroke, float speed)
      : pinEna(ena), pinIn1(in1), pinIn2(in2), strokeMm(stroke), speedMmS(speed)
    {
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

        isMoving = true;
    }

    void retract() {
        if (pinEna == -1) return;

        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, HIGH);
        analogWrite(pinEna, 255);

        isMoving = true;
    }

    void brake() {
        if (pinEna == -1) return;

        digitalWrite(pinIn1, LOW);
        digitalWrite(pinIn2, LOW);
        analogWrite(pinEna, 0);

        isMoving = false;
    }

    void home() {
        if (pinEna == -1) return;

        // Safety margin to make sure the actuators are fully retracted (30% atm).
        float extraTimeMultiplier = 1.3f;
     
        targetPositionMm = 0.0f;
        travelTimeMs = (strokeMm / speedMmS) * 1000.0f * extraTimeMultiplier;

        retract();

        startTimeMs = millis();
    }

    // Must be a value between 0 and 1.0 or undefined behavior
    void moveTo(float position) {
        if (pinEna == -1 || isMoving) return;

        targetPositionMm = strokeMm * position;

        if (targetPositionMm == currentPositionMm) return;

        float distanceMm = abs(targetPositionMm - currentPositionMm);
        travelTimeMs = (distanceMm / speedMmS) * 1000.0f;

        if (targetPositionMm > currentPositionMm) {
            extend();
        }
        else {
            retract();
        }

        startTimeMs = millis();
    }

    void update() {
        if (!isMoving) return;

        if (millis() - startTimeMs >= travelTimeMs) {
            currentPositionMm = targetPositionMm;
            brake();
        }
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

    int smokeMachinePin;

    Motor* allMotors[8];

    Dragon()
      : wingsTop        (-1, -1, -1, 100.0, 14.0),
        wingsBottom     (-1, -1, -1, 150.0, 14.0),
        headPan         (6, 26, 34, 150.0, 14.0),   // Counting 18.3 seconds for extension
        headTilt        (13, 43, 51, 300.0, 14.0),
        headRoll        (12, 42, 50, 50.0, 14.0),
        eyelids         (-1, -1, -1, 0.0, 0.0),
        ears            (-1, -1, -1, 0.0, 0.0),
        mouth           (7, 27, 35, 50.0, 14.0),

        eyes            (2),
        breath          (8),

        smokeMachinePin (-1)
    {
        allMotors[0] = &wingsTop;
        allMotors[1] = &wingsBottom;
        allMotors[2] = &headPan;
        allMotors[3] = &headTilt;
        allMotors[4] = &headRoll;
        allMotors[5] = &eyelids;
        allMotors[6] = &ears;
        allMotors[7] = &mouth;
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
            allMotors[i]->brake();
        }
    }

    bool isAnyMotorMoving() {
        for (int i = 0; i < sizeof(allMotors) / sizeof(allMotors[0]); i++) {
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

Dragon graoully;

struct MotorMapEntry {
    const char* name;
    Motor* motor;
};

MotorMapEntry motorMap[] = {
    {"mouth",     &graoully.mouth},
    {"tilt",      &graoully.headTilt},
    {"pan",       &graoully.headPan},
    {"roll",      &graoully.headRoll},
    {"ears",      &graoully.ears},
    {"eyelids",   &graoully.eyelids},
    {"wings_top", &graoully.wingsTop},
    {"wings_bot", &graoully.wingsBottom}
};

bool isMotorCommand(const String& target)
{
    for (int i = 0; i < sizeof(motorMap) / sizeof(motorMap[0]); i++) {
        if (target == motorMap[i].name) {
            return true;
        }
    }
    return false;
}

Motor* getMotor(const String& target)
{
    for (int i = 0; i < sizeof(motorMap) / sizeof(motorMap[0]); i++) {
        if (target == motorMap[i].name) {
            return motorMap[i].motor;
        }
    }
    return nullptr;
}

void handleMotorCommand(const String& target, const String& action)
{
    Motor* motor = getMotor(target);

    if (action == "home") {
        motor->home();
    }
    else {
        float val = action.toFloat();
        motor->moveTo(val);
    }
}

void handleGraoullyCommand(const String& action)
{
    if (action == "home") {
        graoully.reset();
    }
    else if (action == "stop") {
        graoully.stop();
    }
    else if (action == "roar") {
        graoully.roar();
    }
    else {
        Serial.print("ERROR: Unknown Graoully action -> ");
        Serial.println(action);
    }
}

void handleSerialCommands()
{
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');

        input.trim();

        if (input.length() == 0) return;

        int separatorIndex = input.indexOf(':');

        if (separatorIndex != -1) {
            String target = input.substring(0, separatorIndex);
            String action = input.substring(separatorIndex + 1);

            Serial.print("Received -> ");
            Serial.print(target);
            Serial.print(" : ");
            Serial.println(action);

            if (target == "graoully") {
                handleGraoullyCommand(action);
            }
            else if (isMotorCommand(target)) {
                handleMotorCommand(target, action);
            }
            else {
                Serial.print("ERROR: Unknown target -> ");
                Serial.println(target);
            }
        }
    }
}

void setup()
{
    Serial.begin(115200);
    graoully.begin();
}

void loop()
{
    graoully.update();
    handleSerialCommands();
}
