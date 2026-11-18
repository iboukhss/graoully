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
        lastMillis_ = millis();
    }

    void update() {
        if (currentMode != ControlMode::AUTO) return;
        if (player.getState() == PlayerState::PLAYING) return;

        unsigned long currentMillis = millis();

        if (!rtc_.isReady()) {
            const unsigned long kIntervalMs = 10UL * 60UL * 1000UL;

            if (currentMillis - lastMillis_ >= kIntervalMs) {
                lastMillis_ = currentMillis;
                Serial.println(F("INFO: RTC offline. Falling back to scheduling shows every 10 minutes."));
                player.playRandomPreset();
            }
            return;
        }

        if (currentMillis - lastPollMs_ < 500) return;

        lastPollMs_ = currentMillis;

        TimeOfDay now = rtc_.getTimeOfDay();
        int currentTotalMinutes = now.hour * 60 + now.minute;

        if (currentTotalMinutes == lastTriggeredMinute_) return;

        DayOfWeek day = rtc_.getDayOfWeek();

        // 10 PM on Fri-Sat, 9 PM on Sun-Thu
        int eveningEndHour = (day >= DayOfWeek::FRIDAY) ? 22 : 21;

        // Daytime block: 11:00 to 17:00 (Every 15 minutes)
        if (now.hour >= 11 && now.hour < 17) {
            if (now.minute % 15 == 0) {
                lastTriggeredMinute_ = currentTotalMinutes;
                Serial.println(F("INFO: Triggering daytime preset."));
                player.playRandomPreset();
            }
        }
        // Evening block: 17:00 to eveningEndHour (Every 10 minutes)
        else if (now.hour >= 17 && now.hour < eveningEndHour) {
            if (now.minute % 10 == 0) {
                lastTriggeredMinute_ = currentTotalMinutes;
                Serial.println(F("INFO: Triggering nighttime preset."));
                player.playRandomPreset();
            }
        }
    }

    RtcModule& getClock() { return rtc_; }

private:
    RtcModule rtc_;
    unsigned long lastPollMs_ = 0;
    unsigned long lastMillis_ = 0;
    int lastTriggeredMinute_ = -1;
};

#endif
