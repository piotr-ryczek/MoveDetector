#include "sonarWrapper.h"

SonarWrapper::SonarWrapper(int trigPin, int echoPin, int maxDistance) : sonar(trigPin, echoPin, maxDistance) {}

int SonarWrapper::getDistanceInMM() {
    unsigned int uS = this->sonar.ping();

    return uS / 5.8;
}

bool SonarWrapper::checkIsDetected() {
    return this->getDistanceInMM() < HUMAN_DETECTION_DISTANCE;
}