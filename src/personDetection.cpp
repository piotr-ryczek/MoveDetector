#include <personDetection.h>
#include <Arduino.h>

PersonDetector::PersonDetector() {
  detectionSequence = {};
}

int PersonDetector::update(bool insideIsDetected, bool outsideIsDetected) {
    switch (detectionSequence.size()) {
        case 0: {
             if (insideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::APPEARED});
            } else if (outsideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::APPEARED});
            }

            return 0;
        }
        case 1: {
            if (detectionSequence[0].sensor == SENSOR_ENUM::INSIDE) {
                if (!insideIsDetected) {
                    this->clearSequence();
                    return 0;
                }

                if (outsideIsDetected) {
                    detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::APPEARED});
                }
            } else {
                if (!outsideIsDetected) {
                    this->clearSequence();
                    return 0;
                }

                if (insideIsDetected) {
                    detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::APPEARED});
                }
            }

            return 0;
        }
        case 2: {
            if (!insideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::INSIDE, CHANGE_ENUM::DISAPPEARED});

                return 0;
            }

            if (!outsideIsDetected) {
                detectionSequence.push_back({SENSOR_ENUM::OUTSIDE, CHANGE_ENUM::DISAPPEARED});

                return 0;
            }

            return 0;
        }
        case 3: {
            if (insideIsDetected) {
                if (detectionSequence[2].sensor == SENSOR_ENUM::INSIDE) {
                    detectionSequence.pop_back();
                }

                return 0;
            }

            if (outsideIsDetected) {
                if (detectionSequence[2].sensor == SENSOR_ENUM::OUTSIDE) {
                    detectionSequence.pop_back();
                }

                return 0;
            }

            if (!insideIsDetected && detectionSequence[2].sensor == SENSOR_ENUM::OUTSIDE) {
                if (detectionSequence[0].sensor == SENSOR_ENUM::INSIDE) {
                    this->clearSequence();

                    return 0;
                } else {
                    this->clearSequence();

                    return 1;
                }
            }

            if (!outsideIsDetected && detectionSequence[2].sensor == SENSOR_ENUM::INSIDE) {
                if (detectionSequence[0].sensor == SENSOR_ENUM::OUTSIDE) {
                    this->clearSequence();

                    return 0;
                } else {
                    this->clearSequence();

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