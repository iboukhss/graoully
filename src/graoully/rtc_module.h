#ifndef RTC_MODULE_H
#define RTC_MODULE_H

#include <DFRobot_DS3231M.h>

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
                    Serial.println(F("WARNING: RTC module lost power. Initializing clock as last compile time."));
                    rtc_.dateTime();
                    rtc_.adjust();
                }

                // 1Hz ticks every second
                rtc_.writeSqwPinMode(eDS3231M_SquareWave_1Hz);

                Serial.println(F("OK: RTC module initialized successfully."));
                return;
            }

            delay(500);
        }

        Serial.println(F("ERROR: Couldn't initialize RTC module."));
    }

    void printToSerial()
    {
        rtc_.getNowTime();
        Serial.print(rtc_.year(), DEC); Serial.print('/');
        Serial.print(rtc_.month(), DEC); Serial.print('/');
        Serial.print(rtc_.day(), DEC); Serial.print(' ');
        Serial.print(rtc_.hour(), DEC); Serial.print(':');
        Serial.print(rtc_.minute(), DEC); Serial.print(':');
        Serial.println(rtc_.second(), DEC);
    }

private:
    DFRobot_DS3231M rtc_;
};

#endif
