#ifndef LED_H
#define LED_H

enum class LedState {
    OFF,
    ON,
    TIMED,
    BLINK,
    BLINK_TIMED
};

class Led {
public:
    Led(const String& name, int p) : name_(name), pin_(p) {}

    void begin() {
        if (pin_ == -1) return;

        pinMode(pin_, OUTPUT);
    }

    void update() {
        if (pin_ == -1) return;

        switch (state_) {
        case LedState::OFF:
        case LedState::ON:
            break;
        
        case LedState::TIMED:
            if (millis() - startTimeMs_ >= durationMs_) {
                off();
            }
            break;

        case LedState::BLINK:
            if (millis() - lastToggleMs_ >= blinkIntervalMs_) {
                isLedOn_ = !isLedOn_;
                setBrightness(isLedOn_ ? 255 : 0);
                lastToggleMs_ = millis();
            }
            break;

        case LedState::BLINK_TIMED:
            if (millis() - startTimeMs_ >= durationMs_) {
                off();
                break;
            }
            if (millis() - lastToggleMs_ >= blinkIntervalMs_) {
                isLedOn_ = !isLedOn_;
                setBrightness(isLedOn_ ? 255 : 0);
                lastToggleMs_ = millis();
                break;
            }
            break;
        }
    }

    void setBrightness(int level) {
        if (pin_ == -1) return;

        analogWrite(pin_, level);
    }

    void on() {
        if (pin_ == -1) return;
    
        setBrightness(255);
        state_ = LedState::ON;
    }

    void off() {
        if (pin_ == -1) return;
    
        setBrightness(0);
        state_ = LedState::OFF;
    }

    void onFor(unsigned long durationMs) {
        if (pin_ == -1) return;

        durationMs_ = durationMs;
        setBrightness(255);
        startTimeMs_ = millis();
        state_ = LedState::TIMED;
    }

    void blink(unsigned long intervalMs) {
        if (pin_ == -1) return;

        blinkIntervalMs_ = intervalMs;
        setBrightness(255);
        lastToggleMs_ = millis();
        isLedOn_ = true;
        state_ = LedState::BLINK;
    }

    void blinkFor(unsigned long durationMs, unsigned long intervalMs = 100) {
        if (pin_ == -1) return;

        durationMs_ = durationMs;
        blinkIntervalMs_ = intervalMs;
        setBrightness(255);
        startTimeMs_ = millis();
        lastToggleMs_ = startTimeMs_;
        isLedOn_ = true;
        state_ = LedState::BLINK_TIMED;
    }

    const String& getName() const { return name_; }
    LedState getState() const { return state_; }

private:
    String name_;

    int pin_ = -1;

    LedState state_ = LedState::OFF;
    unsigned long startTimeMs_ = 0;
    unsigned long durationMs_ = 0;
    unsigned long blinkIntervalMs_ = 0;
    unsigned long lastToggleMs_ = 0;
    bool isLedOn_ = false;
};

#endif
