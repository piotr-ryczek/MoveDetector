#include <timeHelpers.h>

String getCurrentDateTime() {
    char buffer[128];
    struct tm currentTime;

    if (!getLocalTime(&currentTime)) {
        Serial.println("Failed to obtain current time");
        return "";
    }

    if (currentTime.tm_year < 70) { 
        Serial.println("Invalid time data");
        return "";
    }

    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &currentTime);

    return String(buffer);
}

String extractDate(String dateTime) {
    return dateTime.substring(0, 10);
}

String extractTime(String dateTime) {
    return dateTime.substring(11, 19);
}

String addDayToDate(String date) {
    struct tm timeStruct = {};
    sscanf(date.c_str(), "%d-%d-%d", &timeStruct.tm_year, &timeStruct.tm_mon, &timeStruct.tm_mday);
    
    timeStruct.tm_year -= 1900;
    timeStruct.tm_mon -= 1;
    timeStruct.tm_mday += 1;
    
    time_t timeVal = mktime(&timeStruct);
    localtime_r(&timeVal, &timeStruct);
    
    char buffer[11];
    snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d", timeStruct.tm_year + 1900, timeStruct.tm_mon + 1, timeStruct.tm_mday);
    
    return String(buffer);
}