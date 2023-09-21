/*
Copyright (c) 2017-2022, Michael Romans of Romans Audio
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

The views and conclusions contained in the software and documentation are those
of the authors and should not be interpreted as representing official policies,
either expressed or implied, of the MRUtils project.
*/

/*
 * This is a very basic method for getting current conditions from Weather Underground
 * for your Personal Weather Station.  It requires your API Key and your Station ID.
 */

#ifndef WEATHER_EZPERSONALWEATHERSTATION_H
#define WEATHER_EZPERSONALWEATHERSTATION_H

#pragma once

#include "EZTools.h"
#include "EZAppFull.h"
#include "nlohmann/json.hpp"

namespace EZPersonalWeatherStation {

    EZTools::EZString directionString(EZTools::EZString dir) {
        std::map<int, EZTools::EZString> dirs;
        dirs.insert(std::pair<int, EZTools::EZString>(1,"N"));
        dirs.insert(std::pair<int, EZTools::EZString>(2,"NNE"));
        dirs.insert(std::pair<int, EZTools::EZString>(3,"NE"));
        dirs.insert(std::pair<int, EZTools::EZString>(4,"ENE"));
        dirs.insert(std::pair<int, EZTools::EZString>(5,"E"));
        dirs.insert(std::pair<int, EZTools::EZString>(6,"ESE"));
        dirs.insert(std::pair<int, EZTools::EZString>(7,"SE"));
        dirs.insert(std::pair<int, EZTools::EZString>(8,"SSE"));
        dirs.insert(std::pair<int, EZTools::EZString>(9,"S"));
        dirs.insert(std::pair<int, EZTools::EZString>(10,"SSW"));
        dirs.insert(std::pair<int, EZTools::EZString>(11,"SW"));
        dirs.insert(std::pair<int, EZTools::EZString>(12,"WSW"));
        dirs.insert(std::pair<int, EZTools::EZString>(13,"W"));
        dirs.insert(std::pair<int, EZTools::EZString>(14,"WNW"));
        dirs.insert(std::pair<int, EZTools::EZString>(15,"NW"));
        dirs.insert(std::pair<int, EZTools::EZString>(16,"NNW"));
        dirs.insert(std::pair<int, EZTools::EZString>(17,"N"));
        int mapVal = ( (dir.asInt() - 0) / (360 - 0) ) * (17 - 1) + 1;
        return dirs.find(mapVal)->second;
    }

    struct currentConditions {
        float humidity = 0;
        float dewpoint = 0;
        float heatIndex = 0;
        float precipRate = 0.0;
        float precipTotal = 0.0;
        float pressure = 0.0;
        float temp = 0;
        float windChill = 0;
        float windGust = 0;
        float windSpeed = 0;
        int windDir = 0;
        EZTools::EZString obsTimeLocal;
    };

    class CurrentConditions {
    public:
        CurrentConditions(EZAppFull::App &app, EZTools::EZString APIKey, EZTools::EZString StationID) {
            _APIKEY = APIKey;
            _STATIONID = StationID;
            _currentCondistionsString.regexReplace("\\<APIKEY\\>", _APIKEY);
            _currentCondistionsString.regexReplace("\\<STATIONID\\>", _STATIONID);
            EZHTTP::URI uri(_currentCondistionsString);
            EZHTTP::Headers header;
            header.addPair("Accept-Encoding", "gzip");
            header.addPair("User-Agent", "C++ IBM Weather Data Client by Romans Audio");
            uri.requestHeaders(header.list());
            app.http.init(uri);
            auto res = app.http.get();
            _wasSuccessful = res.wasSuccessful();
            _httpCode = res.httpCode();
            _message = res.message();
            _data = res.data;
            if (res.httpCode() == 200) {
                _root = nlohmann::json::parse(_data);
                if (_root["observations"].is_array()) {
                    nlohmann::json c = _root["observations"].at(0);
                    if (c["imperial"]["dewpt"].is_number()) {
                        current.dewpoint = c["imperial"]["dewpt"].get<float>();
                    }
                    if (c["humidity"].is_number()) {
                        current.humidity = c["humidity"].get<float>();
                    }
                    if (c["imperial"]["heatIndex"].is_number()) {
                        current.heatIndex = c["imperial"]["heatIndex"].get<float>();
                    }
                    if (c["imperial"]["precipRate"].is_number()) {
                        current.precipRate = c["imperial"]["precipRate"].get<float>();
                    }
                    if (c["imperial"]["precipTotal"].is_number()) {
                        current.precipTotal = c["imperial"]["precipTotal"].get<float>();
                    }
                    if (c["imperial"]["pressure"].is_number()) {
                        current.pressure = c["imperial"]["pressure"].get<float>();
                    }
                    if (c["imperial"]["temp"].is_number()) {
                        current.temp = c["imperial"]["temp"].get<float>();
                    }
                    if (c["imperial"]["windChill"].is_number()) {
                        current.windChill = c["imperial"]["windChill"].get<float>();
                    }
                    if (c["imperial"]["windGust"].is_number()) {
                        current.windGust = c["imperial"]["windGust"].get<float>();
                    }
                    if (c["imperial"]["windSpeed"].is_number()) {
                        current.windSpeed = c["imperial"]["windSpeed"].get<float>();
                    }
                    if (c["obsTimeLocal"].is_string()) {
                        current.obsTimeLocal = c["obsTimeLocal"].get<EZTools::EZString>();
                    }
                    if (c["winddir"].is_number_integer()) {
                        current.windDir = c["winddir"].get<int>();
                    }
                }
            }
        }
        ~CurrentConditions() = default;
        currentConditions current;
        nlohmann::json json() { return _root; }
        bool wasSuccessful() const { return _wasSuccessful; }
        EZTools::EZString message() { return _message; }
        long httpCode() const { return _httpCode; }
        EZTools::EZString httpCall() { return _currentCondistionsString; }
    private:
        nlohmann::json _root;
        bool _wasSuccessful = false;
        long _httpCode;
        EZTools::EZString _message;
        EZTools::EZString _data;
        EZTools::EZString _APIKEY;
        EZTools::EZString _STATIONID;
        EZTools::EZString _currentCondistionsString = "https://api.weather.com/v2/pws/observations/current?stationId=<STATIONID>&numericPrecision=decimal&format=json&units=e&apiKey=<APIKEY>";
    };

    class WeatherConfig {
    public:
        WeatherConfig(EZFiles::URI weatherConfigURI) {
            EZFiles::TextFile configFile(&weatherConfigURI, EZFiles::READ);
            nlohmann::json cf = nlohmann::json::parse(configFile.contents());
            _APIKEY = cf["APIKey"].get<std::string>();
            _STATIONID = cf["StationID"].get<std::string>();
            _title = cf["title"].get<std::string>();
        }
        ~WeatherConfig() = default;
        EZTools::EZString apiKey() { return _APIKEY; }
        EZTools::EZString stationID() { return _STATIONID; }
        EZTools::EZString title() { return _title; }
    private:
        EZTools::EZString _APIKEY;
        EZTools::EZString _STATIONID;
        EZTools::EZString _title;
    };

}

#endif //WEATHER_EZPERSONALWEATHERSTATION_H
