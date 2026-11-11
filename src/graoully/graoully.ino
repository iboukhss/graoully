#include "dragon.h"
#include "show.h"

Dragon graoully;

const SequenceItem PROGMEM kSleepSeqData[] = {
    { 0,    "breath",   "off" },
    { 0,    "pan",      "0.5" },
    { 0,    "roll",     "0.5" },
    { 2000, "tilt",     "0.1" },
    { 8000, "pan",      "0.1"},
    { 12000, "mouth",   "0.0" },
    { 12000, "eyelids", "0.5" },
    { 14000, "eyes",    "off" },
    { 14000, "eyelids", "0.0" },
    { 16000, "ears",    "0.0" },
    { 20000, "mouth",   "0.1"}
};

constexpr int kSleepSeqCount = sizeof(kSleepSeqData) / sizeof(kSleepSeqData[0]);

const SequenceItem PROGMEM kAlertSeqData[] = {
    { 0,     "ears",   "0.5" },
    { 3000,  "eyes",   "on" },
    { 3000,  "breath", "on" },
    { 4000,  "pan",    "0.4"},
    { 6000,  "mouth",  "0.2" },
    { 7000,  "smoke",  "3000" },
    { 11000, "eyes",   "off" },
    { 11000, "mouth",  "0.0" },
    { 13000, "pan",    "0.1" },
    { 17000, "ears",   "0.0" }
};

constexpr int kAlertSeqCount = sizeof(kAlertSeqData) / sizeof(kAlertSeqData[0]);

const SequenceItem PROGMEM kWakeSeqData[] = {
    { 0,     "breath",   "on" },
    { 0,     "mouth",    "0.1" },
    { 1500,  "smoke",    "3000" }
};

constexpr int kWakeSeqCount = sizeof(kWakeSeqData) / sizeof(kWakeSeqData[0]);

const ShowPreset PROGMEM kPresetLibrary[] = {
    { "SLEEP", kSleepSeqData, kSleepSeqCount},
    { "ALERT", kAlertSeqData, kAlertSeqCount },
    { "WAKE",  kWakeSeqData,  kWakeSeqCount }
};

constexpr int kNumPresets = sizeof(kPresetLibrary) / sizeof(kPresetLibrary[0]);

ShowSequence presetSlot("SLOT0");
ShowSequence serialSlot("SLOT1");
SequencePlayer player;

bool isCapturingSequence = false;

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

// show:play
// show:play?NAME
// show:stop

// seq:begin
// seq:print
// seq:print?NAME

void handleAudioCommand(const String& action)
{
    String parsedAction = action;
    String parsedArg = "";

    int qIndex = action.indexOf('?');

    if (qIndex != -1) {
        parsedAction = action.substring(0, qIndex);
        parsedArg = action.substring(qIndex + 1);
    }

    if (parsedAction == "play") {
        if (parsedArg.length() > 0) {
            int trackNum = parsedArg.toInt();
            graoully.mp3.playFileNum(trackNum);
            Serial.print(F("Playing track number: "));
            Serial.println(trackNum);
        }
        else {
            graoully.mp3.start();
        }
    }
    else if (parsedAction == "vol") {
        if (parsedArg.length() > 0) {
            int vol = parsedArg.toInt();
            graoully.mp3.setVol(vol);
            Serial.print(F("Volume set to: "));
            Serial.println(vol);
        }
    }
}

const ShowPreset* getPreset(const String& name)
{
    for (int i = 0; i < kNumPresets; i++) {
        ShowPreset localPreset;

        memcpy_P(&localPreset, &kPresetLibrary[i], sizeof(kPresetLibrary[i]));

        if (String(localPreset.name).equalsIgnoreCase(name)) {
            return &kPresetLibrary[i];
        }
    }
    return nullptr;
}

void handleShowCommand(const String& action)
{
    String parsedAction = action;
    String parsedArg = "";

    int qIndex = action.indexOf('?');

    if (qIndex != -1) {
        parsedAction = action.substring(0, qIndex);
        parsedArg = action.substring(qIndex + 1);
    }

    if (parsedAction == "play") {
        if (parsedArg.length() > 0) {
            const ShowPreset* ptr = getPreset(parsedArg);

            if (!ptr) {
                Serial.print(F("ERROR: Show preset not found -> "));
                Serial.println(parsedArg);
                return;
            }

            presetSlot.loadFromFlash(ptr);
            player.load(presetSlot);
            player.play();
        }
        else {
            // Just play whatever is on the serial slot already
            player.load(serialSlot);
            player.play();
        }
    }
    else if (parsedAction == "stop") {
        player.stop();
    }
}

void handleSeqCommand(const String& action)
{
    String parsedAction = action;
    String parsedArg = "";

    int qIndex = action.indexOf('?');

    if (qIndex != -1) {
        parsedAction = action.substring(0, qIndex);
        parsedArg = action.substring(qIndex + 1);
    }

    if (parsedAction == "begin") {
        serialSlot.clear();
        isCapturingSequence = true;
        Serial.println(F("--- Entering CSV capture mode ---"));
        Serial.println(F("Send lines as: offset,target,action"));
        Serial.println(F("Send a single '.' to finish the capture."));
    }
    else if (parsedAction == "clear") {
        serialSlot.clear();
    }
    else if (parsedAction == "print") {
        if (parsedArg.length() > 0) {
            const ShowPreset* ptr = getPreset(parsedArg);

            if (!ptr) {
                Serial.print(F("ERROR: Show preset not found -> "));
                Serial.println(parsedArg);
                return;
            }

            ShowSequence tempSlot("TMP");
            tempSlot.loadFromFlash(ptr);
            tempSlot.printToSerial();
        }
        else {
            serialSlot.printToSerial();
        }
    }
    else {
        Serial.print(F("ERROR: Unknown seq action -> "));
        Serial.println(action);
    }
}

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
    else {
        long duration = action.toInt();
        
        led->onFor(duration);
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
    else if (action == "status") {
        motor->printToSerial();
    }
    else {
        float val = action.toFloat();
        motor->moveTo(val);
    }
}

void handleGraoullyCommand(const String& action)
{
    if (action == "home") {
        graoully.home();
    }
    else if (action == "stop") {
        graoully.stop();
    }
    else if (action == "reset") {
        graoully.reset();
    }
    else if (action == "status") {
        Serial.println("--- Graoully status ---");

        for (int i = 0; i < Dragon::kNumMotors; i++) {
            Motor* m = graoully.allMotors[i];

            m->printToSerial();
        }

        Serial.println(F("-------------------------------------"));
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
    else if (target == "show") {
        handleShowCommand(action);
    }
    else if (target == "seq") {
        handleSeqCommand(action);
    }
    else if (target == "mp3") {
        handleAudioCommand(action);
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

        if (!input) return;

        if (isCapturingSequence) {
            if (input == ".") {
                isCapturingSequence = false;
                Serial.println(F("Capture complete."));
                return;
            }

            int firstComma = input.indexOf(',');
            int secondComma = input.indexOf(',', firstComma + 1);

            if (firstComma == -1 || secondComma == -1) {
                Serial.print(F("ERROR: Invalid CSV format -> "));
                Serial.println(input);
            }
            else {
                unsigned long offset = input.substring(0, firstComma).toInt();
                String target = input.substring(firstComma + 1, secondComma);
                String action = input.substring(secondComma + 1);

                serialSlot.append(offset, target, action);

                Serial.print(F("OK Offset: "));
                Serial.print(offset);
                Serial.print(F(", Target: "));
                Serial.print(target);
                Serial.print(F(", Action: "));
                Serial.println(action);
            }
            return;
        }

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
    player.update();
    handleSerialCommands();
}
