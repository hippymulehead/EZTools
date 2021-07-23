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

#ifndef EZT_EZDATETIME_H
#define EZT_EZDATETIME_H

#include <ctime>
#include <chrono>
#include <cstdint>
#include <stdint-gcc.h>
#include "EZTools.h"

namespace EZDateTime {

    inline uint64_t CurrentTime_milliseconds() {
        return std::chrono::duration_cast<std::chrono::milliseconds>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    inline uint64_t CurrentTime_microseconds() {
        return std::chrono::duration_cast<std::chrono::microseconds>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    inline uint64_t CurrentTime_nanoseconds() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>
                (std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    }

    class LocalTime {
    public:
        LocalTime() {
            _now = std::time(nullptr);
            tm *p = localtime(&_now);
            _t = p;
            _year = _t->tm_year + 1900;
            _mon = _t->tm_mon + 1;
            _day = _t->tm_mday;
            _hour = _t->tm_hour;
            _min = _t->tm_min;
            _sec = _t->tm_sec;
            _gmOffset = _t->tm_gmtoff / 60 / 60;
            _millis = CurrentTime_milliseconds();
        }
        LocalTime(int year, int month, int day, int hour, int min, int sec, long gmOffset, uint64_t millis = 0) {
            _year = year;
            _mon = month;
            _day = day;
            _hour = hour;
            _min = min;
            _sec = sec;
            _gmOffset = gmOffset;
            _millis = millis;
        }
        ~LocalTime() = default;
        void update() {
            _now = std::time(nullptr);
            tm *p = localtime(&_now);
            _t = p;
            _year = _t->tm_year + 1900;
            _mon = _t->tm_mon + 1;
            _day = _t->tm_mday;
            _hour = _t->tm_hour;
            _min = _t->tm_min;
            _sec = _t->tm_sec;
            _gmOffset = _t->tm_gmtoff / 60 / 60;
            _millis = CurrentTime_milliseconds();
        }
        int year() { return _year; }
        int month() { return _mon; }
        int day() { return _day; }
        int hour() { return _hour; }
        int hourAMPM() {
            if (_hour == 0) { return 12; }
            if (_hour < 13) { return _hour; }
            else {
                return _hour - 12;
            }
        }
        bool isAM() { return _hour < 12; }
        int min() { return _min; }
        int sec() { return _sec; }
        uint64_t millis() { return _millis; }
        long gmOffset() { return _gmOffset; }
        EZTools::EZString zone() { return EZTools::EZString(_t->tm_zone); }
        int weekday() { return _t->tm_wday; }
        int yearday() { return _t->tm_yday; }
        int dst() { return _t->tm_isdst; }
        void swap(LocalTime localTime) {
            _year = localTime.year();
            _mon = localTime.month();
            _day = localTime.day();
            _hour = localTime.hour();
            _min = localTime.min();
            _sec = localTime.sec();
            _gmOffset = localTime.gmOffset();
        }

    private:
        int _year;
        int _mon;
        int _day;
        int _hour;
        int _min;
        int _sec;
        long _gmOffset;
        uint64_t _millis;
        time_t _now;
        tm *_t;
    };

    class CurrentDateTime {
    public:
//        enum MONTH_T{};
        CurrentDateTime() = default;
        ~CurrentDateTime() = default;
        EZTools::EZString ymdto() {
            LocalTime _dt;
            std::stringstream out;
            out << _dt.year() << "-" << std::setfill('0') << std::setw(2) << _dt.month() << "-" <<
                std::setfill('0') << std::setw(2) << _dt.day() << " " <<
                std::setfill('0') << std::setw(2) << _dt.hour() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.min() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.sec() << " " <<
                _dt.gmOffset() << ":00";
            return out.str();
        }
        EZTools::EZString ymdt() {
            LocalTime t;
            std::stringstream out;
            out << t.year() << "-" << std::setfill('0') << std::setw(2) << t.month() << "-" <<
                std::setfill('0') << std::setw(2) << t.day() << " " <<
                std::setfill('0') << std::setw(2) << t.hour() << ":" <<
                std::setfill('0') << std::setw(2) << t.min() << ":" <<
                std::setfill('0') << std::setw(2) << t.sec();
            return out.str();
        }
        EZTools::EZString stamp() {
            LocalTime _dt;
            std::stringstream out;
            out << _dt.year() << std::setfill('0') << std::setw(2) << _dt.month()
                << std::setfill('0') << std::setw(2) << _dt.day()
                << std::setfill('0') << std::setw(2) << _dt.hour()
                << std::setfill('0') << std::setw(2) << _dt.min()
                << std::setfill('0') << std::setw(2) << _dt.sec()
                << std::setfill('0') << std::setw(3) << _dt.millis();
            return out.str();
        }
        EZTools::EZString fancy() {
            LocalTime _dt;
            std::stringstream out;
            out << EZTools::intToDay(_dt.weekday()) << " " << EZTools::intToMonth(_dt.month()) << " "
                << _dt.day() << EZTools::dateSuffix(_dt.day()) << ", " << _dt.year() << " "
                << std::setfill('0') << std::setw(2) << _dt.hourAMPM() << ":"
                << std::setfill('0') << std::setw(2) << _dt.min() << " ";
            out << (_dt.isAM() ? "am" : "pm");
            return out.str();
        }
        EZTools::EZString time() {
            LocalTime _dt;
            std::stringstream out;
            out << std::setfill('0') << std::setw(2) << _dt.hourAMPM() << ":"
                << std::setfill('0') << std::setw(2) << _dt.min() << ":"
                << std::setfill('0') << std::setw(2) << _dt.sec() << " ";
            out << (_dt.isAM() ? "am" : "pm");
            return out.str();
        }
        EZTools::EZString date() {
            LocalTime _dt;
            std::stringstream out;
            out << _dt.year() << "-" << std::setfill('0') << std::setw(2) << _dt.month() << "-" <<
                std::setfill('0') << std::setw(2) << _dt.day();
            return out.str();
        }
        EZTools::EZString ymdtampm() {
            LocalTime _dt;
            std::stringstream out;
            out << _dt.year() << "-" << std::setfill('0') << std::setw(2) << _dt.month() << "-" <<
                std::setfill('0') << std::setw(2) << _dt.day() << " "
                << std::setfill('0') << std::setw(2) << _dt.hourAMPM() << ":"
                << std::setfill('0') << std::setw(2) << _dt.min() << ":"
                << std::setfill('0') << std::setw(2) << _dt.sec() << " ";
            out << (_dt.isAM() ? "am" : "pm");
            return out.str();
        }
    };

    inline time_t dateTimeToTimeT(int month, int day, int year, int hour, int min, int sec) {
        tm tmp = tm();
        tmp.tm_mday = day;
        tmp.tm_mon = month - 1;
        tmp.tm_year = year - 1900;
        tmp.tm_hour = hour;
        tmp.tm_min = min;
        tmp.tm_sec = sec;
        return mktime(&tmp);
    }

    class EZDT {
    public:
        EZDT() = default;
        explicit EZDT(EZTools::EZString ISO8601) {
            if (!ISO8601.empty()) {
                auto dsp = ISO8601.regexExtract(
                        "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([-+])([0-9]+):([0-9]+)");
                _year = dsp.at(1).asInt();
                _month = dsp.at(2).asInt();
                _day = dsp.at(3).asInt();
                _hour = dsp.at(4).asInt();
                _min = dsp.at(5).asInt();
                _sec = dsp.at(6).asInt();
                EZTools::EZString sign = dsp.at(7);
                EZTools::EZString off = dsp.at(8);
                std::stringstream ss;
                ss << sign << off;
                EZTools::EZString os = ss.str();
                _offset = os.asInt();
                LocalTime dt(_year, _month, _day, _hour, _min, _sec, _offset);
                _dt.swap(dt);
            } else {
                std::cout << "Date must be valid" << std::endl;
                exit(1);
            }
        }
        ~EZDT() = default;
        void ISO8601(EZTools::EZString iso8601) {
            if (!iso8601.empty()) {
                auto dsp = iso8601.regexExtract(
                        "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([-+])([0-9]+):([0-9]+)");
                _year = dsp.at(1).asInt();
                _month = dsp.at(2).asInt();
                _day = dsp.at(3).asInt();
                _hour = dsp.at(4).asInt();
                _min = dsp.at(5).asInt();
                _sec = dsp.at(6).asInt();
                EZTools::EZString sign = dsp.at(7);
                EZTools::EZString off = dsp.at(8);
                std::stringstream ss;
                ss << sign << off;
                EZTools::EZString os = ss.str();
                _offset = os.asInt();
                LocalTime dt(_year, _month, _day, _hour, _min, _sec, _offset);
                _dt.swap(dt);
            } else {
                std::cout << "Date must be valid" << std::endl;
                exit(1);
            }
        }
        LocalTime dateTime() { return _dt; }
        EZTools::EZString ymdto() {
            std::stringstream out;
            out << _dt.year() << "-" << std::setfill('0') << std::setw(2) << _dt.month() << "-" <<
                std::setfill('0') << std::setw(2) << _dt.day() << " " <<
                std::setfill('0') << std::setw(2) << _dt.hour() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.min() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.sec() << " " <<
                _offset << ":00";
            return out.str();
        }
        EZTools::EZString ymdt() {
            std::stringstream out;
            out << _dt.year() << "-" << std::setfill('0') << std::setw(2) << _dt.month() << "-" <<
                std::setfill('0') << std::setw(2) << _dt.day() << " " <<
                std::setfill('0') << std::setw(2) << _dt.hour() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.min() << ":" <<
                std::setfill('0') << std::setw(2) << _dt.sec();
            return out.str();
        }
        EZTools::EZString stamp() {
            std::stringstream out;
            out << _dt.year() << std::setfill('0') << std::setw(2) << _dt.month()
                << std::setfill('0') << std::setw(2) << _dt.day()
                << std::setfill('0') << std::setw(2) << _dt.hour()
                << std::setfill('0') << std::setw(2) << _dt.min()
                << std::setfill('0') << std::setw(2) << _dt.sec()
                << std::setfill('0') << std::setw(3) << _dt.millis();
            return out.str();
        }
        EZTools::EZString fancy() {
            std::stringstream out;
            out << EZTools::intToDay(_dt.weekday()) << " " << EZTools::intToMonth(_dt.month())
                << " " << _dt.day() << EZTools::dateSuffix(_dt.day()) << ", " << _dt.year() << " "
                << std::setfill('0') << std::setw(2) << _dt.hourAMPM() << ":"
                << std::setfill('0') << std::setw(2) << _dt.min() << " ";
            out << (_dt.isAM() ? "am" : "pm");
            return out.str();
        }
        EZTools::EZString plusHours(int hours) {
            std::stringstream ss;
            std::cout << "DERP" << std::endl;
            auto t = dateTimeToTimeT(_month, _day, _year, _hour, 0, 0);
            auto tm = localtime(&t);
            tm->tm_hour = tm->tm_hour + hours;
            ss << tm->tm_mon << "/" << tm->tm_mday << " ";
            if (tm->tm_hour > 12) {
                ss << tm->tm_hour - 12 << "pm";
            } else {
                ss << tm->tm_hour << "am";
            }
            return ss.str();
        }
    private:
        LocalTime _dt;
        int _year = 1000;
        int _month = 1;
        int _day = 1;
        int _hour = 0;
        int _min = 0;
        int _sec = 0;
        int _offset = 0;
    };

    class Timer {
    public:
        Timer() = default;
        ~Timer() = default;
        void start() {
            _start = std::chrono::high_resolution_clock::now();
        }
        void stop() {
            auto elapsed = std::chrono::high_resolution_clock::now() - _start;
            _microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
            _milliseconds = static_cast<float>(_microseconds) / 1000.0;
            _seconds = _milliseconds / 1000;
            _minutes = _seconds / 60;
            _hours = _minutes / 60;
        }
        long long asMicroseconds() { return _microseconds; }
        float asMillisenconds() { return _milliseconds; }
        float asSeconds() { return _seconds; }
        float asMinutes() { return  _minutes; }
        float asHours() { return _hours; }
    private:
        std::chrono::system_clock::time_point _start;
        long long _microseconds = 0;
        float _milliseconds = 0;
        float _seconds = 0;
        float _minutes = 0;
        float _hours = 0;
    };

}


#endif //EZT_EZDATETIME_H
