#ifndef SHOW_PLAYER_H
#define SHOW_PLAYER_H

#include "show.h"
#include "show_presets.h"

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

class ShowPlayer {
public:
    void play(const String& presetName) {
        const ShowPreset* ptr = getPreset(presetName);
        if (!ptr) {
            Serial.print(F("ERROR: Show preset not found -> "));
            Serial.println(presetName);
            return;
        }

        flashSlot_.loadFromFlash(ptr);

        currentShow_ = &flashSlot_;
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        Serial.print(F("Currently playing: "));
        Serial.println(flashSlot_.getName());

        startTimeMs_ = millis();
    }

    void playSerialSlot() {
        if (!serialSlot_.getCount()) {
            Serial.println(F("ERROR: Serial slot is empty! Nothing to do."));
            return;
        }

        currentShow_ = &serialSlot_;
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        Serial.print(F("Currently playing: "));
        Serial.println(serialSlot_.getName());

        startTimeMs_ = millis();
    }

    void pause() {
        if (state_ != PlayerState::PLAYING) return;

        state_ = PlayerState::PAUSED;

        Serial.println(F("Show paused."));
    }

    void stop() {
        state_ = PlayerState::STOPPED;
        currentIndex_ = 0;
        currentShow_ = nullptr;

        Serial.println(F("Show stopped."));
    }

    void update() {
        if (state_ != PlayerState::PLAYING) return;

        assert(currentShow_ && "currentShow_ cannot be null!");

        const ShowItem& item = currentShow_->getItemAt(currentIndex_);
        unsigned long targetTriggerTimeMs = startTimeMs_ + item.offsetMs;

        if (millis() >= targetTriggerTimeMs) {
            executeCommand(item.target, item.action);
            currentIndex_++;
        }

        if (currentIndex_ >= currentShow_->getCount()) {
            Serial.println(F("Show finished playing. Stopping now."));
            stop();
        }
    }

    PlayerState getState() const { return state_; }
    Show& getSerialSlot() { return serialSlot_; }

private:
    Show* currentShow_ = nullptr;
    int currentIndex_ = 0;
    unsigned long startTimeMs_ = 0;

    Show flashSlot_{ "FLASH_SLOT" };
    Show serialSlot_{ "SERIAL_SLOT" };

    PlayerState state_ = PlayerState::STOPPED;
};

#endif
