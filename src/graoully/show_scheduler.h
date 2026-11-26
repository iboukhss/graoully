#ifndef SHOW_SCHEDULER_H
#define SHOW_SCHEDULER_H

#include "rtc_module.h"

enum class ControlMode {
    AUTO,
    MANUAL
};

extern ControlMode currentMode;
extern ShowPlayer player;
extern bool allowNightEffects;

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
            if (currentMillis - lastMillis_ >= kShowIntervalMs) {
                lastMillis_ = currentMillis;
                LOG_INFO_BEGIN();
                Serial.print(F("sched: RTC offline - falling back to scheduling shows every "));
                Serial.print(kShowIntervalMinutes);
                Serial.print(F(" minutes"));
                LOG_INFO_END();
                player.playRandomPreset();
            }
            return;
        }

        if (currentMillis - lastPollMs_ < 500) return;

        lastPollMs_ = currentMillis;

        TimeOfDay now = rtc_.getTimeOfDay();

        if (now.hour >= 24 || now.minute >= 60 || now.second >= 60) {
            LOG_ERR_BEGIN();
            Serial.print(F("sched: Invalid time: "));
            Serial.print(F("hours="));
            Serial.print(now.hour);
            Serial.print(F(", minutes="));
            Serial.print(now.minute);
            Serial.print(F(", seconds="));
            Serial.print(now.second);
            LOG_ERR_END();
            return;
        }

        int currentTotalMinutes = now.hour * 60 + now.minute;

        if (currentTotalMinutes == lastTriggeredMinute_) return;

        if (now.hour >= 16 && now.hour < 22) {
            if (now.minute % kShowIntervalMinutes == 0) {
                lastTriggeredMinute_ = currentTotalMinutes;
                player.playRandomPreset();
            }
        }
    }

    RtcModule& getClock() { return rtc_; }

private:
    static constexpr unsigned long kShowIntervalMinutes = 5UL;
    static constexpr unsigned long kShowIntervalMs = kShowIntervalMinutes * 60UL * 1000UL;

    RtcModule rtc_;
    unsigned long lastPollMs_ = 0;
    unsigned long lastMillis_ = 0;
    int lastTriggeredMinute_ = -1;
};

#endif
