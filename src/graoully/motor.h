#ifndef MOTOR_H
#define MOTOR_H

// Example motor 1

// Stroke:     50mm
// Max load:   1000N (100kg)
// Speed:      14mm/s
// Duty cycle: 10%

// Approx 3.57 seconds to fully extend/retract

class Motor {
public:
    Motor(const char* name, int ena, int in1, int in2, float stroke, float speed)
      : name_(name), pinEna_(ena), pinIn1_(in1), pinIn2_(in2), strokeMm_(stroke), speedMmS_(speed)
    {
        // Let's assume we start dead center (this doesn't achieve anything)
        startPositionMm_ = stroke * 0.5f;
    }

    void begin() {
        if (pinEna_ == -1) return;

        pinMode(pinEna_, OUTPUT);
        pinMode(pinIn1_, OUTPUT);
        pinMode(pinIn2_, OUTPUT);
    }

    void update() {
        if (pinEna_ == -1 || !isMoving_) return;

        unsigned long elapsedTimeMs = millis() - startTimeMs_;

        if (elapsedTimeMs >= travelTimeMs_) {
            brake();
        }

        updateCurrentPosition();
    }

    void home() {
        if (pinEna_ == -1) return;

        if (isMoving_) {
            Serial.println(F("WARNING: Motor is busy. 'home' command ignored."));
            return;
        }

        targetPositionMm_ = 0.0f;

        float fullStrokeTimeMs = (strokeMm_ / speedMmS_) * 1000.0f;
        travelTimeMs_ = fullStrokeTimeMs * 1.4f;

        retract();
    }

    void moveTo(float position) {
        if (pinEna_ == -1) return;

        if (isMoving_) {
            updateCurrentPosition();
        }

        targetPositionMm_ = strokeMm_ * position;

        float distanceMm = abs(targetPositionMm_ - currentPositionMm_);

        // 0.1mm tolerance to avoid wearing down the motor for nothing
        if (distanceMm < 0.1f) return;

        travelTimeMs_ = (distanceMm / speedMmS_) * 1000.0f;

        // If we move to an absolute boundary (0.0 or 1.0) then we add a safety
        // margin proportional to the full stroke length, here 20%
        if (position <= 0.0f || position >= 1.0f) {
            float fullStrokeTimeMs = (strokeMm_ / speedMmS_) * 1000.0f;
            travelTimeMs_ += (fullStrokeTimeMs * 0.05f);
        }

        if (targetPositionMm_ > currentPositionMm_) {
            extend();
        }
        else {
            retract();
        }
    }

    void stop() {
        if (pinEna_ == -1 || !isMoving_) return;

        brake();
        updateCurrentPosition();
    }

    const char* getName() const { return name_; }
    float getPosition() const { return currentPositionMm_; }
    bool isMoving() const { return isMoving_; }

private:
    void extend() {
        if (pinEna_ == -1) return;

        digitalWrite(pinIn1_, HIGH);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        startPositionMm_ = currentPositionMm_;
        isMoving_ = true;
    }

    void retract() {
        if (pinEna_ == -1) return;

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, HIGH);
        analogWrite(pinEna_, 255);

        startTimeMs_ = millis();
        startPositionMm_ = currentPositionMm_;
        isMoving_ = true;
    }

    void brake() {
        if (pinEna_ == -1 || !isMoving_) return;

        digitalWrite(pinIn1_, LOW);
        digitalWrite(pinIn2_, LOW);
        analogWrite(pinEna_, 0);

        isMoving_ = false;
    }

    void updateCurrentPosition() {
        unsigned long elapsedTimeMs = millis() - startTimeMs_;
        float distanceTravelledMm = (elapsedTimeMs / 1000.0f) * speedMmS_;

        if (targetPositionMm_ > startPositionMm_) {
            currentPositionMm_ = constrain(startPositionMm_ + distanceTravelledMm, 0.0f, strokeMm_);
        }
        else {
            currentPositionMm_ = constrain(startPositionMm_ - distanceTravelledMm, 0.0f, strokeMm_);
        }
    }

    const char* name_;

    int pinEna_;
    int pinIn1_;
    int pinIn2_;

    float strokeMm_;
    float speedMmS_;

    float targetPositionMm_ = 0.0f;
    float startPositionMm_ = 0.0f;
    float currentPositionMm_ = 0.0f;

    unsigned long travelTimeMs_ = 0;
    unsigned long startTimeMs_ = 0;

    bool isMoving_ = false;
};

#endif
