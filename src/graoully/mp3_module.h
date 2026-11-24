#ifndef MP3_MODULE_H
#define MP3_MODULE_H

#include <DFRobot_DF1201S.h>

#include "log.h"

class Mp3Module {
public:
    Mp3Module(HardwareSerial& serial) : serial_(serial) {}

    void begin() {
        serial_.begin(115200);

        const int kMaxAttempts = 5;

        for (int attempt = 1; attempt <= kMaxAttempts; attempt++) {
            LOG_INFO_BEGIN();
            Serial.print(F("mp3: initializing (attempt "));
            Serial.print(attempt);
            Serial.print(F("/"));
            Serial.print(kMaxAttempts);
            Serial.print(F(")..."));
            LOG_INFO_END();

            if (mp3_.begin(serial_)) {
                mp3_.setPrompt(false);
                mp3_.switchFunction(mp3_.MUSIC);
                mp3_.setPlayMode(mp3_.SINGLE);
                mp3_.setVol(15);

                isReady_ = true;

                LOG_INFO(F("mp3: initialized successfully"));
                return;
            }

            delay(500);
        }

        isReady_ = false;

        LOG_ERR(F("mp3: failed to initialize MP3 module"));
    }

    void playFileNumber(int fileNum) {
        if (!isReady_) {
            LOG_ERR(F("mp3: command ignored (module offline)"));
            return;
        }

        mp3_.playFileNum(fileNum);

        delay(300);

        LOG_INFO_BEGIN();
        Serial.print(F("mp3: currently playing: #"));
        Serial.print(fileNum);
        Serial.print(F(" - "));
        Serial.print(mp3_.getFileName());
        LOG_INFO_END();
    }

    void setVolume(int volume) {
        if (!isReady_) {
            LOG_ERR(F("mp3: command ignored (module offline)"));
            return;
        }

        mp3_.setVol(volume);

        LOG_INFO_BEGIN();
        Serial.print(F("mp3: volume set to "));
        Serial.print(volume);
        LOG_INFO_END();
    }

    void play() {
        if (!isReady_) return;

        mp3_.start();
    }

    void pause() {
        if (!isReady_) return;

        mp3_.pause();
    }
    
    void printToSerial() {
        if (!isReady_) {
            LOG_ERR(F("mp3: command ignored (module offline)"));
            return;
        }

        Serial.println(F("--- MP3 status ---"));
        Serial.print(F("Total files: ")); Serial.println(mp3_.getTotalFile());
        Serial.print(F("Current track: #")); Serial.println(mp3_.getCurFileNumber());
        Serial.print(F("File name: ")); Serial.println(mp3_.getFileName());
        Serial.print(F("Volume: ")); Serial.println(mp3_.getVol());
        Serial.println(F("-------------------------------------------------------"));
    }

    bool isReady() const { return isReady_; }

private:
    HardwareSerial& serial_;
    DFRobot_DF1201S mp3_;

    bool isReady_ = false;
};

#endif
