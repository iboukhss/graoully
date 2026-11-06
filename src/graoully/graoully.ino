#include <assert.h>

// Example motor 1

// Stroke:     50mm
// Max load:   1000N (100kg)
// Speed:      14mm/s
// Duty cycle: 10%

// Approx 3.57 seconds to fully extend/retract

class Motor {
public:
    Motor(const char* name, int ena, int in1, int in2, float stroke, float speed)
      : name_(name), pinEna_(ena), pinIn1_(in1), pinIn2_(in2), strokeMm_(stroke), speedMmS_(speed)
    {
    }

    void begin() {
        if (pinEna_ == -1) return;

        pinMode(pinEna_, OUTPUT);
        pinMode(pinIn1_, OUTPUT);
        pinMode(pinIn2_, OUTPUT);
    }

    const char* getName() const { return name_; }

    bool isMoving() const { return isMoving_; }

    float getPosition() const {
        if (!isMoving_) return currentPositionMm_;

        unsigned long elapsedMs = millis() - startTimeMs_;
        float distanceTravelledMm = (elapsedMs / 1000.0f) * speedMmS_;

        if (targetPositionMm_ > currentPositionMm_) {
            return constrain(currentPositionMm_ + distanceTravelledMm, 0.0f, strokeMm_);
        }
        else {
            return constrain(currentPositionMm_ - distanceTravelledMm, 0.0f, strokeMm_);
        }
    }

    void brake() {
        if (pinEna_ == -1 || !isMoving_) return;

        currentPositionMm_ = getPosition();

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 0);

        isMoving_ = false;
    }

    void home() {
        if (pinEna_ == -1) return;

        if (isMoving_) {
            Serial.println(F("WARNING: Motor is busy. 'home' command ignored."));
            return;
        }

        targetPositionMm_ = 0.0f;
        float fullStrokeTimeMs = (strokeMm_ / speedMmS_) * 1000.0f;
        travelTimeMs_ = fullStrokeTimeMs * 1.4f;

        retract();
    }

    void moveTo(float position) {
        if (pinEna_ == -1) return;

        if (isMoving_) {
            Serial.println(F("WARNING: Motor is busy. 'moveTo' command ignored."));
            return;
        }

        targetPositionMm_ = strokeMm_ * position;

        if (targetPositionMm_ == currentPositionMm_) return;

        float distanceMm = abs(targetPositionMm_ - currentPositionMm_);
        travelTimeMs_ = (distanceMm / speedMmS_) * 1000.0f;

        // If we move to an absolute boundary (0.0 or 1.0) then we add a safety
        // margin proportional to the full stroke length, here 20%
        if (position <= 0.0f || position >= 1.0f) {
            float fullStrokeTimeMs = (strokeMm_ / speedMmS_) * 1000.0f;
            travelTimeMs_ += (fullStrokeTimeMs * 0.2f);
        }

        if (targetPositionMm_ > currentPositionMm_) {
            extend();
        }
        else {
            retract();
        }
    }

    void update() {
        if (pinEna_ == -1 || !isMoving_) return;

        unsigned long elapsedMs = millis() - startTimeMs_;

        if (elapsedMs >= travelTimeMs_) {
            brake();
        }
    }

private:
    void extend() {
        if (pinEna_ == -1 || isMoving_) return;

        digitalWrite(pinIn1_, HIGH);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        isMoving_ = true;
    }

    void retract() {
        if (pinEna_ == -1 || isMoving_) return;

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, HIGH);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        isMoving_ = true;
    }

    const char* name_;

    int pinEna_;
    int pinIn1_;
    int pinIn2_;

    float strokeMm_;
    float speedMmS_;

    float currentPositionMm_ = 0.0f;
    float targetPositionMm_ = 0.0f;

    unsigned long travelTimeMs_ = 0;
    unsigned long startTimeMs_ = 0;

    bool isMoving_ = false;
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
      : wingsTop        ("wings_top", -1, -1, -1, 100.0, 14.0),
        wingsBottom     ("wings_bot", -1, -1, -1, 150.0, 14.0),
        headPan         ("pan",        8, 38, 46, 100.0, 5.5),  // Needs adjustment!
        headTilt        ("tilt",       2, 22, 30, 300.0, 14.0),
        headRoll        ("roll",       9, 39, 47, 50.0, 14.0),
        eyelids         ("eyelids",    4, 24, 32, 50.0, 14.0),  // This is just a guess
        ears            ("ears",      10, 40, 48, 50.0, 14.0),  // Also a guess
        mouth           ("mouth",      3, 23, 31, 50.0, 14.0),

        eyes            (A8),
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

Dragon graoully;

// List of available serial commands:

// graoully:home
// graoully:stop
// graoully:roar

// motor:home
// motor:stop
// motor:float_value

bool isMotorCommand(const String& target)
{
    for (int i = 0; i < sizeof(graoully.allMotors) / sizeof(graoully.allMotors[0]); i++) {
        if (target == graoully.allMotors[i]->getName()) {
            return true;
        }
    }
    return false;
}

Motor* getMotor(const String& target)
{
    for (int i = 0; i < sizeof(graoully.allMotors) / sizeof(graoully.allMotors[0]); i++) {
        if (target == graoully.allMotors[i]->getName()) {
            return graoully.allMotors[i];
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
    else if (action == "stop") {
        motor->brake();
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
    graoully.eyes.on();
}

void loop()
{
    graoully.update();
    handleSerialCommands();
}
