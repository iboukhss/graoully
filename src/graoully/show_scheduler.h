#ifndef SHOW_SCHEDULER_H
#define SHOW_SCHEDULER_H

#include "rtc_module.h"

enum class ControlMode {
    AUTO,
    MANUAL
};

extern ControlMode currentMode;
extern ShowPlayer player;

class ShowScheduler {
public:
    void begin() {
        rtc_.begin();
    }

    void update() {
        if (currentMode != ControlMode::AUTO) return;
        if (player.getState() == PlayerState::PLAYING) return;

        unsigned long currentTimeMs = millis();

        if (currentTimeMs - lastPollMs_ < 500) return;

        lastPollMs_ = currentTimeMs;

        TimeOfDay now = rtc_.getTimeOfDay();
        int currentTotalMinutes = now.hour * 60 + now.minute;

        if (currentTotalMinutes == lastTriggeredMinute_) return;

        if (now.minute % 10 == 0) {
            if (now.hour >= 10 && now.hour < 18) {
                lastTriggeredMinute_ = currentTotalMinutes;
                Serial.println(F("INFO: Triggering daytime preset."));
            }
            else if (now.hour >= 18 && now.hour < 22) {
                lastTriggeredMinute_ = currentTotalMinutes;
                Serial.println(F("INFO: Triggering nighttime preset."));
            }
        }
    }

    void printTime() {
        rtc_.printToSerial();
    }

private:
    RtcModule rtc_;
    unsigned long lastPollMs_ = 0;
    int lastTriggeredMinute_ = -1;
};

#endif
