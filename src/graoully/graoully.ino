#include "dragon.h"
#include "show.h"
#include "show_player.h"
#include "show_scheduler.h"

Dragon graoully;
ShowPlayer player;
ShowScheduler scheduler;

ControlMode currentMode = ControlMode::AUTO;

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
        if (parsedArg.length()) {
            int trackNum = parsedArg.toInt();
            graoully.mp3.playFileNumber(trackNum);
            Serial.print(F("Playing track number: "));
            Serial.println(trackNum);
        }
        else {
            graoully.mp3.play();
        }
    }
    else if (parsedAction == "vol") {
        if (parsedArg.length() > 0) {
            int vol = parsedArg.toInt();
            graoully.mp3.setVolume(vol);
            Serial.print(F("Volume set to: "));
            Serial.println(vol);
        }
    }
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
        if (parsedArg.length()) {
            player.play(parsedArg);
        }
        else {
            player.playSerialSlot();
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
        player.getSerialSlot().clear();
        isCapturingSequence = true;
        Serial.println(F("--- Entering CSV capture mode ---"));
        Serial.println(F("Send lines as: offset,target,action"));
        Serial.println(F("Send a single '.' to finish the capture."));
    }
    else if (parsedAction == "clear") {
        player.getSerialSlot().clear();
    }
    else if (parsedAction == "print") {
        if (parsedArg.length()) {
            const ShowPreset* ptr = getPresetByName(parsedArg);

            if (!ptr) {
                Serial.print(F("ERROR: Show preset not found -> "));
                Serial.println(parsedArg);
                return;
            }

            Show tempSlot("TMP");
            tempSlot.loadFromFlash(ptr);
            tempSlot.printToSerial();
        }
        else {
            player.getSerialSlot().printToSerial();
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

void handleTimeCommand(const String& action)
{
    String parsedAction = action;
    String parsedArg = "";

    int qIndex = action.indexOf('?');

    if (qIndex != -1) {
        parsedAction = action.substring(0, qIndex);
        parsedArg = action.substring(qIndex + 1);
    }

    if (parsedAction == "print") {
        scheduler.getClock().printToSerial();
    }
    else if (parsedAction == "set") {
        int y, m, d, hh, mm, ss;
        int parsed = sscanf(parsedArg.c_str(), "%d,%d,%d,%d,%d,%d", &y, &m, &d, &hh, &mm, &ss);

        if (parsed == 6) {
            scheduler.getClock().setDateTime(y, m, d, hh, mm, ss);
        }
        else {
            Serial.println(F("ERROR: Invalid format. Use: time:set?YYYY,MM,DD,HH,MM,SS"));
        }
    }
    else {
        Serial.print(F("ERROR: Unknown time action -> "));
        Serial.println(action);
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

void handleModeCommand(const String& action)
{
    if (action == "auto") {
        currentMode = ControlMode::AUTO;
        Serial.println(F("OK: Switched to automatic mode."));
    }
    else if (action == "manual") {
        currentMode = ControlMode::MANUAL;
        Serial.println(F("OK: Switched to manual mode."));
    }
    else if (action == "status") {
        Serial.print(F("Current mode: "));
        Serial.println(currentMode == ControlMode::AUTO ? F("AUTO") : F("MANUAL"));
    }
    else {
        Serial.print(F("ERROR: Unknown mode action: "));
        Serial.println(action);
    }
}

void executeCommand(const String& target, const String& action)
{
    Serial.print(F("Received -> "));
    Serial.print(target);
    Serial.print(F(" : "));
    Serial.println(action);

    // Mode commands are always allowed
    if (target == "mode") {
        handleModeCommand(action);
        return;
    }

    if (currentMode == ControlMode::AUTO) {
        Serial.println(F("ERROR: Command blocked. System is currently in AUTO mode. Switch to MANUAL mode to take control."));
        return;
    }

    if (target == "time") {
        handleTimeCommand(action);
    }
    else if (target == "graoully") {
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

        if (!input.length()) return;

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

                player.getSerialSlot().append(offset, target, action);

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

    // Wait for serial monitor to connect
    delay(2000);

    randomSeed(analogRead(A15));
    graoully.begin();
    graoully.eyes.on();
    scheduler.begin();
}

void loop()
{
    handleSerialCommands();
    player.update();
    scheduler.update();
    graoully.update();
}
