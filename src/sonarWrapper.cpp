#include "sonarWrapper.h"

SonarWrapper::SonarWrapper(int trigPin, int echoPin, int maxDistance) : sonar(trigPin, echoPin, maxDistance) {}

int SonarWrapper::getDistanceInMM() {
    unsigned int uS = this->sonar.ping();

    return uS / 5.8;
}

std::tuple<bool, int> SonarWrapper::checkIsDetected() {
    int distance = this->getDistanceInMM();

    return std::make_tuple(distance < HUMAN_DETECTION_DISTANCE, distance);
}