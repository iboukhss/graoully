#include "dragon.h"
#include "log.h"
#include "show.h"
#include "show_player.h"
#include "show_scheduler.h"

Dragon graoully;
ShowPlayer player;
ShowScheduler scheduler;

ControlMode currentMode = ControlMode::AUTO;

bool allowNightEffects = true;
bool isCapturingSequence = false;

// List of available serial commands:

// time:print
// time:set?yyyy,mm,dd,hh,mm,ss

// drag:home
// drag:reset
// drag:stop

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

// mp3:play?u8_value

void splitActionArg(const String& rawAction, String& outAction, String& outArg)
{
    int qIndex = rawAction.indexOf('?');

    if (qIndex != -1) {
        outAction = rawAction.substring(0, qIndex);
        outArg = rawAction.substring(qIndex + 1);
    }
    else {
        outAction = rawAction;
        outArg = "";
    }
}

void handleAudioCommand(const String& action)
{
    String parsedAction = "";
    String parsedArg = "";

    splitActionArg(action, parsedAction, parsedArg);

    if (parsedAction == "play") {
        if (parsedArg.length() > 0) {
            int trackNum = parsedArg.toInt();
            graoully.mp3.playFileNumber(trackNum);
        }
        else {
            graoully.mp3.play();
        }
    }
    else if (parsedAction == "vol") {
        if (parsedArg.length() > 0) {
            int vol = parsedArg.toInt();
            graoully.mp3.setVolume(vol);
        }
    }
    else if (parsedAction == "status") {
        graoully.mp3.printToSerial();
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("mp3: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleShowCommand(const String& action)
{
    String parsedAction = "";
    String parsedArg = "";

    splitActionArg(action, parsedAction, parsedArg);

    if (parsedAction == "play") {
        if (parsedArg.length() > 0) {
            player.play(parsedArg);
        }
        else {
            player.playSerialSlot();
        }
    }
    else if (parsedAction == "stop") {
        player.stop();
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("show: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleSeqCommand(const String& action)
{
    String parsedAction = "";
    String parsedArg = "";

    splitActionArg(action, parsedAction, parsedArg);

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
                LOG_ERR_BEGIN();
                Serial.print(F("seq: Preset not found: "));
                Serial.print(parsedArg);
                LOG_ERR_END();
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
        LOG_ERR_BEGIN();
        Serial.print(F("seq: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleCaptureLine(const String& input)
{
    if (input == ".") {
        isCapturingSequence = false;
        LOG_INFO(F("seq: CSV capture done"));
        return;
    }

    int firstComma = input.indexOf(',');
    int secondComma = input.indexOf(',', firstComma + 1);

    if (firstComma == -1 || secondComma == -1) {
        LOG_ERR_BEGIN();
        Serial.print(F("seq: Invalid CSV line: "));
        Serial.print(input);
        LOG_ERR_END();
        return;
    }

    unsigned long offset = input.substring(0, firstComma).toInt();
    String target = input.substring(firstComma + 1, secondComma);
    String action = input.substring(secondComma + 1);

    player.getSerialSlot().append(offset, target, action);

    LOG_DBG_BEGIN();
    Serial.print(F("seq: Appended offset="));
    Serial.print(offset);
    Serial.print(F(", target="));
    Serial.print(target);
    Serial.print(F(", action="));
    Serial.print(action);
    LOG_DBG_END();
}

void handleLedCommand(const String& target, const String& action)
{
    if (!allowNightEffects) {
        LOG_WARN(F("led: Blocked (night effects disabled)"));
        return;
    }

    Led* led = graoully.getLed(target);

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

void handleMotorCommand(const String& target, const String& action)
{
    Motor* motor = graoully.getMotor(target);

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
    String parsedAction = "";
    String parsedArg = "";

    splitActionArg(action, parsedAction, parsedArg);

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
            LOG_ERR(F("time: Invalid format - expected: YYYY,MM,DD,HH,MM,SS"));
        }
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("time: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleGraoullyCommand(const String& action)
{
    if (action == "home") {
        graoully.home();
    }
    else if (action == "stop") {
        graoully.estop();
    }
    else if (action == "reset") {
        graoully.reset();
    }
    else if (action == "status") {
        graoully.printStatus();
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("drag: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleModeCommand(const String& action)
{
    if (action == "auto") {
        currentMode = ControlMode::AUTO;
        LOG_INFO(F("mode: Switched to 'auto' mode"));
    }
    else if (action == "manual") {
        currentMode = ControlMode::MANUAL;
        LOG_INFO(F("mode: Switched to 'manual' mode"));
    }
    else if (action == "status") {
        LOG_INFO_BEGIN();
        Serial.print(F("mode: Current control mode: "));
        Serial.print(currentMode == ControlMode::AUTO ? F("auto") : F("manual"));
        LOG_INFO_END();
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("mode: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void handleNightCommand(const String& action)
{
    if (action == "on") {
        allowNightEffects = true;
        LOG_INFO(F("night: Effects enabled"));
    }
    else if (action == "off") {
        allowNightEffects = false;
        LOG_INFO(F("night: Effects disabled"));
    }
    else if (action == "status") {
        LOG_INFO_BEGIN();
        Serial.print(F("night: Effects status: "));
        Serial.print(allowNightEffects ? F("enabled") : F("disabled"));
        LOG_INFO_END();
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("night: Unknown action: "));
        Serial.print(action);
        LOG_ERR_END();
    }
}

void executeCommand(const String& target, const String& action)
{
    LOG_DBG_BEGIN();
    Serial.print(F("cmd: "));
    Serial.print(target);
    Serial.print(" ");
    Serial.print(action);
    LOG_DBG_END();

    if (target == "drag") {
        handleGraoullyCommand(action);
    }
    else if (graoully.getMotor(target) != nullptr) {
        handleMotorCommand(target, action);
    }
    else if (graoully.getLed(target) != nullptr) {
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
    else if (target == "night") {
        handleNightCommand(action);
    }
    else {
        LOG_ERR_BEGIN();
        Serial.print(F("cmd: Unknown target: "));
        Serial.print(target);
        LOG_ERR_END();
    }
}

void handleSerialCommands()
{
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (!input.length()) return;

        if (isCapturingSequence) {
            handleCaptureLine(input);
            return;
        }

        int separatorIndex = input.indexOf(':');

        if (separatorIndex != -1) {
            String target = input.substring(0, separatorIndex);
            String action = input.substring(separatorIndex + 1);

            // These commands are always allowed regardless of the current control mode
            if (target == "mode") {
                handleModeCommand(action);
                return;
            }

            if (target == "time") {
                handleTimeCommand(action);
                return;
            }
            
            if (currentMode == ControlMode::AUTO) {
                LOG_ERR(F("cmd: Command blocked (switch to manual mode)"));
                return;
            }
            
            executeCommand(target, action);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    // Give some time for serial monitor to connect
    delay(3000);

    randomSeed(analogRead(A15));

    scheduler.begin();
    graoully.begin();
    graoully.home();
    graoully.reset();

    LOG_INFO(F("setup: Initialization finished - Graoully is ready"));
}

void loop()
{
    handleSerialCommands();
    player.update();
    scheduler.update();
    graoully.update();
}
