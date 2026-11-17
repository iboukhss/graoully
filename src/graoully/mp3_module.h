#ifndef MP3_MODULE_H
#define MP3_MODULE_H

#include <DFRobot_DF1201S.h>

class Mp3Module {
public:
    Mp3Module(HardwareSerial& serial) : serial_(serial) {}

    void begin() {
        serial_.begin(115200);

        const int kMaxAttempts = 5;

        for (int attempt = 1; attempt <= kMaxAttempts; attempt++) {
            Serial.print(F("INFO: Initializing MP3 module (attempt "));
            Serial.print(attempt);
            Serial.print(F("/"));
            Serial.print(kMaxAttempts);
            Serial.println(F(")..."));

            if (mp3_.begin(serial_)) {
                mp3_.setPrompt(false);
                mp3_.setVol(15);
                mp3_.switchFunction(mp3_.MUSIC);
                mp3_.setPlayMode(mp3_.SINGLE);

                Serial.println(F("OK: MP3 module initialized successfully."));
                return;
            }

            delay(500);
        }

        Serial.println(F("ERROR: Couldn't initialize MP3 module."));
        isReady_ = false;
    }

    void playFileNumber(int fileNum) {
        if (!isReady_) return;

        mp3_.playFileNum(fileNum);
    }

    void setVolume(int volume) {
        if (!isReady_) return;

        mp3_.setVol(volume);
    }

    void play() {
        if (!isReady_) return;

        mp3_.start();
    }

    bool isReady() const { return isReady_; }

private:
    HardwareSerial& serial_;
    DFRobot_DF1201S mp3_;

    bool isReady_ = false;
};

#endif
