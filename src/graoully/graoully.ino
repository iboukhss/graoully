#include <assert.h>

#include "dragon.h"
#include "show.h"

Dragon graoully;

const SequenceItem PROGMEM kRoarData[] = {
    { 0,    "mouth", "1.0" },
    { 500,  "tilt",  "0.8" },
    { 1500, "eyes",  "on"  }
};

const int kRoarCount = sizeof(kRoarData) / sizeof(kRoarData[0]);

const ShowPreset PROGMEM kShowLibrary[] = {
    { "ROAR", kRoarData, kRoarCount }
};

ShowSequence presetSlot("SLOT0");
ShowSequence serialSlot("SLOT1");

SequencePlayer player;

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

void executeCommand(const String& target, const String& action)
{
    Serial.print(F("Received -> "));
    Serial.print(target);
    Serial.print(F(" : "));
    Serial.println(action);

    if (target == "graoully") {
        handleGraoullyCommand(action);
    }
    else if (isMotorCommand(target)) {
        handleMotorCommand(target, action);
    }
    else if (isLedCommand(target)) {
        handleLedCommand(target, action);
    }
    else {
        Serial.print(F("ERROR: Unknown target -> "));
        Serial.println(target);
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

            executeCommand(target, action);
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
