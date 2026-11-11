#ifndef LED_H
#define LED_H

class Led {
public:
    Led(const char* name, int p) : name_(name), pin_(p) {}

    void begin() {
        if (pin_ == -1) return;

        pinMode(pin_, OUTPUT);
    }

    void setBrightness(int level) {
        if (pin_ == -1) return;

        analogWrite(pin_, level);
    }

    void on() {
        setBrightness(255);
    }

    void off() {
        setBrightness(0);
    }

    const char* getName() const { return name_; }

private:
    const char* name_;
    int pin_;
};

#endif
