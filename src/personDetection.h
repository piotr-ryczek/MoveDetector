#ifndef PERSON_DETECTION_H
#define PERSON_DETECTION_H

#include <vector>

using namespace std;

enum CHANGE_ENUM { NONE, APPEARED, DISAPPEARED };
enum SENSOR_ENUM { INSIDE, OUTSIDE };

struct DetectorItem {
    SENSOR_ENUM sensor;
    CHANGE_ENUM change;
};

class PersonDetector {
  public:
    PersonDetector();
    int update(bool insideIsDetected, bool outsideIsDetected);
    
  private:
    vector<DetectorItem> detectionSequence;

    void clearSequence();
};

#endif
