#include <WiFi.h>
#include <HTTPClient.h>
#include "weather_forecast.hpp"

WeatherForecast::WeatherForecast(void)
{
    this->endpoint = "https://www.jma.go.jp/bosai/forecast/data/forecast/330000.json";
    this->region = "南部";
}

String WeatherForecast::createJson(String json_string)
{
    return json_string;
}

bool WeatherForecast::getWeatherForecast(DynamicJsonDocument &doc)
{
    Serial.println("getWeatherForecast");//debug
    bool ret = true;
    if((WiFi.status() == WL_CONNECTED)){
        HTTPClient http;
        http.begin(this->endpoint.c_str());
        int http_code = http.GET();
        if (http_code > 0) {
            String json_string = createJson(http.getString());
            deserializeJson(doc, json_string);
        } else {
            Serial.println("Error on HTTP request");
            ret = false;
        }
        http.end();
    }
    return ret;
}

String WeatherForecast::createTimeAreaString(String timearea_string)
{
    return timearea_string.substring(11, timearea_string.length() - 12);
}

bool WeatherForecast::downloadWeatherForecast(void)
{
    Serial.println("downloadWeatherForecast");//debug
    if(!WiFi.isConnected()){
        Serial.println("WiFi error");//debug
        this->is_downloaded_weather = false;
        return false;
    }

    DynamicJsonDocument weather_info(20000);

    if(!getWeatherForecast(weather_info)){
        Serial.println("getWeatherForecast error");//debug
        this->is_downloaded_weather = false;
        return false;
    }


    String weather_series = weather_info[0]["timeSeries"][0]["areas"][0]; // ["areas"][0] = "南部"
    DynamicJsonDocument weather_series_info(1000);
    deserializeJson(weather_series_info, weather_series);    

    String temp_series = weather_info[1]["timeSeries"][1]["areas"][0];
    DynamicJsonDocument temp_series_info(1000);
    deserializeJson(temp_series_info, temp_series);    

    String pops_series = weather_info[0]["timeSeries"][1];
    DynamicJsonDocument pops_series_info(1000);
    deserializeJson(pops_series_info, pops_series);    

    String w = weather_series_info["weatherCodes"][0];
    this->weather = w;
    // Serial.println("weather: " + w);//debug

    String max_temp = temp_series_info["tempsMax"][1];
    this->max_temperature = max_temp;

    String min_temp = temp_series_info["tempsMin"][1];
    this->min_temperature = min_temp;
    // Serial.println("max_temp: " + max_temp);//debug
    // Serial.println("min_temp: " + min_temp);//debug

    String rain_0 = pops_series_info["areas"][0]["pops"][0];
    String rain_1 = pops_series_info["areas"][0]["pops"][1];
    String rain_2 = pops_series_info["areas"][0]["pops"][2];
    String rain_3 = pops_series_info["areas"][0]["pops"][3];
    this->rain_fall_chance_00_06 = rain_0;
    this->rain_fall_chance_06_12 = rain_1;
    this->rain_fall_chance_12_18 = rain_2;
    this->rain_fall_chance_18_24 = rain_3;

    String rain_timearea_0 = pops_series_info["timeDefines"][0];
    rain_timearea_0 = createTimeAreaString(rain_timearea_0);
    String rain_timearea_1 = pops_series_info["timeDefines"][1];
    rain_timearea_1 = createTimeAreaString(rain_timearea_1);
    String rain_timearea_2 = pops_series_info["timeDefines"][2];
    rain_timearea_2 = createTimeAreaString(rain_timearea_2);
    String rain_timearea_3 = pops_series_info["timeDefines"][3];
    rain_timearea_3 = createTimeAreaString(rain_timearea_3);

    this->rain_fall_timearea_0 = rain_timearea_0 + "-" + rain_timearea_1;
    this->rain_fall_timearea_1 = rain_timearea_1 + "-" + rain_timearea_2;
    this->rain_fall_timearea_2 = rain_timearea_2 + "-" + rain_timearea_3;
    this->rain_fall_timearea_3 = rain_timearea_3 + "-" + rain_timearea_0;

    Serial.println("rain_timearea_0: " + this->rain_fall_timearea_0);//debug
    Serial.println("rain_timearea_1: " + this->rain_fall_timearea_1);//debug
    Serial.println("rain_timearea_2: " + this->rain_fall_timearea_2);//debug
    Serial.println("rain_timearea_3: " + this->rain_fall_timearea_3);//debug

    this->is_downloaded_weather = true;

    return true;
}

int WeatherForecast::getWeatherEnum(void)
{
    int ret = WEATHER_NOT_SET;
    // see http://weather.yukigesho.com/code.html
    String w = getWeather();

    int weatherCode = w.toInt();
    
    if((weatherCode / 100) == 3){
        if(weatherCode >= 340){
            ret = SNOW;
        }else{
            ret = RAINY;
        }
    }else if((weatherCode / 100) == 1){
        if(weatherCode >= 102){
            ret = SUNNY_AND_CLOUDY;
        }else{
            ret = SUNNY;
        }
    }else if((weatherCode / 100) == 4){
        ret = SNOW;
    }else if((weatherCode / 100) == 2){
        ret = CLOUDY;
    }
    return ret;
}

bool WeatherForecast::willBeRainy(void)
{
    if(weather.indexOf("雨") != -1){
        return true;
    }
    return false;
}
 