#include <personDetection.h>
#include <Arduino.h>

PersonDetector::PersonDetector() {
  detectionSequence = {};
}

unsigned long initialSensorAppearedMillis = 0;
unsigned long finishSequenceMillis = 0;
const int initialSensorAppearedTimeout = 1000; // 1 second
const int finishSequenceTimeout = 500;

int PersonDetector::update(bool insideIsDetected, bool outsideIsDetected) {
    switch (detectionSequence.size()) {
        case 0: {
            if (millis() - finishSequenceMillis < finishSequenceTimeout) {
                return 0;
            }

            initialSensorAppearedMillis = millis();

             if (insideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::APPEARED});
                Serial.println("0 -> 1");
            } else if (outsideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::APPEARED});
                Serial.println("0 -> 1");
            }

            return 0;
        }
        case 1: {
            if (detectionSequence[0].sensor == SENSOR_ENUM::INSIDE) {
                if (outsideIsDetected) {
                    detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::APPEARED});
                    Serial.println("1 -> 2");

                    return 0;
                }

                if (!insideIsDetected) {
                    if (millis() - initialSensorAppearedMillis > initialSensorAppearedTimeout) {
                        this->clearSequence();
                        Serial.println("1 -> 0");
                    } else {
                        Serial.println("[1]");
                    }

                    return 0;
                }

                if (insideIsDetected) {
                    initialSensorAppearedMillis = millis();
                }
            } else {
                if (insideIsDetected) {
                    detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::APPEARED});
                    Serial.println("1 -> 2");

                    return 0;
                }

                if (!outsideIsDetected) {
                    if (millis() - initialSensorAppearedMillis > initialSensorAppearedTimeout) {
                        this->clearSequence();
                        Serial.println("1 -> 0");
                    } else {
                        Serial.println("[1]");
                    }

                    return 0;
                }

                if (outsideIsDetected) {
                    initialSensorAppearedMillis = millis();
                }
            }

            return 0;
        }
        case 2: {
            if (!insideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::DISAPPEARED});
                Serial.println("2 -> 3");
                return 0;
            }

            if (!outsideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::DISAPPEARED});
                Serial.println("2 -> 3");
                return 0;
            }

            return 0;
        }
        case 3: {
            if (insideIsDetected) {
                if (detectionSequence[2].sensor == SENSOR_ENUM::INSIDE) {
                    detectionSequence.pop_back();
                    Serial.println("3 -> 2");
                }

                return 0;
            }

            if (outsideIsDetected) {
                if (detectionSequence[2].sensor == SENSOR_ENUM::OUTSIDE) {
                    detectionSequence.pop_back();
                    Serial.println("3 -> 2");
                }

                return 0;
            }

            if (!insideIsDetected && detectionSequence[2].sensor == SENSOR_ENUM::OUTSIDE) {
                if (detectionSequence[0].sensor == SENSOR_ENUM::INSIDE) {
                    this->clearSequence();
                    Serial.println("3 -> 0");

                    return 0;
                } else {
                    this->clearSequence();
                    finishSequenceMillis = millis();
                    Serial.println("+ 1");
                    return 1;
                }
            }

            if (!outsideIsDetected && detectionSequence[2].sensor == SENSOR_ENUM::INSIDE) {
                if (detectionSequence[0].sensor == SENSOR_ENUM::OUTSIDE) {
                    this->clearSequence();
                    Serial.println("3 -> 0");

                    return 0;
                } else {
                    this->clearSequence();
                    finishSequenceMillis = millis();
                    Serial.println("- 1");

                    return -1;
                }
            }

            return 0;
        }
        default: {
            throw std::runtime_error("Detection sequence has more than 3 items");
        }
    }

    return 0;
}

void PersonDetector::clearSequence() {
    detectionSequence = {};
}