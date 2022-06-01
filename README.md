m5paper_weather_display
====

## Description

* Show the weather & condition(temperature, humidity)
* Use [LovyanGFX](https://github.com/lovyan03/LovyanGFX) to draw the LCD

## Requirement

- [M5Paper](https://www.switch-science.com/catalog/6749/)
- Arduino IDE (Version: 1.8.15)
- Arduino Json (Version: 6.18.2)
- LovyanGFX (Version: 0.4.1)
- M5EPD (Version: 0.1.1)

## Board settings
- ボード M5Stack-Paper
- Upload Speed  115200
- CPU Frequency 240MHz
- Partition Scheme  Default
- Core Debug Level なし
- PSRAM: Disabled

## Usage

1. Write the image files to the SD card & insert the SD card  
    *  cloudy.jpeg  
    *  humidity.jpg  
    *  rainly.jpeg  
    *  rainyandcloudy.jpeg  
    *  snow.jpeg  
    *  sunny.jpeg  
    *  sunnyandcloudy.jpeg  
    *  thermometer.jpg  

2. Modify the area configuration in `weather_forecast.cpp`(example below, use JMA)  

```weather_forecast.cpp
#include "weather_forecast.hpp"

WeatherForecast::WeatherForecast(void)
{
    // this->endpoint = "https://www.drk7.jp/weather/json/14.js";
    // this->region = "東部";
    this->endpoint = "https://www.jma.go.jp/bosai/forecast/data/forecast/(pathCode).json";
    this->region = "(areaName)";
}

```

3. Modify the Wi-Fi configuration in `config.h`  

```config.h
#pragma once

const char* SSID = "(Your NW SSID)";
const char* PASS = "(Your NW Password)";

```

4. Build the project & upload the application to your board 

## Licence

   This program is licensed under Apache-2.0.

## Changelog

### [0.0.1] - 2022-06-01
#### Added
- This README file 
