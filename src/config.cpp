#include <config.h>

bool hasNTPAlreadyConfigured = false; // Can be changed only once
bool isNTPUnderConfiguration = false;
bool isWifiConnected = false;
bool isHttpQueriesQueueOccupied = false;
String todayDate = "";

const int GMT_OFFSET_SEC = 3600;
const int DAYLIGHT_OFFSET_SEC = 3600;
const char* NTP_SERVER_URL = "pool.ntp.org";

const char* BACKEND_APP_URL = "http://window-opening.nero12.usermd.net";
// Office 18
// Bedroom 249
const char* WLED_URL = "http://192.168.31.249";

