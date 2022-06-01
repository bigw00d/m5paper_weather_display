#pragma once

#include <string>
#include <ArduinoJson.h>

enum {
  WEATHER_NOT_SET = 0,
  CLOUDY,
  SUNNY,
  RAINY,
  SNOW,
  SUNNY_AND_CLOUDY,
  RAINY_AND_CLOUDY,
};

class WeatherForecast
{
private:
    String endpoint;
    String region;
    bool is_downloaded_weather;
    String max_temperature;
    String min_temperature;
    String weather;
    String rain_fall_chance_00_06;
    String rain_fall_chance_06_12;
    String rain_fall_chance_12_18;
    String rain_fall_chance_18_24;
    String rain_fall_timearea_0;
    String rain_fall_timearea_1;
    String rain_fall_timearea_2;
    String rain_fall_timearea_3;

    String createJson(String json_string);
    bool getWeatherForecast(DynamicJsonDocument &doc);
    String createTimeAreaString(String timearea_string);

public:
    WeatherForecast();
    ~WeatherForecast() = default; 
    bool downloadWeatherForecast(void);
    bool isDownloadedWeatherForecast(void){ return is_downloaded_weather; };
    String getWeather(void){ return weather; };
    int getWeatherEnum(void);
    String getMaxTemperature(void){ return max_temperature; };
    String getMinTemperature(void){ return min_temperature; };
    String getRainFallChance00_06(void){ return rain_fall_chance_00_06; };
    String getRainFallChance06_12(void){ return rain_fall_chance_06_12; };
    String getRainFallChance12_18(void){ return rain_fall_chance_12_18; };
    String getRainFallChance18_24(void){ return rain_fall_chance_18_24; };
    String getRainFallTimeArea0(void){ return rain_fall_timearea_0; };
    String getRainFallTimeArea1(void){ return rain_fall_timearea_1; };
    String getRainFallTimeArea2(void){ return rain_fall_timearea_2; };
    String getRainFallTimeArea3(void){ return rain_fall_timearea_3; };
    bool willBeRainy(void);
};
