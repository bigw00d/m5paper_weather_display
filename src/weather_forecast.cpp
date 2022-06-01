#include <WiFi.h>
#include <HTTPClient.h>
#include "weather_forecast.hpp"
#include "../debug.h"

WeatherForecast::WeatherForecast(void)
{
    this->endpoint = "https://www.drk7.jp/weather/json/14.js";
    this->region = "東部";
}

String WeatherForecast::createJson(String json_string)
{
    // json_string.replace("drk7jpweather.callback(","");
    // return json_string.substring(0, json_string.length() - 2);
    return json_string;
}

bool WeatherForecast::getWeatherForecast(DynamicJsonDocument &doc)
{
    DEBUG_SERIAL.println("getWeatherForecast");//debug
    bool ret = true;
    if((WiFi.status() == WL_CONNECTED)){
        HTTPClient http;
        http.begin(this->endpoint.c_str());
        int http_code = http.GET();
        if (http_code > 0) {
            String json_string = createJson(http.getString());
            // DEBUG_SERIAL.println("get: " + json_string);//debug
            deserializeJson(doc, json_string);
        } else {
            DEBUG_SERIAL.println("Error on HTTP request");
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
    DEBUG_SERIAL.println("downloadWeatherForecast");//debug
    if(!WiFi.isConnected()){
        DEBUG_SERIAL.println("WiFi error");//debug
        this->is_downloaded_weather = false;
        return false;
    }

    DynamicJsonDocument weather_info(20000);

    if(!getWeatherForecast(weather_info)){
        DEBUG_SERIAL.println("getWeatherForecast error");//debug
        this->is_downloaded_weather = false;
        return false;
    }

    // String test = weather_info[0]["reportDatetime"];
    // DEBUG_SERIAL.println("test: " + test);//debug

    String weather_series = weather_info[0]["timeSeries"][0]["areas"][0]; // ["areas"][0] = "南部"
    DynamicJsonDocument weather_series_info(1000);
    deserializeJson(weather_series_info, weather_series);    

    String temp_series = weather_info[1]["timeSeries"][1]["areas"][0];
    DynamicJsonDocument temp_series_info(1000);
    deserializeJson(temp_series_info, temp_series);    

    String pops_series = weather_info[0]["timeSeries"][1];
    DynamicJsonDocument pops_series_info(1000);
    deserializeJson(pops_series_info, pops_series);    

    // this->weather = ("雨"); // dummy
    String w = weather_series_info["weatherCodes"][0];
    this->weather = w;
    // DEBUG_SERIAL.println("weather: " + w);//debug

    String max_temp = temp_series_info["tempsMax"][1];
    this->max_temperature = max_temp;

    String min_temp = temp_series_info["tempsMin"][1];
    this->min_temperature = min_temp;
    // DEBUG_SERIAL.println("max_temp: " + max_temp);//debug
    // DEBUG_SERIAL.println("min_temp: " + min_temp);//debug

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

    DEBUG_SERIAL.println("rain_timearea_0: " + this->rain_fall_timearea_0);//debug
    DEBUG_SERIAL.println("rain_timearea_1: " + this->rain_fall_timearea_1);//debug
    DEBUG_SERIAL.println("rain_timearea_2: " + this->rain_fall_timearea_2);//debug
    DEBUG_SERIAL.println("rain_timearea_3: " + this->rain_fall_timearea_3);//debug

    // String today_weather = weather_info["pref"]["area"][this->region.c_str()]["info"][0];

    // DynamicJsonDocument today_weather_info(20000);
    // deserializeJson(today_weather_info, today_weather);    

    // String w = today_weather_info["weather"];
    // this->weather = w;

    // String max_temp = today_weather_info["temperature"]["range"][0]["content"];
    // this->max_temperature = max_temp;

    // String min_temp = today_weather_info["temperature"]["range"][1]["content"];
    // this->min_temperature = min_temp;

    // String rain_0 = today_weather_info["rainfallchance"]["period"][0]["content"];
    // this->rain_fall_chance_00_06 = rain_0;

    // String rain_1 = today_weather_info["rainfallchance"]["period"][1]["content"];
    // this->rain_fall_chance_06_12 = rain_1;

    // String rain_2 = today_weather_info["rainfallchance"]["period"][2]["content"];
    // this->rain_fall_chance_12_18 = rain_2;

    // String rain_3 = today_weather_info["rainfallchance"]["period"][3]["content"];
    // this->rain_fall_chance_18_24 = rain_3;

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

    // if(weather.indexOf("雨") != -1){
    //     if(weather.indexOf("くもり") != -1){
    //         return RAINY_AND_CLOUDY;
    //     }else{
    //         return RAINY;
    //     }
    // }else if(weather.indexOf("晴") != -1){
    //     if(weather.indexOf("くもり") != -1){
    //         return SUNNY_AND_CLOUDY;
    //     }else{
    //         return SUNNY;
    //     }
    // }else if(weather.indexOf("雪") != -1){
    //     return SNOW;
    // }else if(weather.indexOf("くもり") != -1){
    //     return CLOUDY;
    // }
    // return WEATHER_NOT_SET;
}

bool WeatherForecast::willBeRainy(void)
{
    if(weather.indexOf("雨") != -1){
        return true;
    }
    return false;
}
 