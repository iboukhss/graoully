#ifndef SHOW_PLAYER_H
#define SHOW_PLAYER_H

#include "show.h"
#include "show_presets.h"

extern Dragon graoully;

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

class ShowPlayer {
public:
    void play(const String& presetName) {
        const ShowPreset* ptr = getPresetByName(presetName);
        if (!ptr) {
            LOG_ERR_BEGIN();
            Serial.print(F("player: preset not found: "));
            Serial.print(presetName);
            LOG_ERR_END();
            return;
        }

        flashSlot_.loadFromFlash(ptr);

        currentShow_ = &flashSlot_;
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        LOG_INFO_BEGIN();
        Serial.print(F("player: currently playing: "));
        Serial.print(currentShow_->getName());
        LOG_INFO_END();

        startTimeMs_ = millis();
    }

    void playRandomPreset() {
        const ShowPreset* ptr = getRandomPreset();
        if (!ptr) {
            LOG_ERR(F("player: no presets available"));
            return;
        }

        flashSlot_.loadFromFlash(ptr);

        currentShow_ = &flashSlot_;
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        LOG_INFO_BEGIN();
        Serial.print(F("player: currently playing (random): "));
        Serial.print(currentShow_->getName());
        LOG_INFO_END();

        startTimeMs_ = millis();
    }

    void playSerialSlot() {
        if (!serialSlot_.getCount()) {
            LOG_ERR(F("player: serial slot is empty"));
            return;
        }

        currentShow_ = &serialSlot_;
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        LOG_INFO_BEGIN();
        Serial.print(F("player: currently playing (serial slot): "));
        Serial.print(serialSlot_.getName());
        LOG_INFO_END();

        startTimeMs_ = millis();
    }

    void pause() {
        if (state_ != PlayerState::PLAYING) return;

        state_ = PlayerState::PAUSED;

        LOG_INFO(F("player: show paused"));
    }

    void stop() {
        currentShow_ = nullptr;
        currentIndex_ = 0;
        state_ = PlayerState::STOPPED;

        LOG_INFO(F("player: show stopped"));
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
            stop();
            graoully.home();
            graoully.reset();
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
