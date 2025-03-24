#ifndef SONAR_WRAPPER_H
#define SONAR_WRAPPER_H

#include <tuple>
#include <NewPing.h>

const int WINDOW_WIDTH = 750; // In mm
const int HUMAN_DETECTION_DISTANCE = 600; // In mm

class SonarWrapper {
    public:
        SonarWrapper(int trigPin, int echoPin, int maxDistance);
        int getDistanceInMM();
        std::tuple<bool, int> checkIsDetected();
    private:
        NewPing sonar;
};

#endif