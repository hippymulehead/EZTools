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

#ifndef EZT_EZJSONDATABASETYPES_H
#define EZT_EZJSONDATABASETYPES_H
#include <ostream>
#include <istream>
#include "EZTools.h"
#include "EZDateTime.h"
#include "json.h"

namespace EZJSONData {

    inline nlohmann::json contextObject() {
        nlohmann::json root = nlohmann::json::object();
        root["context"] = nlohmann::json::object();
        root["context"]["dataType"] = "EZTools::EZJSONData";
        root["context"]["version"] = VERSION;
        root["context"]["refrence"] = "https://github.com/hippymulehead/EZTools";
        EZDateTime::CurrentDateTime dt;
        root["timestamp"] = dt.ymdto();
        return root;
    }

    enum Numeric_t {
        INT,
        UNSIGNED,
        UNSIGNEDLONG,
        UNSIGNEDLONGLONG,
        FLOAT,
        DOUBLE,
        UINT8,
        UINT16,
        UINT32,
        INT8,
        INT16,
        INT32,
        UNKNOWN
    };

    inline EZTools::EZString Numeric_tToString(Numeric_t type) {
        switch (type) {
            case INT:
                return "int";
            case UNSIGNED:
                return "unsigned";
            case UNSIGNEDLONG:
                return "unsignedLong";
            case UNSIGNEDLONGLONG:
                return "unsignedLongLong";
            case FLOAT:
                return "float";
            case DOUBLE:
                return "double";
            case UINT8:
                return "uint8_t";
            case UINT16:
                return "unit16_t";
            case UINT32:
                return "uint32_t";
            case INT8:
                return "int8_t";
            case INT16:
                return "int16_t";
            case INT32:
                return "int32_t";
            default:
                return "unknown_t";
        }
    }

    inline Numeric_t stringToNumeric_t(EZTools::EZString type) {
        EZTools::EZString t = type.lower();
        if (t == "int") {
            return INT;
        } else if (t == "unsigned") {
            return UNSIGNED;
        } else if (t == "unsignedlong") {
            return UNSIGNEDLONG;
        } else if (t == "unsignedlonglong") {
            return UNSIGNEDLONGLONG;
        } else if (t == "float") {
            return FLOAT;
        } else if (t == "double") {
            return DOUBLE;
        } else if (t == "uint8_t") {
            return UINT8;
        } else if (t == "uint16_t") {
            return UINT16;
        } else if (t == "uint32_t") {
            return UINT32;
        } else if (t == "int8_t") {
            return INT8;
        } else if (t == "int16_t") {
            return INT16;
        } else if (t == "int32_t") {
            return INT32;
        } else {
            return UNKNOWN;
        }
    }

    inline Numeric_t Numeric_tFromTypeid(EZTools::EZString type) {
        if (type == "i") {
            return INT;
        } else if (type == "j") {
            return UNSIGNED;
        } else if (type == "m") {
            return UNSIGNEDLONG;
        } else if (type == "y") {
            return UNSIGNEDLONGLONG;
        } else if (type == "f") {
            return FLOAT;
        } else if (type == "d") {
            return DOUBLE;
        } else if (type == "h") {
            return UINT8;
        } else if (type == "t") {
            return UINT16;
        } else {
            return UNKNOWN;
        }
    }

    template <typename T>
    class Numeric {
    public:
        Numeric() {
            _type = Numeric_tFromTypeid(typeid(_data).name());
            _root["type"] = Numeric_tToString(_type);
        };
        Numeric(T value) {
            _type = Numeric_tFromTypeid(typeid(_data).name());
            _data = value;
            _root["type"] = Numeric_tToString(_type);
            _root["value"] = _data;
            _hasData = true;
        }
        Numeric(nlohmann::json objectRoot) {
            if (objectRoot["type"].is_string()) {
                _type = Numeric_tFromTypeid(typeid(_data).name());
                _root["type"] = Numeric_tToString(_type);
                switch (_type) {
                    case INT:
                        _data = objectRoot["value"].get<int>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UNSIGNED:
                        _data = objectRoot["value"].get<unsigned>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UNSIGNEDLONG:
                        _data = objectRoot["value"].get<unsigned long>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UNSIGNEDLONGLONG:
                        _data = objectRoot["value"].get<unsigned long long>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UINT8:
                        _data = objectRoot["value"].get<uint8_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UINT16:
                        _data = objectRoot["value"].get<uint16_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case UINT32:
                        _data = objectRoot["value"].get<uint32_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case INT8:
                        _data = objectRoot["value"].get<int8_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case INT16:
                        _data = objectRoot["value"].get<int16_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    case INT32:
                        _data = objectRoot["value"].get<int32_t>();
                        _root["value"] = _data;
                        _hasData = true;
                        break;
                    default:
                        break;
                }
            }
        }
        virtual ~Numeric() = default;
        bool operator==(T const &obj) {
            return this->_data == obj;
        }

        bool operator!=(T const &obj) {
            return this->_data != obj;
        }

        bool operator>=(T const &obj) {
            return this->_data >= obj;
        }

        bool operator<=(T const &obj) {
            return this->_data <= obj;
        }

        Numeric &operator++() {
            _data++;
            _root["value"] = _data;
            return *this;
        }

        Numeric &operator++(int) {
            ++_data;
            _root["value"] = _data;
            return *this;
        }

        Numeric &operator--() {
            _data--;
            _root["value"] = _data;
            return *this;
        }

        Numeric &operator--(int) {
            --_data;
            _root["value"] = _data;
            return *this;
        }

        Numeric operator+(T const &obj) {
            Numeric<T> t;
            t = _data + obj;
            return t;
        }

        const Numeric operator-(T const &obj) {
            Numeric<T> t;
            t = _data - obj;
            return t;
        }

        Numeric operator=(T const &obj) {
            _data = obj;
            _root["value"] = _data;
            return *this;
        }

        Numeric operator+=(T const &obj) {
            Numeric<T> t;
            t = _data += obj;
            return t;
        }

        Numeric operator-=(T const &obj) {
            Numeric<T> t;
            t = _data -= obj;
            return t;
        }

        Numeric operator*(T const &obj) {
            Numeric<T> t;
            t = _data * obj;
            return t;
        }

        Numeric operator/(T const &obj) {
            Numeric<T> t;
            t = _data / obj;
            return t;
        }

        virtual nlohmann::json json() {
            return _root;
        }
        T value() { return _data; }
        Numeric_t type() { return _type; }
        friend std::ostream &operator<<(std::ostream &output, Numeric<T> &D ) {
            output << D.value();
            return output;
        }

    protected:
        bool _hasData = false;
        Numeric_t _type;
        nlohmann::json _root;
        T _data;
    };

    class Bool {
    public:
        Bool() {
            _root["type"] = "bool";
        }
        Bool(bool value) {
            _data = value;
            _root["value"] = _data;
            _root["type"] = "bool";
        }
        Bool(nlohmann::json root) {
            if (root["value"].is_boolean()) {
                _data = root["value"].get<bool>();
                _root["value"] = _data;
            }
        }
        ~Bool() = default;
        bool operator==(bool const &obj) {
            return this->_data == obj;
        }

        bool operator!=(bool const &obj) {
            return this->_data != obj;
        }
        Bool operator=(bool const &obj) {
            _data = obj;
            return *this;
        }
        bool value() {
            return _data;
        }
        nlohmann::json json() {
            return _root;
        }
    private:
        EZTools::EZString _type = "bool";
        bool _data = false;
        nlohmann::json _root;
    };

    class String: public EZTools::EZString {
    public:
        using EZTools::EZString::EZString;
        String(nlohmann::json root) {
            if (root["value"].is_string()) {
                *this = root["value"].get<EZTools::EZString>();
            }
        }
        EZTools::EZString value() {
            return *this;
        }
        nlohmann::json json() {
            _root["type"] = _type;
            _root["value"] = this->str();
            return _root;
        }
    private:
        EZTools::EZString _type = "string";
        nlohmann::json _root;
    };

    template <typename T>
    class Array: public std::vector<T> {
    public:
        enum Array_t {
            INT,
            UNSIGNED,
            UNSIGNEDLONG,
            UNSIGNEDLONGLONG,
            FLOAT,
            DOUBLE,
            UINT8,
            UINT16,
            UINT32,
            INT8,
            INT16,
            INT32,
            STRING,
            UNKNOWN
        };
        using std::vector<T>::vector;
        Array() {
            EZTools::EZString tid = typeid(T).name();
            _unit = stringToType(tid);
            if (_unit == UNKNOWN) {

            }
            _root["type"] = "Array";
            _root["unit"] = typeToString(_unit);
        }
        Array(nlohmann::json root) {

        }
        nlohmann::json json() {
            nlohmann::json v = nlohmann::json::array();
            for (auto& l : *this) {
                v.push_back(l);
            }
            _root["value"] = v;
            return _root;
        }

    private:
        Array_t _unit;
        nlohmann::json _root;
    protected:
        EZTools::EZString typeToString(Array_t type) {
            switch (type) {
                case STRING:
                    return "string";
                case INT:
                    return "int";
                case UNSIGNED:
                    return "unsigned";
                case UNSIGNEDLONG:
                    return "unsignedLong";
                case UNSIGNEDLONGLONG:
                    return "unsignedLongLong";
                case FLOAT:
                    return "float";
                case DOUBLE:
                    return "double";
                case UINT8:
                    return "uint8_t";
                case UINT16:
                    return "unit16_t";
                case UINT32:
                    return "uint32_t";
                case INT8:
                    return "int8_t";
                case INT16:
                    return "int16_t";
                case INT32:
                    return "int32_t";
                default:
                    return "unknown";
            }
        }
        Array_t stringToType(EZTools::EZString type) {
            if (type.regexMatch("basic_string|EZString")) {
                return STRING;
            }
            if (type == "i") {
                return INT;
            } else if (type == "j") {
                return UNSIGNED;
            } else if (type == "m") {
                return UNSIGNEDLONG;
            } else if (type == "y") {
                return UNSIGNEDLONGLONG;
            } else if (type == "f") {
                return FLOAT;
            } else if (type == "d") {
                return DOUBLE;
            } else if (type == "h") {
                return UINT8;
            } else if (type == "t") {
                return UINT16;
            } else {
                return UNKNOWN;
            }
        }
    };

}

#endif //EZT_EZJSONDATABASETYPES_H