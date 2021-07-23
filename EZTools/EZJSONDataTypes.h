/*
Copyright (c) 2017-2021, Michael Romans of Romans Audio
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

#ifndef EZT_EZJSONDATATYPES_H
#define EZT_EZJSONDATATYPES_H
#include <iomanip>
#include "EZJSONDataBaseTypes.h"

namespace EZJSONData {

    enum Temperature_t {
        Fahrenheit,
        Celsius
    };

    inline EZTools::EZString Temerature_tAsString(Temperature_t type) {
        switch (type) {
            case Fahrenheit:
                return "Fahrenheit";
            case Celsius:
                return "Celsius";
            default:
                return "Error";
        }
    }

    inline Temperature_t Temperature_tFromString(EZTools::EZString unit) {
        if (unit == "Fahrenheit") {
            return Fahrenheit;
        } else {
            return Celsius;
        }
    }

    class Temperature: public Numeric<double> {
    public:
        using Numeric<double>::Numeric;
        Temperature(float value, Temperature_t unit) : Numeric(value) {
            _root["type"] = Numeric_tToString(FLOAT);
            _data = value;
            _unit = unit;
            _root["value"] = _data;
            _root["unit"] = _unit;
        }
        Temperature(nlohmann::json root) {
            if (root["unit"].is_string()) {
                _unit = Temperature_tFromString(root["unit"].get<EZTools::EZString>());
                _root["unit"] = Temerature_tAsString(_unit);
            }
            if (root["value"].is_string()) {
                _data = root["value"].get<double>();
                _root["value"] = _data;
            }
        }
        double asFahrenheit() {
            if (_unit == Fahrenheit) {
                return _data;
            } else {
                return (_data - 32) * 5 / 9;
            }
        }
        double asCelsius() {
            if (_unit == Celsius) {
                return _data;
            } else {
                return (_data - 32) * 5 / 9;
            }
        }
        nlohmann::json json() {
            _root["type"] = "double";
            _root["unit"] = Temerature_tAsString(_unit);
            return _root;
        }
        nlohmann::json jsonAsUnit(Temperature_t unit, int decimalPlaces = -1) {
            nlohmann::json root;
            root["type"] = _root["type"].get<EZTools::EZString>();
            root["unit"] = Temerature_tAsString(unit);
            double val;
            val = _data;
            if (unit != _unit) {
                switch (unit) {
                    case Fahrenheit:
                        val = (_data * 9 / 5) + 32;
                        break;
                    case Celsius:
                        val = (_data - 32) * 5 / 9;
                        break;
                }
            }
            if (decimalPlaces > -1) {
                val = EZTools::round(val, decimalPlaces);
            }
            root["value"] = val;
            return root;
        }
    private:
        Temperature_t _unit;
    };

    enum Distance_t {
        Mile,
        Foot,
        Inch,
        Kilometer,
        Meter,
        Centimeter,
        Error
    };

    inline EZTools::EZString Distance_tAsString(Distance_t unit) {
        switch (unit) {
            case Mile:
                return "Mile";
            case Foot:
                return "Foot";
            case Inch:
                return "Inch";
            case Kilometer:
                return "Kilometer";
            case Meter:
                return "Meter";
            case Centimeter:
                return "Centimeter";
            default:
                return "Error";
        }
    }

    inline Distance_t Distance_tFromString(EZTools::EZString unit) {
        if (unit == "Mile") {
            return Mile;
        } else if (unit == "Foot") {
            return Foot;
        } else if (unit == "Inch") {
            return Inch;
        } else if (unit == "Kilometer") {
            return Kilometer;
        } else if (unit == "Meter") {
            return Meter;
        } else if (unit == "Centimeter") {
            return Centimeter;
        }
        return Error;
    }

    class Distance: public Numeric<double> {
    public:
        using Numeric<double>::Numeric;
        Distance(double value, Distance_t unit) : Numeric(value) {
            _data = value;
            _unit = unit;
            _root["value"] = _data;
        }
        Distance(nlohmann::json root) {
            if (root["unit"].is_string()) {
                _unit = Distance_tFromString(root["unit"].get<EZTools::EZString>());
                _root["unit"] = Distance_tAsString(_unit);
            }
            if (root["value"].is_number_float()) {
                _data = root["value"].get<double>();
                _root["value"] = _data;
            }
        }
        double asInch() {
            if (_unit == Inch) {
                return _data;
            }
            switch (_unit) {
                case Foot:
                    return _data * 12;
                case Mile:
                    return _data * 63360;
                case Centimeter:
                    return _data / 2.54;
                case Meter:
                    return _data * 39.37;
                case Kilometer:
                    return _data * 39370;
                default:
                    return 0;
            }
        }
        void setUnit(Distance_t unit) {
            _unit = unit;
        }
        double asFoot() {
            double i = asInch();
            return i / 12;
        }
        double asMile() {
            double i = asInch();
            return i / 63360;
        }
        double asCentimeter() {
            double i = asInch();
            return i * 2.54;
        }
        double asMeter() {
            double i = asInch();
            return i / 39.37;
        }
        double asKilometer() {
            double i = asInch();
            return i / 39370;
        }
        nlohmann::json json() {
            _root["unit"] = Distance_tAsString(_unit);
            return _root;
        }
        nlohmann::json jsonAsUnit(Distance_t unit) {
            nlohmann::json r;
            r["type"] = _root["type"].get<EZTools::EZString>();
            r["unit"] = Distance_tAsString(unit);
            r["value"] = 0.0;
            switch (unit) {
                case Inch:
                    r["value"] = asInch();
                    break;
                case Foot:
                    r["value"] = asFoot();
                    break;
                case Mile:
                    r["value"] = asMile();
                    break;
                case Centimeter:
                    r["value"] = asCentimeter();
                    break;
                case Meter:
                    r["value"] = asMeter();
                    break;
                case Kilometer:
                    r["value"] = asKilometer();
                    break;
            }
            return r;
        }
    private:
        Distance_t _unit;
    };

    enum Speed_t {
        MilesPerHour,
        KilometersPerHour,
        FeetPerSecond,
        MetersPerSecond,
        UNKNOWNSPEED
    };

    inline EZTools::EZString Speed_tAsString(Speed_t unit) {
        switch (unit) {
            case MilesPerHour:
                return "milesPerHour";
            case KilometersPerHour:
                return "kilometersPerHour";
            case FeetPerSecond:
                return "feetPerSecond";
            case MetersPerSecond:
                return "metersPerSecond";
            default:
                return "";
        }
    }

    inline Speed_t Speed_tFromString(EZTools::EZString unit) {
        if (unit.lower() == "feetpersecond") {
            return FeetPerSecond;
        } else if (unit.lower() == "meterspersecond") {
            return MetersPerSecond;
        } else if (unit.lower() == "milesperhour") {
            return MilesPerHour;
        } else if (unit.lower() == "kilometersperhour") {
            return KilometersPerHour;
        } else {
            return UNKNOWNSPEED;
        }
    }

    class Speed: public Numeric<double> {
    public:
        using Numeric<double>::Numeric;
        Speed(Speed_t unit, double speed) {
            _unit = unit;
            _data = speed;
            _root["type"] = "speed";
            _root["unit"] = Speed_tAsString(_unit);
            _root["value"] = _data;
        }
        double asFeetPerSecond() {
            if (_unit == FeetPerSecond) {
                return _data;
            }
            switch (_unit) {
                case MilesPerHour:
                    return _data * 1.467;
                case MetersPerSecond:
                    return _data / 2.237;
                case KilometersPerHour:
                    return _data / 1.097;
                default:
                    return 0;
            }
        }
        double asMilesPerHour() {
            if (_unit == MilesPerHour) {
                return _data;
            }
            return asFeetPerSecond() / 1.467;
        }
        double asKilometersPerHour() {
            if (_unit == KilometersPerHour) {
                return _data;
            }
            return asFeetPerSecond() * 1.097;
        }
        double asMetersPerSecond() {
            if (_unit == MetersPerSecond) {
                return _data;
            }
            return asMetersPerSecond() * 2.237;
        }
        nlohmann::json json() {
            return _root;
        }
        nlohmann::json jsonWithUnit(Speed_t unit) {
            nlohmann::json root = nlohmann::json::object();
            root["type"] = "speed";
            root["unit"] = Speed_tAsString(unit);
            if (_unit == unit) {
                root["value"] = _data;
            } else {
                switch (unit) {
                    case FeetPerSecond:
                        root["value"] = asFeetPerSecond();
                        break;
                    case MetersPerSecond:
                        root["value"] = asMetersPerSecond();
                        break;
                    case MilesPerHour:
                        root["value"] = asMilesPerHour();
                        break;
                    case KilometersPerHour:
                        root["value"] = asKilometersPerHour();
                        break;
                    default:
                        root["value"] = 0;
                        break;
                }
            }
            return root;
        }
        EZTools::EZString dump(int indent) {
            return _root.dump(indent);
        }
    private:
        Speed_t _unit;
    };

    //Fixme: This needs fleshed out but I did this for my weather API
    enum Pressure_t {
        InchesOfMercury,
        Pascals
    };

    inline EZTools::EZString Pressure_tToString(Pressure_t unit) {
        switch (unit) {
            case InchesOfMercury:
                return "InchesOfMercury";
            case Pascals:
                return "Pascal";
            default:
                return "";
        }
    }

    class Pressure: public Numeric<double> {
    public:
        using Numeric<double>::Numeric;
        Pressure(Pressure_t unit, double value) {
            _unit = unit;
            _data = value;
            _root["type"] = "Pressure";
            _root["unit"] = Pressure_tToString(_unit);
            _root["value"] = _data;
        }
        double asInchesOfMercury() {
            if (_unit == InchesOfMercury) {
                return _data;
            } else {
                return _data / 3386;
            }
        }
        double asBars() {
            if (_unit == Pascals) {
                return _data;
            } else {
                return _data * 3386;
            }
        }
        nlohmann::json jsonWithUnit(Pressure_t unit) {
            nlohmann::json root = nlohmann::json::object();
            root["type"] = "Pressure";
            root["unit"] = Pressure_tToString(unit);
            if (_unit == unit) {
                root["value"] = _data;
            } else {
                switch (unit) {
                    case InchesOfMercury:
                        root["value"] = asInchesOfMercury();
                        break;
                    case Pascals:
                        root["value"] = asBars();
                        break;
                    default:
                        root["value"] = 0;
                        break;
                }
            }
            return root;
        }
    private:
        Pressure_t _unit;
    };

//    struct ExampleObject {
//        Temperature temperature;
//        Distance distance;
//        Numeric<int> aint;
//        String astring;
//        Bool abool;
//        ExampleObject(double temp, Temperature_t tempUnit, double dist, Distance_t distUnit, int intval,
//                      bool boolval, std::string stringval) {
//            temperature = temp;
//            temperature.setUnit(tempUnit);
//            distance = 432;
//            distance.setUnit(Mile);
//            aint = intval;
//            abool = boolval;
//            astring = stringval;
//        }
//        ExampleObject(nlohmann::json json) {
//            if (json["tempFahrenheit"].is_object()) {
//                temperature = Temperature(json["tempFahrenheit"]);
//            }
//            if (json["distance"].is_object()) {
//                distance = Distance(json["distance"]);
//            }
//            if (json["exampleInt"].is_object()) {
//                aint = Numeric<int>(json["exampleInt"]);
//            }
//            if (json["exampleBool"].is_object()) {
//                abool = Bool(json["exampleBool"]);
//            }
//            if (json["exampleString"].is_object()) {
//                astring = String(json["exampleString"]);
//            }
//        }
//        nlohmann::json json() {
//            nlohmann::json root = nlohmann::json::object();
//            root["tempFahrenheit"] = temperature.jsonAsUnit(Fahrenheit);
//            root["tempCelsius"] = temperature.jsonAsUnit(Celsius);
//            root["distance"] = distance.json();
//            root["distanceMeters"] = distance.jsonAsUnit(Meter);
//            root["exampleInt"] = aint.json();
//            root["exampleBool"] = abool.json();
//            root["exampleString"] = astring.json();
//            return root;
//        }
//    };

}

#endif //EZT_EZJSONDATATYPES_H
