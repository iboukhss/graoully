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
        // Let's assume we start dead center (this doesn't achieve anything)
        startPositionMm_ = stroke * 0.5f;
    }

    void begin() {
        if (pinEna_ == -1) return;

        pinMode(pinEna_, OUTPUT);
        pinMode(pinIn1_, OUTPUT);
        pinMode(pinIn2_, OUTPUT);
    }

    void stop() {
        if (pinEna_ == -1 || !isMoving_) return;

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 0);

        isMoving_ = false;
    }

    void update() {
        if (pinEna_ == -1 || !isMoving_) return;

        unsigned long elapsedMs = millis() - startTimeMs_;

        if (elapsedMs >= travelTimeMs_) {
            stop();
            currentPositionMm_ = constrain(targetPositionMm_, 0.0f, strokeMm_);
            return;
        }

        float distanceTravelledMm = (elapsedMs / 1000.0f) * speedMmS_;

        if (targetPositionMm_ > startPositionMm_) {
            currentPositionMm_ = startPositionMm_ + distanceTravelledMm;
        }
        else {
            currentPositionMm_ = startPositionMm_ - distanceTravelledMm;
        }
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

        float distanceMm = abs(targetPositionMm_ - currentPositionMm_);

        // 0.1mm tolerance to avoid wearing dowm the motor for nothing
        if (distanceMm < 0.1f) return;

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

    const char* getName() const { return name_; }
    float getPosition() const { return currentPositionMm_; }
    bool isMoving() const { return isMoving_; }

private:
    void extend() {
        if (pinEna_ == -1 || isMoving_) return;

        digitalWrite(pinIn1_, HIGH);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        startPositionMm_ = currentPositionMm_;
        isMoving_ = true;
    }

    void retract() {
        if (pinEna_ == -1 || isMoving_) return;

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, HIGH);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        startPositionMm_ = currentPositionMm_;
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
    float startPositionMm_ = 0.0f;

    unsigned long travelTimeMs_ = 0;
    unsigned long startTimeMs_ = 0;

    bool isMoving_ = false;
};

class Led {
public:
    Led(const char* name, int p) : name_(name), pin_(p) {}

    void begin() {
        if (pin_ == -1) return;

        pinMode(pin_, OUTPUT);
    }

    void setBrightness(int level) {
        if (pin_ == -1) return;

        analogWrite(pin_, level);
    }

    void on() {
        setBrightness(255);
    }

    void off() {
        setBrightness(0);
    }

    const char* getName() const { return name_; }

private:
    const char* name_;
    int pin_;
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
    Led*   allLeds[2];

    Dragon()
      : wingsTop        ("wings_top", -1, -1, -1, 100.0, 14.0),
        wingsBottom     ("wings_bot", -1, -1, -1, 150.0, 14.0),
        headPan         ("pan",        8, 38, 46, 100.0, 5.5),  // Needs adjustment!
        headTilt        ("tilt",       2, 22, 30, 300.0, 14.0),
        headRoll        ("roll",       9, 39, 47, 50.0, 14.0),
        eyelids         ("eyelids",    4, 24, 32, 50.0, 14.0),  // This is just a guess
        ears            ("ears",      10, 40, 48, 50.0, 14.0),  // Also a guess
        mouth           ("mouth",      3, 23, 31, 50.0, 14.0),

        eyes            ("eyes",   A8),
        breath          ("breath", 8),

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

        allLeds[0] = &eyes;
        allLeds[1] = &breath;
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

Dragon graoully;

// List of available serial commands:

// graoully:home
// graoully:stop
// graoully:roar

// motor:home
// motor:stop
// motor:float_value

// led:on
// led:off
// led:u8_value

bool isLedCommand(const String& target)
{
    for (int i = 0; i < sizeof(graoully.allLeds) / sizeof(graoully.allLeds[0]); i++) {
        if (target == graoully.allLeds[i]->getName()) {
            return true;
        }
    }
    return false;
}

Led* getLed(const String& target)
{
    for (int i = 0; i < sizeof(graoully.allLeds) / sizeof(graoully.allLeds[0]); i++) {
        if (target == graoully.allLeds[i]->getName()) {
            return graoully.allLeds[i];
        }
    }
    return nullptr;
}

void handleLedCommand(const String& target, const String& action)
{
    Led* led = getLed(target);

    if (action == "on") {
        led->on();
    }
    else if (action == "off") {
        led->off();
    }

    // TODO: Add brightness control action

    else {
        Serial.print(F("ERROR: Unknown LED action -> "));
        Serial.println(action);
    }
}

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
        motor->stop();
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
        Serial.print(F("ERROR: Unknown Graoully action -> "));
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
