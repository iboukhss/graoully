#ifndef LED_H
#define LED_H

enum class LedState {
    OFF,
    ON,
    TIMED
};

class Led {
public:
    Led(const char* name, int p) : name_(name), pin_(p) {}

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

        setBrightness(255);
        state_ = LedState::TIMED;
        durationMs_ = durationMs;
        startTimeMs_ = millis();
    }

    const char* getName() const { return name_; }
    LedState getState() const { return state_; }

private:
    const char* name_;
    int pin_;

    LedState state_ = LedState::OFF;
    unsigned long startTimeMs_ = 0;
    unsigned long durationMs_ = 0;
};

#endif
