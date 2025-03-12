#include <map>
#include <Arduino.h>
#include <WiFi.h>
#include <variant>

#include <secrets.h>
#include <sonarWrapper.h>
#include <personDetection.h>
#include <config.h>
#include <backendApp.h>
#include <timeHelpers.h>

using namespace std;

std::map<String, SunriseAndSunset> sunrisesAndSunsets;

TaskHandle_t WifiTask;
TaskHandle_t HttpTask;
TaskHandle_t NTPTask;
TaskHandle_t SunriseAndSunsetTask;

#define MAX_DISTANCE 400  // In cm

#define INSIDE_TRIG_PIN 12
#define INSIDE_ECHO_PIN 13
#define OUTSIDE_TRIG_PIN 27
#define OUTSIDE_ECHO_PIN 14

bool outsideIsDetected = false;
bool insideIsDetected = false;
bool hasPeopleAmountChanged = false;
bool hasPeopleAmountIncreased = false;
bool hasPeopleAmountDecreased = false;

int numberOfPeople = 0;

enum HttpQueryTypeEnum { SunriseAndSunsetQuery, SwitchLightsQuery };

struct SunriseAndSunsetQueryData {
    const char* date;
};

struct SwitchLightsQueryData {
    int state;
};

struct HttpQueryQueueItem {
    HttpQueryTypeEnum type;

    union {
        SunriseAndSunsetQueryData sunriseAndSunsetData;
        SwitchLightsQueryData switchLightsData;
    };

    HttpQueryQueueItem(HttpQueryTypeEnum queryType) : type(queryType) {}
};

vector<HttpQueryQueueItem> httpQueriesQueue;

SonarWrapper sonarInside(INSIDE_TRIG_PIN, INSIDE_ECHO_PIN, MAX_DISTANCE);
SonarWrapper sonarOutside(OUTSIDE_TRIG_PIN, OUTSIDE_ECHO_PIN, MAX_DISTANCE);
BackendApp backendApp;

PersonDetector personDetector;

void wifiTask(void *pvParameters) {
  while (true) {
    if (isWifiConnected) {
      vTaskDelay(pdMS_TO_TICKS(10000));
    } else {
      vTaskDelay(pdMS_TO_TICKS(1000));
    }
    
    auto wifiStatus = WiFi.status();

    switch (wifiStatus) {
      case WL_CONNECTED:
        isWifiConnected = true;
        Serial.println("Connected to WiFi");
        continue;
      case WL_CONNECT_FAILED:
      case WL_IDLE_STATUS:
        WiFi.disconnect();
        WiFi.reconnect();
        vTaskDelay(pdMS_TO_TICKS(5000));
        break;
      case WL_DISCONNECTED:
      case WL_CONNECTION_LOST:
        WiFi.reconnect();
        vTaskDelay(pdMS_TO_TICKS(5000));
        break;
      default:
        break;
    }

    isWifiConnected = false;
  }
}

void httpTask(void *pvParameters) {
  while (true) {
    vTaskDelay(pdMS_TO_TICKS(100));

    if (
      !hasNTPAlreadyConfigured ||
      httpQueriesQueue.empty() ||
      isHttpQueriesQueueOccupied ||
      !isWifiConnected
    ) {
      continue;
    }

    isHttpQueriesQueueOccupied = true;

    auto httpQueryQueueItem = httpQueriesQueue.front();
    httpQueriesQueue.erase(httpQueriesQueue.begin());

    Serial.println("Processing query: " + String(httpQueryQueueItem.type));

    
    switch (httpQueryQueueItem.type) {
      case SunriseAndSunsetQuery: {
        Serial.println("Processing sunrise and sunset query");
        auto sunriseAndSunset = backendApp.fetchSunriseAndSunset(httpQueryQueueItem.sunriseAndSunsetData.date);
        sunrisesAndSunsets[httpQueryQueueItem.sunriseAndSunsetData.date] = sunriseAndSunset;
        break;
      }
      case SwitchLightsQuery: {
        Serial.println("Processing switch lights query");
        backendApp.switchLights(httpQueryQueueItem.switchLightsData.state);
        break;
      }
      default:
        break;
    }

    isHttpQueriesQueueOccupied = false;
  }
}

void sunriseAndSunsetTask(void *pvParameters) {
  while (true) {
    auto currentDateTime = getCurrentDateTime();
    auto currentDate = extractDate(currentDateTime);

    for (auto it = sunrisesAndSunsets.begin(); it != sunrisesAndSunsets.end();) {
      if (it->first < currentDate) {
        Serial.println("Removing date from queue: " + it->first);
        it = sunrisesAndSunsets.erase(it);
      } else {
        it++;
      }
    }

    todayDate = currentDate;

    // Retrieve for the next week
    auto nextDate = currentDate;
    for (int i = 0; i < 7; i++) {
      if (sunrisesAndSunsets.find(nextDate) == sunrisesAndSunsets.end()) {
        Serial.println("Adding date to queue: " + nextDate);

        HttpQueryQueueItem query = HttpQueryQueueItem(SunriseAndSunsetQuery);
        query.sunriseAndSunsetData.date = nextDate.c_str();

        httpQueriesQueue.push_back(query);
      }

      nextDate = addDayToDate(nextDate);
    }

    vTaskDelay(pdMS_TO_TICKS(1000 * 60 * 60)); // Once per hour
  }
}

void ntpTask(void *param) {
    while (true) {
        vTaskDelay(1000 / portTICK_PERIOD_MS); // Once per second

        if (!isWifiConnected || hasNTPAlreadyConfigured) {
            continue;
        }

        if (!isNTPUnderConfiguration) {
            Serial.println("Starting NTP configuration");
            isNTPUnderConfiguration = true;
            configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER_URL); // Synchronize time
        }

        Serial.println("Trying to get current time");
        if (getCurrentDateTime() == "") {
            continue;
        } else {
            Serial.println("Current time obtained");
            hasNTPAlreadyConfigured = true; // It happens only once

            xTaskCreate(sunriseAndSunsetTask, "SunriseAndSunsetTask", 2048, NULL, 1, &SunriseAndSunsetTask);
            xTaskCreate(httpTask, "HttpTask", 8192, NULL, 1, &HttpTask);
            vTaskDelete(NTPTask);
        }
    }
}

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreate(wifiTask, "WifiTask", 4096, NULL, 1, &WifiTask);
  xTaskCreate(ntpTask, "NTPTask", 2048, NULL, 1, &NTPTask);

  delay(10000); // 10s delay to get out of the room while initializing the system
}

void loop() {
  delay(50);

  if (!isWifiConnected || !hasNTPAlreadyConfigured || todayDate == "") {
    return;
  }

  insideIsDetected = sonarInside.checkIsDetected();
  outsideIsDetected = sonarOutside.checkIsDetected();

  int result = personDetector.update(insideIsDetected, outsideIsDetected);

  if (result == 1) {
    numberOfPeople += 1;
    hasPeopleAmountChanged = true;
    hasPeopleAmountIncreased = true;
    hasPeopleAmountDecreased = false;
    Serial.println("New person entered: " + String(numberOfPeople));
  } else if (result == -1) {
    if (numberOfPeople < 1) {
      Serial.println("Number of people is less than 1, error - ignoring");
    } else {
      numberOfPeople -= 1;
      hasPeopleAmountChanged = true;
      hasPeopleAmountIncreased = false;
      hasPeopleAmountDecreased = true;
    }

    Serial.println("Person left: " + String(numberOfPeople));
  } else {
    hasPeopleAmountChanged = false;
    hasPeopleAmountIncreased = false;
    hasPeopleAmountDecreased = false;
  }

  if (hasPeopleAmountChanged && (numberOfPeople == 0 || numberOfPeople == 1)) {
    if (numberOfPeople == 1 && hasPeopleAmountIncreased) {
      auto currentTime = extractTime(getCurrentDateTime());

      if (currentTime < sunrisesAndSunsets[todayDate].sunrise || currentTime > sunrisesAndSunsets[todayDate].sunset) {
        HttpQueryQueueItem query = HttpQueryQueueItem(SwitchLightsQuery);
        query.switchLightsData.state = 1;
        httpQueriesQueue.push_back(query);
      }

      return;
    }
    
    if (numberOfPeople == 0 && hasPeopleAmountDecreased) {
      HttpQueryQueueItem query = HttpQueryQueueItem(SwitchLightsQuery);
      query.switchLightsData.state = 0;
      httpQueriesQueue.push_back(query);

      return;
    }
  }

  
}
