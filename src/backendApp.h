#ifndef BACKEND_APP_H
#define BACKEND_APP_H

#include <vector>
#include <HTTPClient.h>


using namespace std;

struct SunriseAndSunset {
  String sunrise;
  String sunset;
};

class BackendApp {
  private:
      void addHeaders(HTTPClient* httpClient);
      void setClientProperties(HTTPClient* httpClient);
  public:
      BackendApp();

      SunriseAndSunset fetchSunriseAndSunset(String date);
      void switchLights(int state);
};



#endif