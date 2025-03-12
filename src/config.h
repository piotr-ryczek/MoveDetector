#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include <Arduino.h>

// Flags
extern bool hasNTPAlreadyConfigured;
extern bool isNTPUnderConfiguration;
extern bool isWifiConnected;
extern bool isHttpQueriesQueueOccupied;
extern String todayDate;

extern const int GMT_OFFSET_SEC;
extern const int DAYLIGHT_OFFSET_SEC;
extern const char* NTP_SERVER_URL;

extern const char* BACKEND_APP_URL;

extern const char* WLED_URL;


#endif