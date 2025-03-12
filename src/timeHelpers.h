#ifndef TIME_HELPERs_H
#define TIME_HELPERs_H

#include <Arduino.h>
#include <time.h>

String getCurrentDateTime();
String extractDate(String dateTime);
String extractTime(String dateTime);
String addDayToDate(String date);

#endif