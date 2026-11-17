#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <DFRobot_DS3231M.h>

struct TimeOfDay {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
};

class RtcModule {
public:
    void begin() {
        const int kMaxAttempts = 5;

        for (int attempt = 1; attempt <= kMaxAttempts; attempt++) {
            Serial.print(F("INFO: Initializing RTC module (attempt "));
            Serial.print(attempt);
            Serial.print(F("/"));
            Serial.print(kMaxAttempts);
            Serial.println(F(")..."));

            if (rtc_.begin()) {
                if (rtc_.lostPower()) {
                    Serial.println(F("WARNING: RTC module lost power. Initializing clock with last compile time."));
                    rtc_.dateTime();
                    rtc_.adjust();
                }

                // 1Hz ticks every second
                if (rtc_.readSqwPinMode() != eDS3231M_SquareWave_1Hz) {
                    rtc_.writeSqwPinMode(eDS3231M_SquareWave_1Hz);
                    Serial.println(F("INFO: SQW pin reconfigured to 1Hz."));
                }

                Serial.println(F("OK: RTC module initialized successfully."));
                isReady_ = true;
                return;
            }

            delay(500);
        }

        Serial.println(F("ERROR: Couldn't initialize RTC module."));
        isReady_ = false;
    }

    TimeOfDay getTimeOfDay() {
        rtc_.getNowTime();
        return { rtc_.hour(), rtc_.minute(), rtc_.second() };
    }

    void printToSerial()
    {
        if (!isReady_ ) {
            Serial.println(F("ERROR: RTC module is offline."));
            return;
        }

        rtc_.getNowTime();
        Serial.print(rtc_.year(), DEC); Serial.print('/');
        Serial.print(rtc_.month(), DEC); Serial.print('/');
        Serial.print(rtc_.day(), DEC); Serial.print(' ');
        Serial.print(rtc_.hour(), DEC); Serial.print(':');
        Serial.print(rtc_.minute(), DEC); Serial.print(':');
        Serial.println(rtc_.second(), DEC);
    }

    bool isReady() const { return isReady_; }

private:
    DFRobot_DS3231M rtc_;

    bool isReady_ = false;
};

#endif
