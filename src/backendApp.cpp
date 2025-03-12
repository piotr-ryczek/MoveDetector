#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <backendApp.h>
#include <secrets.h>
#include <config.h>

BackendApp::BackendApp() {}

void BackendApp::addHeaders(HTTPClient* httpClient) {
  httpClient->addHeader("Content-Type", "application/json");
  httpClient->addHeader("App-Secret", BACKEND_APP_SECRET);
}

void BackendApp::setClientProperties(HTTPClient* httpClient) {
  httpClient->setReuse(false);
  httpClient->setTimeout(10000);
}

SunriseAndSunset BackendApp::fetchSunriseAndSunset(String date) {
  Serial.println("Trying to query SunriseAndSunset");

  WiFiClient wifiClient;

  wifiClient.flush();
  wifiClient.stop();

  HTTPClient httpClient;

  this->setClientProperties(&httpClient);

  try {
    if (!httpClient.begin(wifiClient, String(BACKEND_APP_URL) + "/sunrise-sunset?date=" + date)) {
      Serial.println("Failed to connect to BackendApp");

      httpClient.end();
      return SunriseAndSunset{};
    }

    this->addHeaders(&httpClient);

    int httpCode = httpClient.GET();

    if (httpCode != 200) {
      httpClient.end();

      Serial.println("Sunrise/Sunset Request didn't respond with 200");
      return SunriseAndSunset{};
    }


    String response = httpClient.getString();

    Serial.println("Sunrise/Sunset: Deserialize JSON");

    JsonDocument doc;
    deserializeJson(doc, response);

    SunriseAndSunset result;
    String sunrise = doc["sunrise"];
    String sunset = doc["sunset"];

    result.sunrise = sunrise;
    result.sunset = sunset;
    httpClient.end();

    Serial.println("Sunrise/Sunset: Query finished");

    return result;
  } catch (const std::exception& e) {
    httpClient.end();

    Serial.println("Error during Sunrise/Sunset query");
    Serial.println(e.what());

    return SunriseAndSunset{};
  }

  httpClient.end();

  wifiClient.flush();
  wifiClient.stop();
}

void BackendApp::switchLights(int state) {
  Serial.println("Switching lights to " + String(state));

  WiFiClient wifiClient;

  wifiClient.flush();
  wifiClient.stop();

  HTTPClient httpClient;

  this->setClientProperties(&httpClient);

  try {
    if (!httpClient.begin(wifiClient, String(WLED_URL) + "/win&T=" + String(state))) {
      Serial.println("Failed to connect to WLED");

      httpClient.end();
      return;
    }

    int httpCode = httpClient.GET();

    if (httpCode != 200) {
      httpClient.end();

      Serial.println("Switching lights Request didn't respond with 200");
      return;
    }

    String response = httpClient.getString();

    httpClient.end();
  } catch (const std::exception& e) {
    Serial.println("Error during switching lights");
    Serial.println(e.what());

    httpClient.end();
  }

  wifiClient.flush();
  wifiClient.stop();
}