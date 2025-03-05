#include <Arduino.h>
#include <sonarWrapper.h>
#include <personDetection.h>
#define MAX_DISTANCE 400  // In cm

#define INSIDE_TRIG_PIN 12
#define INSIDE_ECHO_PIN 13
#define OUTSIDE_TRIG_PIN 27
#define OUTSIDE_ECHO_PIN 14

bool outsideIsDetected = false;
bool insideIsDetected = false;

int numberOfPeople = 0;

SonarWrapper sonarInside(INSIDE_TRIG_PIN, INSIDE_ECHO_PIN, MAX_DISTANCE);
SonarWrapper sonarOutside(OUTSIDE_TRIG_PIN, OUTSIDE_ECHO_PIN, MAX_DISTANCE);

PersonDetector personDetector;

void setup() {
  Serial.begin(115200);

  // TODO: Add 10s delay to get out of the room while initializing the system
}

void loop() {
  delay(50);

  insideIsDetected = sonarInside.checkIsDetected();
  outsideIsDetected = sonarOutside.checkIsDetected();

  int result = personDetector.update(insideIsDetected, outsideIsDetected);

  if (result == 1) {
    numberOfPeople += 1;
  } else if (result == -1) {
    numberOfPeople -= 1;
  }
}
