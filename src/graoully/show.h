#ifndef SHOW_H
#define SHOW_H

extern void executeCommand(const String& target, const String& action);

struct SequenceItem {
    unsigned long offsetMs;
    char target[16];
    char action[16];
};

struct ShowPreset {
    char name[16];
    const SequenceItem* data;
    int count;
};

constexpr int MAX_SHOW_SEQUENCE_CAPACITY = 32;

class ShowSequence {
public:
    ShowSequence(const String& name) : name_(name) {}

    void append(unsigned long offset, const String& target, const String& action) {
        if (count_ >= MAX_SHOW_SEQUENCE_CAPACITY) {
            Serial.println(F("WARNING: Maximum show sequence capacity reached!"));
            return;
        }

        buffer_[count_].offsetMs = offset;

        target.toCharArray(buffer_[count_].target, sizeof(buffer_[count_].target));
        action.toCharArray(buffer_[count_].action, sizeof(buffer_[count_].action));

        count_++;
    }

    void loadFromFlash(const ShowPreset* preset) {
        clear();

        ShowPreset localPreset;
        memcpy_P(&localPreset, preset, sizeof(*preset));

        name_ = localPreset.name;

        for (int i = 0; i < localPreset.count && i < MAX_SHOW_SEQUENCE_CAPACITY; i++) {
            memcpy_P(&buffer_[i], &localPreset.data[i], sizeof(localPreset.data[i]));
            count_++;
        }

        Serial.print(F("Loaded preset '"));
        Serial.print(name_);
        Serial.print(F("'. Items: "));
        Serial.println(count_);
    }

    const String& getName() const { return name_; }
    int getCount() const { return count_; }
    const SequenceItem& getItemAt(int index) const { return buffer_[index]; }
    void clear() { count_ = 0; }

private:
    String name_;

    SequenceItem buffer_[MAX_SHOW_SEQUENCE_CAPACITY];
    int count_ = 0;
};

enum class PlayerState {
    STOPPED,
    PLAYING,
    PAUSED
};

class SequencePlayer {
public:
    void load(ShowSequence& sequence) {
        if (sequence.getCount() == 0) {
            Serial.println(F("ERROR: Sequence is empty!"));
            return;
        }

        currentSequence_ = &sequence;
        currentIndex_ = 0;
        state_ = PlayerState::STOPPED;
        Serial.print(F("Loaded sequence: "));
        Serial.println(sequence.getName());
    }

    // Should play() also resume?

    void play() {
        if (!currentSequence_) {
            Serial.println(F("ERROR: No sequence loaded!"));
            return;
        }

        startTimeMs_ = millis();
        currentIndex_ = 0;
        state_ = PlayerState::PLAYING;

        Serial.print(F("Currently playing: "));
        Serial.println(currentSequence_->getName());
    }

    void pause() {
        if (state_ != PlayerState::PLAYING) return;

        state_ = PlayerState::PAUSED;

        Serial.println(F("Sequence paused."));
    }

    void stop() {
        state_ = PlayerState::STOPPED;
        currentIndex_ = 0;
        currentSequence_ = nullptr;

        Serial.println(F("Sequence stopped."));
    }

    void update() {
        if (state_ != PlayerState::PLAYING) return;

        assert(currentSequence_ && "currentSequence_ cannot be null!");

        const SequenceItem& item = currentSequence_->getItemAt(currentIndex_);
        unsigned long targetTriggerTimeMs = startTimeMs_ + item.offsetMs;

        if (millis() >= targetTriggerTimeMs) {
            executeCommand(item.target, item.action);
            currentIndex_++;
        }

        if (currentIndex_ >= currentSequence_->getCount()) {
            Serial.println(F("Sequence finished playing. Stopping now."));
            stop();
        }
    }

    PlayerState getState() const { return state_; }

private:
    ShowSequence* currentSequence_ = nullptr;
    int currentIndex_ = 0;
    unsigned long startTimeMs_ = 0;

    PlayerState state_ = PlayerState::STOPPED;
};

#endif
