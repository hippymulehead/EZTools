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

#ifndef EZ_EZWEATHER_H
#define EZ_EZWEATHER_H

#include <pthread.h>
#include "EZTools.h"
#include "EZFiles.h"
#include "nlohmann/json.hpp"
#include "EZMySQL.h"
#include "EZHTTP.h"
#include "EZDateTime.h"

namespace EZWeather {

    namespace WeatherUnderground {

        EZTools::EZString radarURL(EZTools::EZString state) {
            EZTools::EZString base = "https://s.w-x.co/staticmaps/wu/wu/wxtype1200_cur/<REPLACEME>/animate.png";
            if (state == "AK") {return base.regexReplace("<REPLACEME>", "alask");} else
            if (state == "AR") {return base.regexReplace("<REPLACEME>", "uslit");} else
            if (state == "AZ") {return base.regexReplace("<REPLACEME>", "usprc");} else
            if (state == "CA") {return base.regexReplace("<REPLACEME>", "usbfl");} else
            if (state == "CO") {return base.regexReplace("<REPLACEME>", "usden");} else
            if (state == "CT") {return base.regexReplace("<REPLACEME>", "ushfd");} else
            if (state == "DC" || state == "VA") {return base.regexReplace("<REPLACEME>", "usfcx");} else
            if (state == "FL") {return base.regexReplace("<REPLACEME>", "uspie");} else
            if (state == "GA") {return base.regexReplace("<REPLACEME>", "uscsg");} else
            if (state == "HI") {return base.regexReplace("<REPLACEME>", "hawai");} else
            if (state == "IA") {return base.regexReplace("<REPLACEME>", "usdsm");} else
            if (state == "ID") {return base.regexReplace("<REPLACEME>", "usmyl");} else
            if (state == "IL") {return base.regexReplace("<REPLACEME>", "usspi");} else
            if (state == "KS") {return base.regexReplace("<REPLACEME>", "ussln");} else
            if (state == "KY") {return base.regexReplace("<REPLACEME>", "usbwg");} else
            if (state == "LA") {return base.regexReplace("<REPLACEME>", "usmsy");} else
            if (state == "ME" || state == "NH") {return base.regexReplace("<REPLACEME>", "usbml");} else
            if (state == "MI") {return base.regexReplace("<REPLACEME>", "uscad");} else
            if (state == "MN") {return base.regexReplace("<REPLACEME>", "usstc");} else
            if (state == "MO") {return base.regexReplace("<REPLACEME>", "usjef");} else
            if (state == "MS") {return base.regexReplace("<REPLACEME>", "ustvr");} else
            if (state == "MT") {return base.regexReplace("<REPLACEME>", "uslwt");} else
            if (state == "NC") {return base.regexReplace("<REPLACEME>", "usclt");} else
            if (state == "ND") {return base.regexReplace("<REPLACEME>", "usbis");} else
            if (state == "NE") {return base.regexReplace("<REPLACEME>", "uslbf");} else
//            if (state == "NH") {return base.regexReplace("<REPLACEME>", "usbml");} else
            if (state == "NM") {return base.regexReplace("<REPLACEME>", "usrow");} else
            if (state == "NV") {return base.regexReplace("<REPLACEME>", "usrno");} else
            if (state == "NY") {return base.regexReplace("<REPLACEME>", "usbgm");} else
            if (state == "OH") {return base.regexReplace("<REPLACEME>", "usday");} else
            if (state == "OK") {return base.regexReplace("<REPLACEME>", "uslaw");} else
            if (state == "OR") {return base.regexReplace("<REPLACEME>", "usrdm");} else
            if (state == "PR") {return base.regexReplace("<REPLACEME>", "prico");} else
            if (state == "SD") {return base.regexReplace("<REPLACEME>", "uspir");} else
            if (state == "TX") {return base.regexReplace("<REPLACEME>", "ussat");} else
            if (state == "UT") {return base.regexReplace("<REPLACEME>", "uspvu");} else
//            if (state == "VA") {return base.regexReplace("<REPLACEME>", "usfcx");} else
            if (state == "WA") {return base.regexReplace("<REPLACEME>", "ustiw");} else
            if (state == "WY") {return base.regexReplace("<REPLACEME>", "usriw");} else
                return {};
        }
    }

    namespace Utils {

        inline EZTools::EZString calcHeatIndex(double temp, double humidity) {
            if (temp >= 80) {
                double hi = 0;
                if (humidity < 13 && 80 < temp < 112) {
                    hi=((13 - humidity) / 4) * sqrt((17 - abs(temp-95.0)) / 17);
                }
                else if (humidity > 85 && 80 < temp < 87) {
                    hi = ((humidity - 85) / 10) * ((87 - temp) / 5);
                } else {
                    hi= 0.5 * (temp +61.0 + ((temp - 68.0) * 1.2) + (humidity * 0.094));
                }
                return {hi};
            } else {
                return temp;
            }
        }

        inline EZTools::EZString calcWindChill(double temp, double windSpeed) {
            if (temp <= 40) {
                double x = pow(windSpeed, 0.16);
                double WindChill = 35.74 + 0.6215 * temp - 35.75 * x + 0.4275 * x * temp;
                return {WindChill};
            } else return {};
        }

        inline EZTools::EZString directionAsString(EZTools::EZString dir) {
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

        inline EZTools::EZString Round(float var) {
            std::stringstream ss;
            ss << std::setprecision(2) << std::fixed << var;
            return ss.str();
        }

        inline float Roundf(float var) {
            std::stringstream ss;
            ss << std::setprecision(2) << std::fixed << var;
            EZTools::EZString f(ss.str());
            return f.asFloat();
        }

        inline EZTools::EZString metersToInches(float &meters) {
            auto meh = Roundf(meters * 39.37);
            return EZTools::EZString(meh);
        }

        inline float metersToInchesf(float &meters) {
            return meters * 39.37;
        }

        inline EZTools::EZString celsiusToFahrenheit(float &celTemp) {
            return Round((celTemp * 1.8) + 32);
        }

        inline float celsiusToFahrenheitf(float &celTemp) {
            return Roundf((celTemp * 1.8) + 32);
        }

        inline EZTools::EZString kilometersPerHourToMilesPerHour(float &k) {
            return Round(k / 1.609);
        }

        inline float kilometersPerHourToMilesPerHourf(float &k) {
            return Roundf(k / 1.609);
        }

        inline EZTools::EZString pascalsToMillibars(float &pa) {
            return Round(pa * 0.00029530);
        }

        inline float pascalsToMillibarsf(float &pa) {
            return Roundf(pa * 0.00029530);
        }

        inline float metersToMilesf(float &meters) {
            return meters * 0.00062137;
        }

        inline EZTools::EZString metersToMiles(float &meters) {
            return Round(meters * 0.00062137);
        }

    }

    namespace EZLocationData {

        // Returns a json array of only the reporting stations with a single zipcode.
        inline nlohmann::json locationSearchByCityState(EZMySQL::MySQLConnetionInfo &connInfo,
                                                        EZTools::EZString city, EZTools::EZString state) {
            EZMySQL::Database db;
            db.connect(connInfo.server(), connInfo.database(), connInfo.user(),
                       connInfo.password());
            std::stringstream ss;
            ss << "select city,state,zipcode,name from zipdata where city = '" << city << "' and state = '"
               << state << "'  group by name order by zipcode";
            auto q = db.query(ss.str());
            nlohmann::json root = nlohmann::json::array();
            if (!q.data.empty()) {
                for (size_t x = 0; x < q.data.num_rows(); x++) {
                    nlohmann::json o;
                    o["city"] = q.data[x]["city"].data();
                    o["state"] = q.data[x]["state"].data();
                    o["zipcode"] = q.data[x]["zipcode"].data();
                    o["name"] = q.data[x]["name"].data();
                    root.push_back(o);
                }
            }
            return root;
        }

        // Returns a json array of only the reporting stations with a single zipcode.
        inline nlohmann::json locationSearchByCity(EZMySQL::MySQLConnetionInfo &connInfo,
                                                   EZTools::EZString city) {
            EZMySQL::Database db;
            db.connect(connInfo.server(), connInfo.database(), connInfo.user(),
                       connInfo.password());
            std::stringstream ss;
            ss << "select city,state,zipcode,name from zipdata where city = '" << city << "' group by name order by zipcode";
            auto q = db.query(ss.str());
            nlohmann::json root = nlohmann::json::array();
            if (!q.data.empty()) {
                for (size_t x = 0; x < q.data.num_rows(); x++) {
                    nlohmann::json o;
                    o["city"] = q.data[x]["city"].data();
                    o["state"] = q.data[x]["state"].data();
                    o["zipcode"] = q.data[x]["zipcode"].data();
                    o["name"] = q.data[x]["name"].data();
                    root.push_back(o);
                }
            }
            return root;
        }

        // Returns a json array of only the reporting stations with a single zipcode.
        inline nlohmann::json locationSearchByZipcode(EZMySQL::MySQLConnetionInfo &connInfo,
                                                      EZTools::EZString zipcode) {
            EZMySQL::Database db;
            db.connect(connInfo.server(), connInfo.database(), connInfo.user(),
                       connInfo.password());
            std::stringstream ss;
            ss << "select * from zipdata where zipcode = '" << zipcode << "' group by name order by zipcode";
            auto res = db.query(ss.str());
            nlohmann::json base;
            if (!res.data.empty()) {
                for (size_t x = 0; x < res.data.num_rows(); x++) {
                    base["state"] = res.data[x]["state"].data();
                    base["city"] = res.data[x]["city"].data();
                    base["county"] = res.data[x]["county"].data();
                    base["longitude"] = res.data[x]["longitude"].data();
                    base["latitude"] = res.data[x]["latitude"].data();
                    base["gridId"] = res.data[x]["gridId"].data();
                    base["gridX"] = res.data[x]["gridX"].data();
                    base["gridY"] = res.data[x]["gridY"].data();
                    std::stringstream f;
                    f << "https://api.weather.gov/gridpoints/" << res.data[x]["gridId"].data() << "/"
                         << res.data[x]["gridX"].data() << "," << res.data[x]["gridY"].data() << "/forecast?units=us";
                    base["forecast"] = f.str();
                    std::stringstream fh;
                    fh << "https://api.weather.gov/gridpoints/" << res.data[x]["gridId"].data() << "/"
                        << res.data[x]["gridX"].data() << "," << res.data[x]["gridY"].data() << "/forecast/hourly?units=us";
                    base["forecastHourly"] = fh.str();
                    base["forecastGridData"] = res.data[x]["forecastGridData"].data();
                    base["zoneId"] = res.data[x]["zoneId"].data();
                    base["observationStations"] = res.data[x]["observationStations"].data();
                    base["radarStation"] = res.data[x]["radarStation"].data();
                    base["elevation"] = res.data[x]["elevation"].data();
                    base["name"] = res.data[x]["name"].data();
                    base["timeZone"] = res.data[x]["timeZone"].data();
                    base["stationIdentifier"] = res.data[x]["stationIdentifier"].data();
                }
            }
            return base;
        }

    }

    enum DataTypes {
        OBSERVATIONS,
        WNW,
        FORECAST,
        HOURLYFORECAST
    };

    struct watchAndWarn_t {
        EZTools::EZString id;
        EZTools::EZString areaDesc;
        std::vector<EZTools::EZString> SAME;
        EZDateTime::DateTime sent;
        EZDateTime::DateTime effective;
        EZDateTime::DateTime onset;
        EZDateTime::DateTime expires;
        EZDateTime::DateTime ends;
        EZTools::EZString status;
        EZTools::EZString messageType;
        EZTools::EZString category;
        EZTools::EZString severity;
        EZTools::EZString certainty;
        EZTools::EZString urgency;
        EZTools::EZString event;
        EZTools::EZString headline;
        EZTools::EZString description;
        EZTools::EZString instruction;
        EZTools::EZString response;
    };

    class WatchAndWarn {
    public:
        WatchAndWarn() = default;
        ~WatchAndWarn() = default;
        void get(nlohmann::json data) {
            nlohmann::json root = data["features"];
            if (root.is_array()) {
                for (auto& t : root) {
                    watchAndWarn_t w;
                    nlohmann::json ww;
                    if (t["properties"]["id"].is_string()) {
                        w.id = t["properties"]["id"].get<std::string>();
                        ww["id"] = w.id;
                    }
                    if (t["properties"]["areaDesc"].is_string()) {
                        w.areaDesc = t["properties"]["areaDesc"].get<std::string>();
                        ww["areaDesc"] = w.areaDesc;
                    }
                    nlohmann::json s = t["properties"]["geocode"]["SAME"];
                    for (auto& tt : s) {
                        if (tt.is_string()) {
                            w.SAME.push_back(tt);
                            ww["SAME"].push_back(tt);
                        }
                    }
                    if (t["properties"]["sent"].is_string()) {
                        w.sent.update(t["properties"]["sent"].get<std::string>());
                        ww["sent"] = t["properties"]["sent"].get<std::string>();
                    }
                    if (t["properties"]["effective"].is_string()) {
                        w.effective.update(t["properties"]["effective"].get<std::string>());
                        ww["effective"] = t["properties"]["effective"].get<std::string>();
                    }
                    if (t["properties"]["onset"].is_string()) {
                        w.onset.update(t["properties"]["onset"].get<std::string>());
                        ww["onset"] = t["properties"]["onset"].get<std::string>();
                    }
                    if (t["properties"]["expires"].is_string()) {
                        w.expires.update(t["properties"]["expires"].get<std::string>());
                        ww["expires"] = t["properties"]["expires"].get<std::string>();
                    }
                    if (t["properties"]["ends"].is_string()) {
                        w.ends.update(t["properties"]["ends"].get<std::string>());
                        ww["ends"] = t["properties"]["ends"].get<std::string>();
                    }
                    if (t["properties"]["childStatus"].is_string()) {
                        w.status = t["properties"]["childStatus"].get<std::string>();
                        ww["childStatus"] = w.status;
                    }
                    if (t["properties"]["messageType"].is_string()) {
                        w.messageType = t["properties"]["messageType"].get<std::string>();
                        ww["messageType"] = w.messageType;
                    }
                    if (t["properties"]["category"].is_string()) {
                        w.category = t["properties"]["category"].get<std::string>();
                        ww["category"] = w.category;
                    }
                    if (t["properties"]["severity"].is_string()) {
                        w.severity = t["properties"]["severity"].get<std::string>();
                        ww["severity"] = w.severity;
                    }
                    if (t["properties"]["certainty"].is_string()) {
                        w.certainty = t["properties"]["certainty"].get<std::string>();
                        ww["certainty"] = w.certainty;
                    }
                    if (t["properties"]["urgency"].is_string()) {
                        w.urgency = t["properties"]["urgency"].get<std::string>();
                        ww["urgency"] = w.urgency;
                    }
                    if (t["properties"]["event"].is_string()) {
                        w.event = t["properties"]["event"].get<std::string>();
                        ww["event"] = w.event;
                    }
                    if (t["properties"]["headline"].is_string()) {
                        w.headline = t["properties"]["headline"].get<std::string>();
                        ww["headline"] = w.headline;
                    }
                    if (t["properties"]["description"].is_string()) {
                        w.description = t["properties"]["description"].get<std::string>();
                        w.description.regexReplace("\n", " ");
                        ww["description"] = w.description;
                    }
                    if (t["properties"]["instruction"].is_string()) {
                        w.instruction = t["properties"]["instruction"].get<std::string>();
                        ww["instruction"] = w.instruction;
                    }
                    if (t["properties"]["response"].is_string()) {
                        w.response = t["properties"]["response"].get<std::string>();
                        ww["response"] = w.response;
                    }
                    _wnw.push_back(w);
                    _root.push_back(ww);
                }
            }
        }
        std::vector<watchAndWarn_t> watchAndWarn() { return _wnw; }
        nlohmann::json json() { return _root; }
    private:
        std::vector<watchAndWarn_t> _wnw;
        nlohmann::json _root = nlohmann::json::array();
    };

    struct forecast_t {
        int number;
        EZTools::EZString name;
        bool isDaytime;
        int temperature;
        EZTools::EZString temperatureTrend;
        EZTools::EZString windSpeed;
        EZTools::EZString windDirection;
        EZTools::EZString icon;
        EZTools::EZString shortForecast;
        EZTools::EZString detailedForecast;
    };

    class Forecast {
    public:
        Forecast() = default;
        ~Forecast() = default;
        void get(nlohmann::json data) {
            nlohmann::json root = data["properties"]["periods"];
            if (root.is_array()) {
                for (auto& t : root) {
                    forecast_t f;
                    nlohmann::json j;
                    if (t["number"].is_number()) {
                        f.number = t["number"].get<int>();
                        j["number"] = f.number;
                    }
                    if (t["name"].is_string()) {
                        f.name = t["name"].get<std::string>();
                        j["name"] = t["name"].get<std::string>();
                    }
                    if (t["temperature"].is_number()) {
                        f.temperature = t["temperature"].get<int>();
                        j["temperature"] = f.temperature;
                    }
                    if (t["temperatureTrend"].is_string()) {
                        f.temperatureTrend = t["temperatureTrend"].get<std::string>();
                        j["temperatureTrend"] = f.temperatureTrend;
                    }
                    if (t["windSpeed"].is_string()) {
                        f.windSpeed = t["windSpeed"].get<std::string>();
                        j["windSpeed"] = f.windSpeed;
                    }
                    if (t["windDirection"].is_string()) {
                        f.windDirection = t["windDirection"].get<std::string>();
                        j["windDirection"] = f.windDirection;
                    }
                    if (t["isDaytime"].is_boolean()) {
                        f.isDaytime = t["isDaytime"].get<bool>();
                        j["isDaytime"] = f.isDaytime;
                    }
                    if (t["shortForecast"].is_string()) {
                        f.shortForecast = t["shortForecast"].get<std::string>();
                        j["shortForecast"] = f.shortForecast;
                    }
                    if (t["icon"].is_string()) {
                        f.icon = t["icon"].get<std::string>();
                        j["icon"] = f.icon;
                    }
                    if (t["detailedForecast"].is_string()) {
                        f.detailedForecast = t["detailedForecast"].get<std::string>();
                        j["detailedForecast"] = f.detailedForecast;
                    }
                    _forecast.push_back(f);
                    _root.push_back(j);
                }
            }
        }
        nlohmann::json json() { return _root; }
        std::vector<forecast_t> forecast() { return _forecast; }
    private:
        std::vector<forecast_t> _forecast;
        nlohmann::json _root = nlohmann::json::array();
    };

    struct hourlyForecast_t {
        int number;
        EZDateTime::DateTime startTime;
        bool isDaytime;
        int temperature;
        EZTools::EZString windSpeed;
        EZTools::EZString windDirection;
        EZTools::EZString icon;
        EZTools::EZString shortForecast;
    };

    class HourlyForecast {
    public:
        HourlyForecast() = default;
        ~HourlyForecast() = default;
        void get(nlohmann::json data, int number = -1) {
            nlohmann::json root = data["properties"]["periods"];
            if (root.is_array()) {
                int counter = 0;
                for (auto& t : root) {
                    hourlyForecast_t f;
                    nlohmann::json j;
                    if (t["number"].is_number()) {
                        f.number = t["number"].get<int>();
                        j["number"] = f.number;
                    }
                    if (t["startTime"].is_string()) {
                        f.startTime.update(t["startTime"].get<std::string>());
                        f.startTime.makeLocal();
                        j["startTime"] = f.startTime.iso8601();
                    }
                    if (t["temperature"].is_number()) {
                        f.temperature = t["temperature"].get<int>();
                        j["temperature"] = f.temperature;
                    }
                    if (t["windSpeed"].is_string()) {
                        f.windSpeed = t["windSpeed"].get<std::string>();
                        j["windSpeed"] = f.windSpeed;
                    }
                    if (t["windDirection"].is_string()) {
                        f.windDirection = t["windDirection"].get<std::string>();
                        j["windDirection"] = f.windDirection;
                    }
                    if (t["isDaytime"].is_boolean()) {
                        f.isDaytime = t["isDaytime"].get<bool>();
                        j["isDaytime"] = f.isDaytime;
                    }
                    if (t["shortForecast"].is_string()) {
                        f.shortForecast = t["shortForecast"].get<std::string>();
                        j["shortForecast"] = f.shortForecast;
                    }
                    if (t["icon"].is_string()) {
                        f.icon = t["icon"].get<std::string>();
                        f.icon.regexReplace("size=small", "size=large");
                        j["icon"] = f.icon;
                    }
                    _forecast.push_back(f);
                    _root.push_back(j);
                    counter++;
                    if (number != -1) {
                        if (counter >= number) {
                            break;
                        }
                    }
                }
            }
        }
        nlohmann::json json() { return _root; }
        std::vector<hourlyForecast_t> hourlyForecast() { return _forecast; }
    private:
        std::vector<hourlyForecast_t> _forecast;
        nlohmann::json _root = nlohmann::json::array();
    };

    struct observations_t {
        EZDateTime::DateTime timestamp;
        EZTools::EZString textDescription;
        EZTools::EZString icon;
        float temperature = 0;
        float dewpoint = 0;
        int windDirection = 0;
        float windSpeed = 0;
        float windGust = 0;
        float barometricPressure = 0;
        float seaLevelPressure = 0;
        float visibility = 0;
        float maxTemperatureLast24Hours = 0;
        float minTemperatureLast24Hours = 0;
        float precipitationLastHour = 0;
        float precipitationLast3Hours = 0;
        float precipitationLast6Hours = 0;
        float relativeHumidity = 0;
        float windChill = 0;
        float heatIndex = 0;
    };

    class Observations {
    public:
        Observations() = default;
        ~Observations() = default;
        void get(nlohmann::json data) {
            nlohmann::json root = data["features"][0]["properties"];
            if (root["timestamp"].is_string()) {
                _observations.timestamp.update(root["timestamp"].get<std::string>());
                _observations.timestamp.makeLocal();
                _root["timestamp"] = _observations.timestamp.iso8601();
            } else {
                EZDateTime::DateTime n;
                _observations.timestamp.update(n.iso8601());
            }
            if (root["textDescription"].is_string()) {
                _observations.textDescription = root["textDescription"].get<std::string>();
                _root["textDescription"] = root["textDescription"].get<std::string>();
            }
            if (root["icon"].is_string()) {
                _observations.icon = root["icon"].get<std::string>();
                _root["icon"] = root["icon"].get<std::string>();
            }
            if (root["temperature"]["value"].is_number()) {
                float b = root["temperature"]["value"].get<float>();
                _root["temperature"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.temperature = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
            if (root["dewpoint"]["value"].is_number()) {
                float b = root["dewpoint"]["value"].get<float>();
                _root["dewpoint"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.dewpoint = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
            if (root["windDirection"]["value"].is_number()) {
                int d = root["windDirection"]["value"].get<int>();
                _root["windDirection"] = EZTools::EZString(d);
                _observations.windDirection = d;
            }
            if (root["windSpeed"]["value"].is_number()) {
                float b = root["windSpeed"]["value"].get<float>();
                _root["windSpeed"] = EZWeather::Utils::kilometersPerHourToMilesPerHour(b);
                _observations.windSpeed = EZWeather::Utils::kilometersPerHourToMilesPerHourf(b);
            }
            if (root["windGust"]["value"].is_number()) {
                float b = root["windGust"]["value"].get<float>();
                _root["windGust"] = EZWeather::Utils::kilometersPerHourToMilesPerHour(b);
                _observations.windGust = EZWeather::Utils::kilometersPerHourToMilesPerHourf(b);
            }
            if (root["barometricPressure"]["value"].is_number()) {
                float b = root["barometricPressure"]["value"].get<float>();
                _root["barometricPressure"] = EZWeather::Utils::pascalsToMillibars(b);
                _observations.barometricPressure = EZWeather::Utils::pascalsToMillibarsf(b);
            }
            if (root["seaLevelPressure"]["value"].is_number()) {
                float b = root["seaLevelPressure"]["value"].get<float>();
                _root["seaLevelPressure"] = EZWeather::Utils::pascalsToMillibars(b);
                _observations.seaLevelPressure = EZWeather::Utils::pascalsToMillibarsf(b);
            }
            if (root["visibility"]["value"].is_number()) {
                float b = root["visibility"]["value"].get<float>();
                _root["visibility"] = EZWeather::Utils::metersToMiles(b);
                _observations.visibility = EZWeather::Utils::metersToMilesf(b);
            }
            if (root["maxTemperatureLast24Hours"]["value"].is_number()) {
                float b = root["maxTemperatureLast24Hours"]["value"].get<float>();
                _root["maxTemperatureLast24Hours"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.maxTemperatureLast24Hours = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
            if (root["minTemperatureLast24Hours"]["value"].is_number()) {
                float b = root["minTemperatureLast24Hours"]["value"].get<float>();
                _root["minTemperatureLast24Hours"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.minTemperatureLast24Hours = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
            if (root["precipitationLastHour"]["value"].is_number()) {
                float b = root["precipitationLastHour"]["value"].get<float>();
                _root["precipitationLastHour"] = EZWeather::Utils::metersToInches(b);
                _observations.precipitationLastHour = EZWeather::Utils::metersToInchesf(b);
            }
            if (root["precipitationLast3Hours"]["value"].is_number()) {
                float b = root["precipitationLast3Hours"]["value"].get<float>();
                _root["precipitationLast3Hours"] = EZWeather::Utils::metersToInches(b);
                _observations.precipitationLast3Hours = EZWeather::Utils::metersToInchesf(b);
            }
            if (root["precipitationLast6Hours"]["value"].is_number()) {
                float b = root["precipitationLast6Hours"]["value"].get<float>();
                _root["precipitationLast6Hours"] = EZWeather::Utils::metersToInches(b);
                _observations.precipitationLast6Hours = EZWeather::Utils::metersToInchesf(b);
            }
            if (root["relativeHumidity"]["value"].is_number()) {
                _root["relativeHumidity"] =
                        EZTools::EZString(
                                EZWeather::Utils::Round(root["relativeHumidity"]["value"].get<float>()));
                _observations.relativeHumidity = EZWeather::Utils::Roundf(
                        root["relativeHumidity"]["value"].get<float>());
            }
            if (root["windChill"]["value"].is_number()) {
                float b = root["windChill"]["value"].get<float>();
                _root["windchill"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.windChill = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
            if (root["heatIndex"]["value"].is_number()) {
                float b = root["heatIndex"]["value"].get<float>();
                _root["heatindex"] = EZWeather::Utils::celsiusToFahrenheit(b);
                _observations.heatIndex = EZWeather::Utils::celsiusToFahrenheitf(b);
            }
        }
        nlohmann::json json() { return _root; }
        observations_t observations() { return _observations; }
    private:
        nlohmann::json _root;
        observations_t _observations;
    };

    inline EZTools::EZReturn<nlohmann::json> httpController(EZTools::EZString URL, EZTools::EZString userAgent,
                                                            EZTools::EZString version) {
        std::stringstream st;
        st << userAgent << " v" << version << ", mromans@romansaudio.com";
        EZHTTP::Headers headers;
        headers.addPair("User-Agent", st.str());
        headers.addPair("Accept", "application/geo+json");
        EZHTTP::URI uri("https://api.weather.gov");
        uri.requestHeaders(headers.list());
        EZHTTP::HTTPClient http(uri);
        EZTools::EZReturn<nlohmann::json> blah;
        for (int i = 0; i < 5; i++) {
            EZTools::EZReturn<nlohmann::json> ret;
            auto res = http.get(URL);
            if (res.wasSuccessful()) {
                ret.wasSuccessful(true);
                ret.data = nlohmann::json::parse(res.data);
                return ret;
            } else {
                ret.data["ERROR"] = nlohmann::json::object();
                ret.data["ERROR"]["httpCode"] = res.httpCode();
                ret.data["ERROR"]["message"] = res.message();
                ret.message(res.message());
                blah = ret;
                EZLinux::EZNanoSleep(300);
            }
        }
        return blah;
    }

    struct confOptions {
        EZMySQL::MySQLConnetionInfo con;
        EZTools::EZString defaultZipcode;
        nlohmann::json location;
    };

    confOptions readConfig(EZTools::EZString &zip, bool &full) {
        confOptions co;
        EZFiles::URI confFileLocation("/etc/EZWeather/EZWeather.json");
        EZFiles::TextFile confFile(&confFileLocation, EZFiles::READ);
        nlohmann::json conf = nlohmann::json::parse(confFile.contents());
        if (conf["isFullForecast"].is_boolean() && !full) {
            full = conf["isFullForecast"].get<bool>();
        }
        if (zip.empty()) {
            if (conf["defaultZipcode"].is_string()) {
                co.defaultZipcode = conf["defaultZipcode"].get<EZTools::EZString>();
            }
        } else {
            co.defaultZipcode = zip;
        }
        co.con.init(conf["database"]["server"].get<EZTools::EZString>(),
                    conf["database"]["database"].get<EZTools::EZString>(),
                    conf["database"]["user"].get<EZTools::EZString>(),
                    conf["database"]["password"].get<EZTools::EZString>());
        EZDateTime::Timer dbs;
        dbs.start();
        co.location = EZLocationData::locationSearchByZipcode(co.con, co.defaultZipcode);
        dbs.stop();
        co.location["zipcode"] = co.defaultZipcode;
        co.location["dbLookup"] = dbs.asMillisenconds();
        std::stringstream ss;
        ss << "/stations/" <<
           co.location["stationIdentifier"].get<EZTools::EZString>() << "/observations?limit=1";
        co.location["observations"] = ss.str();
        co.location["wnw"] = "/alerts/active/zone/" +
                             co.location["zoneId"].get<std::string>();
        return co;
    }

    class WeatherData {
    public:
        WeatherData() = default;
        ~WeatherData() = default;
        void getDataThreaded(EZMySQL::MySQLConnetionInfo conInfo, EZTools::EZString zipcode, int numberOfHourly = -1) {
            EZDateTime::Timer t;
            t.start();
            _coninfo = conInfo;
            EZDateTime::Timer dbs;
            dbs.start();
            _locationData = EZLocationData::locationSearchByZipcode(conInfo, zipcode);
            dbs.stop();
            _locationData["zipcode"] = zipcode;
            _locationData["dbLookup"] = dbs.asMillisenconds();
            std::stringstream ss;
            ss << "/stations/" <<
               _locationData["stationIdentifier"].get<EZTools::EZString>() << "/observations?limit=1";
            _locationData["observations"] = ss.str();
            _locationData["wnw"] = "/alerts/active/zone/" +
                                   _locationData["zoneId"].get<std::string>();
            _root["location"] = _locationData;
            pthread_t id1, id2, id3, id4;
            nlohmann::json* ptr1, ptr2, ptr3, ptr4;
            EZTools::EZString observationsURL = ss.str();
            EZTools::EZString hourlyForecastURL = _locationData["forecastHourly"].get<std::string>();
            EZTools::EZString forecastURL = _locationData["forecast"].get<std::string>();
            EZTools::EZString wnwURL = _locationData["wnw"].get<std::string>();

            auto observationsData = httpController(ss.str(), _userAgent, _version);
            auto hourlyForecastData = httpController(hourlyForecastURL, _userAgent, _version);
            auto forecastData = httpController(forecastURL, _userAgent, _version);
            auto wnwData = httpController(wnwURL, _userAgent, _version);
            if (observationsData.wasSuccessful()) {
                _observations.get(observationsData.data);
            }
            _root["observations"] = _observations.json();
            if (hourlyForecastData.wasSuccessful()) {
                _hourlyForecast.get(hourlyForecastData.data, numberOfHourly);
            }
            _root["hourlyForecast"] = _hourlyForecast.json();
            if (forecastData.wasSuccessful()) {
                _forecast.get(forecastData.data);
            }
            _root["forecast"] = _forecast.json();
            if (wnwData.wasSuccessful()) {
                _watchAndWarn.get(wnwData.data);
            }
            _root["WatchesAndWarnings"] = _watchAndWarn.json();
            _root["radarLoop"] = WeatherUnderground::radarURL(_locationData["state"].get<std::string>());
            t.stop();
            _root["time"] = t.asSeconds();
        }
        void userAgentEMail(EZTools::EZString EMail) {
            std::stringstream ss;
            ss << "EZWeather API v" << _version << ", email: YOUREMAILHERE";
            _userAgent = ss.str();
            _userAgent.regexReplace("YOUREMAILHERE", EMail);
        }
        void userAgentString(EZTools::EZString UserAgent) {
            _userAgent = UserAgent;
        }
        EZTools::EZString userAgentString() { return _userAgent; }
        EZMySQL::MySQLConnetionInfo databaseConnectionInfo() { return _coninfo; }
        nlohmann::json locationData() { return _locationData; }
        nlohmann::json json() { return _root; }
        observations_t observations() { return _observations.observations(); }
        std::vector<hourlyForecast_t> hourlyForecast() { return _hourlyForecast.hourlyForecast(); }
        std::vector<forecast_t> forecast() { return _forecast.forecast(); }
        std::vector<watchAndWarn_t> watchAndWarn() { return _watchAndWarn.watchAndWarn(); }
    private:
        EZTools::EZString _version = VERSION;
        EZTools::EZString _userAgent;
        EZMySQL::MySQLConnetionInfo _coninfo;
        nlohmann::json _locationData;
        nlohmann::json _root;
//        EZHTTP::HTTP _http;
        Observations _observations;
        HourlyForecast _hourlyForecast;
        Forecast _forecast;
        WatchAndWarn _watchAndWarn;
    };

}

#endif //EZ_EZWEATHER_H