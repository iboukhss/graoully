#ifndef SHOW_H
#define SHOW_H

#include <assert.h>

extern void executeCommand(const String& target, const String& action);

struct ShowItem {
    unsigned long offsetMs;
    char target[16];
    char action[16];
};

struct ShowPreset {
    char name[16];
    const ShowItem* data;
    int count;
};

constexpr int kMaxShowCapacity = 32;

class Show {
public:
    Show(const String& name) : name_(name) {}

    void append(unsigned long offset, const String& target, const String& action) {
        if (count_ >= kMaxShowCapacity) {
            Serial.println(F("WARNING: Maximum show capacity reached!"));
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

        for (int i = 0; i < localPreset.count && i < kMaxShowCapacity; i++) {
            memcpy_P(&buffer_[i], &localPreset.data[i], sizeof(localPreset.data[i]));
            count_++;
        }

        Serial.print(F("Loaded preset: "));
        Serial.print(name_);
        Serial.print(F(", Items: "));
        Serial.println(count_);
    }

    void printToSerial() const {
        Serial.print(F("--- Show: "));
        Serial.print(name_);
        Serial.print(F(", Items: "));
        Serial.print(count_);
        Serial.println(F(" ---"));

        for (int i = 0; i < count_; i++) {
            Serial.print(buffer_[i].offsetMs);
            Serial.print(F(","));
            Serial.print(buffer_[i].target);
            Serial.print(F(","));
            Serial.println(buffer_[i].action);
        }

        Serial.println(F("-------------------------------------"));
    }

    const String& getName() const { return name_; }
    int getCount() const { return count_; }
    const ShowItem& getItemAt(int index) const { return buffer_[index]; }
    void clear() { count_ = 0; }

private:
    String name_;

    ShowItem buffer_[kMaxShowCapacity];
    int count_ = 0;
};

#endif
