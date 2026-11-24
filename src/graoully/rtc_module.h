#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <DFRobot_DS3231M.h>

#include "log.h"

struct TimeOfDay {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

enum class DayOfWeek : uint8_t {
    SUNDAY = 0,
    MONDAY,
    TUESDAY,
    WEDNESDAY,
    THURSDAY,
    FRIDAY,
    SATURDAY
};

class RtcModule {
public:
    void begin() {
        const int kMaxAttempts = 5;

        for (int attempt = 1; attempt <= kMaxAttempts; attempt++) {
            LOG_INFO_BEGIN();
            Serial.print(F("rtc: Initializing... (attempt "));
            Serial.print(attempt);
            Serial.print(F("/"));
            Serial.print(kMaxAttempts);
            Serial.print(F(")"));
            LOG_INFO_END();

            if (rtc_.begin()) {
                if (rtc_.lostPower()) {
                    LOG_WARN(F("rtc: Module lost power - please set the time manually"));
                }

                // 1Hz ticks every second
                if (rtc_.readSqwPinMode() != eDS3231M_SquareWave_1Hz) {
                    rtc_.writeSqwPinMode(eDS3231M_SquareWave_1Hz);
                    LOG_INFO(F("rtc: SQW pin reconfigured to 1Hz"));
                }

                isReady_ = true;

                LOG_INFO(F("rtc: Initialized successfully"));

                LOG_INFO_BEGIN();
                Serial.print(F("rtc: Current time: "));
                printToSerial();
                LOG_INFO_END();
                return;
            }

            delay(500);
        }

        isReady_ = false;

        LOG_ERR(F("rtc: Failed to initialize RTC module"));
    }

    void setDateTime(int year, int month, int day, int hour, int minute, int second) {
        if (!isReady_ ) {
            LOG_ERR(F("rtc: Command ignored (module offline)"));
            return;
        }

        rtc_.setYear(year >= 2000 ? year - 2000 : year);
        rtc_.setMonth(month);
        rtc_.setDate(day);
        rtc_.setHour(hour, e24hours);
        rtc_.setMinute(minute);
        rtc_.setSecond(second);
        rtc_.adjust();

        LOG_INFO(F("rtc: Time updated successfully"));
    }

    void printToSerial()
    {
        if (!isReady_ ) {
            LOG_ERR(F("rtc: Command ignored (module offline)"));
            return;
        }

        rtc_.getNowTime();
    
        Serial.print(rtc_.year(), DEC); Serial.print('-');
        if (rtc_.month() < 10) Serial.print('0');
        Serial.print(rtc_.month(), DEC); Serial.print('-');
        if (rtc_.day() < 10) Serial.print('0');
        Serial.print(rtc_.day(), DEC); Serial.print(' ');

        if (rtc_.hour() < 10) Serial.print('0');
        Serial.print(rtc_.hour(), DEC); Serial.print(':');
        if (rtc_.minute() < 10) Serial.print('0');
        Serial.print(rtc_.minute(), DEC); Serial.print(':');

        if (rtc_.second() < 10) Serial.print('0');
        Serial.print(rtc_.second(), DEC);
    }

    TimeOfDay getTimeOfDay() {
        rtc_.getNowTime();
        return { rtc_.hour(), rtc_.minute(), rtc_.second() };
    }

    DayOfWeek getDayOfWeek() {
        rtc_.getNowTime();

        const char* dayStr = rtc_.getDayOfTheWeek();

        if (strcmp(dayStr, "Sunday") == 0) return DayOfWeek::SUNDAY;
        if (strcmp(dayStr, "Monday") == 0) return DayOfWeek::MONDAY;
        if (strcmp(dayStr, "Tuesday") == 0) return DayOfWeek::TUESDAY;
        if (strcmp(dayStr, "Wednesday") == 0) return DayOfWeek::WEDNESDAY;
        if (strcmp(dayStr, "Thursday") == 0) return DayOfWeek::THURSDAY;
        if (strcmp(dayStr, "Friday") == 0) return DayOfWeek::FRIDAY;
        if (strcmp(dayStr, "Saturday") == 0) return DayOfWeek::SATURDAY;

        return DayOfWeek::SUNDAY;
    }

    bool isReady() const { return isReady_; }

private:
    DFRobot_DS3231M rtc_;

    bool isReady_ = false;
};

#endif
