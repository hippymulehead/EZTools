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
 *  formatted output specifiers
 *  %AMPM shows am or pm depending on hour also sets to 12 hour format
 *  %Y 4 digit year
 *  %MO month number with zero fill
 *  %mo month number
 *  %D day number with zero fill
 *  %d day number
 *  %x day with suffix
 *  %H hour number with zero fill
 *  %h hour number
 *  %MI minute number with zero fill
 *  %mi minute number
 *  %S seconds number with zero fill
 *  %s second number
 *  %Z timezone
 *  %G gmtOffset with zero fill
 *  %g gmtOffset
 *  %WD weekday name
 *  %MN month name
 *  %mil milliseconds only if empty init
 *  %mic microseconds only if empty init
 *  %nan nanoseconds only if empty init
 */

//FIXME: The math functions do not work correctly unless both are in localtime.

#ifndef EZT_EZDATETIME_H
#define EZT_EZDATETIME_H

#pragma once

#include <ctime>
#include <chrono>
#include <cstdint>
#include <stdint-gcc.h>
#include "EZTools.h"

using namespace std::chrono;

namespace EZDateTime {

    const EZTools::EZString iso8601DateTime = "^([0-9]+)-([0-9]+)-([0-9]+)([T\\ ])([0-9]+):([0-9]+):([0-9]+)([Z\\-+])?"
                                              "([0-9]+)?:?([0-9]+)?$";
    const EZTools::EZString iso8601Time = "^(0[0-9]|1[0-9]|2[0-4]):(0[0-9]|1[0-9]|2[0-9]|3[0-9]|4[0-9]|5[0-9]):"
                                          "(0[0-9]|1[0-9]|2[0-9]|3[0-9]|4[0-9]|5[0-9])$";

    struct dt_t {
        EZTools::EZString year;
        EZTools::EZString month;
        EZTools::EZString day;
        bool hasT = false;
        EZTools::EZString hour;
        EZTools::EZString minute;
        EZTools::EZString second;
        bool isZ = false;
        EZTools::EZString offsetSign;
        EZTools::EZString offsetHour;
        EZTools::EZString offsetMinutes;
    };

    EZTools::EZReturn<dt_t> splitTime(EZTools::EZString time) {
        EZTools::EZReturn<dt_t> res;
        if (!time.regexMatch(iso8601Time)) {
            res.message("Failed to parse as date time");
            return res;
        }
        auto dtSplit = time.regexExtract(iso8601Time);
        if (dtSplit.at(1).asInt() > 0 && dtSplit.at(1).asInt() < 25) {
            res.data.hour = dtSplit.at(1);
        } else {
            res.message("Hour is out of bounds");
            return res;
        }
        if (dtSplit.at(2).asInt() >= 0 && dtSplit.at(2).asInt() < 60) {
            res.data.minute = dtSplit.at(2);
        } else {
            res.message("Minute is out of bounds");
            return res;
        }
        if (dtSplit.at(3).asInt() >= 0 && dtSplit.at(3).asInt() < 60) {
            res.data.second = dtSplit.at(3);
        } else {
            res.message("Second is out of bounds");
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    EZTools::EZReturn<dt_t> splitDateTime(EZTools::EZString dateTime) {
        EZTools::EZReturn<dt_t> res;
        if (!dateTime.regexMatch(iso8601DateTime)) {
            res.message("Failed to parse as date time");
            return res;
        }
        auto dtSplit = dateTime.regexExtract(iso8601DateTime);
        res.data.year = dtSplit.at(1);
        if (dtSplit.at(2).asInt() > 0 && dtSplit.at(2).asInt() < 13) {
            res.data.month = dtSplit.at(2);
        } else {
            res.message("Month is out of bounds");
            return res;
        }
        if (dtSplit.at(3).asInt() > 0 && dtSplit.at(3).asInt()
            <= EZTools::daysInMonth(res.data.month.asInt(),
                                    res.data.year.asInt())) {
            res.data.day = dtSplit.at(3);
        } else {
            res.message("Day is out of bounds");
            return res;
        }

        if (dtSplit.at(4) == "T") {
            res.data.hasT = true;
        }
        if (dtSplit.at(5).asInt() > 0 && dtSplit.at(5).asInt() < 25) {
            res.data.hour = dtSplit.at(5);
        } else {
            res.message("Hour is out of bounds");
            return res;
        }
        if (dtSplit.at(6).asInt() >= 0 && dtSplit.at(6).asInt() < 60) {
            res.data.minute = dtSplit.at(6);
        } else {
            res.message("Minute is out of bounds");
            return res;
        }
        if (dtSplit.at(7).asInt() >= 0 && dtSplit.at(7).asInt() < 60) {
            res.data.second = dtSplit.at(7);
        } else {
            res.message("Second is out of bounds");
            return res;
        }
        if (dtSplit.at(8) == "Z") {
            res.data.isZ = true;
            res.wasSuccessful(true);
            return res;
        }
        res.data.offsetSign = dtSplit.at(8);
        if (dtSplit.at(9).asInt() >= 0 && dtSplit.at(9).asInt() < 25) {
            res.data.offsetHour = dtSplit.at(9);
        } else {
            res.message("Offset Hour is out of bounds");
            return res;
        }
        if (dtSplit.at(10).asInt() >= 0 && dtSplit.at(10).asInt() < 60) {
            res.data.offsetMinutes = dtSplit.at(10);
        } else {
            res.message("Offset Minute is out of bounds");
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    class DateTime;
    class DateTime_t;
    struct dateTimeSplit_t {
        int Year;
        int Month;
        int Day;
        int Hour;
        int Minute;
        int Second;
    };

    struct timeSplit_t {
        int Year;
        int Month;
        int Day;
        int Hour;
        int Minute;
        int Second;
    };

    inline dateTimeSplit_t timeSplit(EZTools::EZString time);

    inline dateTimeSplit_t dateTimeSplit(EZTools::EZString dateTime) {
        dateTimeSplit_t t;
        if (dateTime.regexMatch("^([0-9]+)-([0-9]+)-([0-9]+) ([0-9]+):([0-9]+):([0-9]+)$")) {
            auto sp = dateTime.regexExtract("^([0-9]+)-([0-9]+)-([0-9]+) ([0-9]+):([0-9]+):([0-9]+)$");
            t.Year = sp.at(1).asInt();
            t.Month = sp.at(2).asInt();
            t.Day = sp.at(3).asInt();
            t.Hour = sp.at(4).asInt();
            t.Minute = sp.at(5).asInt();
            t.Second = sp.at(6).asInt();
        }
        return t;
    }

    enum TimePeriodTypes {
        DAY,
        MONTH,
        YEAR,
        HOUR,
        MIN,
        SEC
    };

    struct tp {
        TimePeriodTypes type;
        int value;
        tp(TimePeriodTypes Type, int Value) {
            type = Type;
            value = Value;
        }
    };

    class TimePeriod {
    public:
        TimePeriod() = default;
        explicit TimePeriod(std::vector<tp> TimePeriods) {
            for (auto& t : TimePeriods) {
                switch (t.type) {
                    case YEAR:
                        _year = t.value;
                        break;
                    case DAY:
                        _day += t.value;
                        break;
                    case HOUR:
                        _hour += t.value;
                        break;
                    case MIN:
                        _minute += t.value;
                        break;
                    case SEC:
                        _second += t.value;
                        break;
                    default:
                        break;
                }
            }
        }
        TimePeriod(int Years, int Days, int Hours, int Minutes, int Seconds) {
            _year = Years;
            _day = Days;
            _hour = Hours;
            _minute = Minutes;
            _second = Seconds;
        }
        ~TimePeriod() = default;
        [[nodiscard]] int years() const { return _year; }
        void years(int Years) { _year = Years; }
        [[nodiscard]] int days() const { return _day; }
        void days(int Days) { _day = Days; }
        [[nodiscard]] int hours() const { return _hour; }
        void hours(int Hours) { _hour = Hours; }
        [[nodiscard]] int minutes() const { return _minute; }
        void minutes(int Minutes) { _minute = Minutes; }
        [[nodiscard]] int seconds() const { return _second; }
        void seconds(int Seconds) { _second = Seconds; }
        [[nodiscard]] std::vector<tp> timePeriods() const { return _timePeriods; }
        [[nodiscard]] EZTools::EZString formatted(EZTools::EZString format) const {
            std::stringstream ss;
            EZTools::EZString output = format;
            output.regexReplace("%Y", _year);
            output.regexReplace("%D", _day);
            output.regexReplace("%H", _hour);
            output.regexReplace("%M", _minute);
            output.regexReplace("%S", _second);
            return output;
        }
        [[nodiscard]] int valueInSeconds() const {
            int v = 0;
            v = v + _year * 655 * 24 * 60 * 60;
            v = v + _day * 24 * 60 * 60;
            v = v + _hour * 60 * 60;
            v = v + _minute * 60;
            v = v + _second;
            return v;
        }
        friend bool operator< (const TimePeriod& lhs, const TimePeriod& rhs) {
            return lhs.valueInSeconds() < rhs.valueInSeconds();
        }
        friend bool operator<= (const TimePeriod& lhs, const TimePeriod& rhs) {
            return lhs.valueInSeconds() <= rhs.valueInSeconds();
        }
        friend bool operator> (const TimePeriod& lhs, const TimePeriod& rhs) {
            return lhs.valueInSeconds() > rhs.valueInSeconds();
        }
        friend bool operator>= (const TimePeriod& lhs, const TimePeriod& rhs) {
            return lhs.valueInSeconds() >= rhs.valueInSeconds();
        }
        friend bool operator!= (const TimePeriod& lhs, const TimePeriod& rhs) {
            return lhs.valueInSeconds() != rhs.valueInSeconds();
        }
    private:
        int _year = 0;
        int _day = 0;
        int _hour = 0;
        int _minute = 0;
        int _second = 0;
        std::vector<tp> _timePeriods;
    };

    TimePeriod timePeriodFromSeconds(std::chrono::seconds input_seconds) {
        TimePeriod t;
        using namespace std::chrono;
        typedef duration<int, std::ratio<86400>> days;
        auto d = duration_cast<days>(input_seconds);
        input_seconds -= d;
        auto h = duration_cast<hours>(input_seconds);
        input_seconds -= h;
        auto m = duration_cast<minutes>(input_seconds);
        input_seconds -= m;
        auto s = duration_cast<seconds>(input_seconds);

        auto dc = d.count();
        auto hc = h.count();
        auto mc = m.count();
        auto sc = s.count();
        if (dc) {
            t.days(d.count());
        }
        if (dc || hc) {
            t.hours(h.count());
        }
        if (dc || hc || mc) {
            t.minutes(m.count());
        }
        if (dc || hc || mc || sc) {
            t.seconds(s.count());
        }

        return t;
    }

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

    inline EZTools::EZString getCurrentMilliseconds() {
        unsigned long mi = CurrentTime_milliseconds();
        EZTools::EZString m(mi);
        EZTools::EZString mm = m.substr(m.size() - 3);
        return mm;
    }

    struct bds_t {
        EZTools::EZString milli;
        EZTools::EZString micro;
        EZTools::EZString nano;
    };

    inline bds_t getNanoTime() {
        bds_t pTime;
        unsigned long mi = CurrentTime_nanoseconds();
        EZTools::EZString m(mi);
        pTime.nano = m.substr(m.size() - 3);
        m.resize(m.size() - 3);
        pTime.micro = m.substr(m.size() - 3);
        m.resize(m.size() - 3);
        pTime.milli = m.substr(m.size() - 3);
        return pTime;
    }

    inline EZTools::EZString getCurrentMicrosseconds() {
        unsigned long mi = CurrentTime_microseconds();
        EZTools::EZString m(mi);
        EZTools::EZString mm = m.substr(m.size() - 3);
        return mm;
    }

    inline EZTools::EZString getCurrentNanoseconds() {
        unsigned long mi = CurrentTime_nanoseconds();
        EZTools::EZString m(mi);
        EZTools::EZString mm = m.substr(m.size() - 3);
        return mm;
    }

    class DateTime_t {
    public:
        DateTime_t() = default;
        ~DateTime_t() = default;
        void year(int Year) { _year = Year; }
        [[nodiscard]] int year() const { return _year; }
        void month(int Month) { _month = Month; }
        [[nodiscard]] int month() const { return _month; }
        void day(int Day) { _day = Day; }
        [[nodiscard]] int day() const { return _day; }
        void yearday(int YearDay) { _yearday = YearDay; }
        [[nodiscard]] int yearday() const { return _yearday; }
        void hour(int Hour) { _hour = Hour; }
        [[nodiscard]] int hour() const { return _hour; }
        [[nodiscard]] int hour12() const {
            return _hour < 12 ? _hour : _hour - 12;
        }
        void minute(int Minute) { _minute = Minute; }
        [[nodiscard]] int minute() const { return _minute; }
        void second(int Second) { _second = Second; }
        [[nodiscard]] int second() const { return _second; }
        void gmtoffset(long inSeconds) {
            _gmtoffset = inSeconds / 60 / 60;
        }
        [[nodiscard]] int gmtoffset() const { return _gmtoffset; }
        void weekday(int Weekday) { _weekday = Weekday; }
        [[nodiscard]] int weekday() const { return _weekday; }
        [[nodiscard]] bool isAM() const { return (_hour < 12); }
        [[nodiscard]] bool isPM() const { return (_hour > 11); }
        [[nodiscard]] bool isLeapYear() const {
            return (_year % 4) == 0;
        }
        [[nodiscard]] EZTools::EZString hourAMPM() const {
            return _hour < 12 ? "am" : "pm";
        }
        void milli(EZTools::EZString Millis) { _milli = Millis; }
        EZTools::EZString milli() { return _milli; }
        void micro(EZTools::EZString Micro) { _micro = Micro; }
        EZTools::EZString micro() { return _micro; }
        void nano(EZTools::EZString Nano) { _nano = Nano; }
        EZTools::EZString nano() { return _nano; }
        void timeZone(EZTools::EZString TimeZone) { _tz = TimeZone; }
        EZTools::EZString timeZone() { return _tz; }
    private:
        int _year = 0;
        int _month = 0;
        int _day = 0;
        int _hour = 0;
        int _minute = 0;
        int _second = 0;
        int _weekday = 0;
        int _yearday = 0;
        bool _isAM = false;
        int _gmtoffset = 0;
        EZTools::EZString _milli;
        EZTools::EZString _micro;
        EZTools::EZString _nano;
        EZTools::EZString _tz;
    };

    inline int DaysInMonth(int Month, bool isLeapYear) {
        switch (Month) {
            case 1:
                return 31;
            case 2:
                return isLeapYear ? 29 : 28;
            case 3:
                return 31;
            case 4:
                return 30;
            case 5:
                return 31;
            case 6:
                return 30;
            case 7:
                return 31;
            case 8:
                return 31;
            case 9:
                return 30;
            case 10:
                return 31;
            case 11:
                return 30;
            case 12:
                return 31;
            default:
                return -1;
        }
    }

    inline int DaysInYear(int Year, bool isLeapYear) {
        int d = 365;
        if (isLeapYear) { d++; }
        return d;
    }

    inline EZTools::EZString getTimeZone() {
        time_t t = time(nullptr);
        struct tm lt = {0};
        localtime_r(&t, &lt);
        return lt.tm_zone;
    }

    inline TimePeriod timeperiodFromString(EZTools::EZString tpString) {
        std::vector<EZDateTime::tp> tps;
        auto w = tpString.regexExtract("^([y,d,m,h,s][0-9]+)([y,d,m,h,s]*[0-9]*)"
                                       "([y,d,m,h,s]*[0-9]*)"
                                       "([y,d,m,h,s]*[0-9]*)([y,d,m,h,s]*[0-9]*)$");
        int counter = 0;
        for (auto& W : w) {
            if (counter > 0 && !W.empty()) {
                if (W.regexMatch("^y")) {
                    W.regexReplace("^y", "");
                    tps.emplace_back(EZDateTime::YEAR, W.asInt());
                } else if (W.regexMatch("^d")) {
                    W.regexReplace("^d", "");
                    tps.emplace_back(EZDateTime::DAY, W.asInt());
                } else if (W.regexMatch("^h")) {
                    W.regexReplace("^h", "");
                    tps.emplace_back(EZDateTime::HOUR, W.asInt());
                } else if (W.regexMatch("^m")) {
                    W.regexReplace("^m", "");
                    tps.emplace_back(EZDateTime::MIN, W.asInt());
                } else if (W.regexMatch("^s")) {
                    W.regexReplace("^s", "");
                    tps.emplace_back(EZDateTime::SEC, W.asInt());
                }
            }
            counter++;
        }
        TimePeriod tp(tps);
        return tp;
    }

    class DateTime {
    public:
        DateTime() {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(_tm.tm_year + 1900);
            _dt.month(_tm.tm_mon + 1);
            _dt.day(_tm.tm_mday);
            _dt.hour(_tm.tm_hour);
            _dt.minute(_tm.tm_min);
            _dt.second(_tm.tm_sec);
            auto pTime = getNanoTime();
            _dt.milli(pTime.milli);
            _dt.micro(pTime.micro);
            _dt.nano(pTime.nano);
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
        }
        DateTime(int Year, int Month, int Day, int Hour, int Minute, int Second) {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(Year);
            _dt.month(Month);
            _dt.day(Day);
            _dt.hour(Hour);
            _dt.minute(Minute);
            _dt.second(Second);
            _tm.tm_year = _dt.year() - 1900;
            _tm.tm_mon = _dt.month() - 1;
            _tm.tm_mday = _dt.day();
            _tm.tm_hour = _dt.hour();
            _tm.tm_min = _dt.minute();
            _tm.tm_sec = _dt.second();
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
        }
        DateTime(dateTimeSplit_t dateTime) {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(dateTime.Year);
            _dt.month(dateTime.Month);
            _dt.day(dateTime.Day);
            _dt.hour(dateTime.Hour);
            _dt.minute(dateTime.Minute);
            _dt.second(dateTime.Second);
            _tm.tm_year = _dt.year() - 1900;
            _tm.tm_mon = _dt.month() - 1;
            _tm.tm_mday = _dt.day();
            _tm.tm_hour = _dt.hour();
            _tm.tm_min = _dt.minute();
            _tm.tm_sec = _dt.second();
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
        }
        DateTime(timeSplit_t dateTime) {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(dateTime.Year);
            _dt.month(dateTime.Month);
            _dt.day(dateTime.Day);
            _dt.hour(dateTime.Hour);
            _dt.minute(dateTime.Minute);
            _dt.second(dateTime.Second);
            _tm.tm_year = _dt.year() - 1900;
            _tm.tm_mon = _dt.month() - 1;
            _tm.tm_mday = _dt.day();
            _tm.tm_hour = _dt.hour();
            _tm.tm_min = _dt.minute();
            _tm.tm_sec = _dt.second();
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
        }
        explicit DateTime(EZTools::EZString ISO8601) {
            if (!ISO8601.empty()) {
                std::vector<EZTools::EZString> d;
                if (ISO8601.regexMatch(iso8601DateTime)) {
                    if (ISO8601.regexMatch(" ")) {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+) ([0-9]+):([0-9]+):([0-9]+)");
                    } else if (ISO8601.regexMatch("Z")) {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([Z])");
                    } else {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([-+])([0-9]+):([0-9]+)");
                    }
                    _dt.year(d.at(1).asInt());
                    _dt.month(d.at(2).asInt());
                    _dt.day(d.at(3).asInt());
                    _dt.hour(d.at(4).asInt());
                    _dt.minute(d.at(5).asInt());
                    _dt.second(d.at(6).asInt());
                    std::stringstream ss;
                    if (d.size() > 7) {
                        if (d.at(7) == "Z") {
                            ss << "0";
                        } else {
                            ss << d.at(7) << d.at(8);
                        }
                    }
                    if (d.size() > 7) {
                        EZTools::EZString g(ss.str());
                        _tm.tm_gmtoff = g.asInt() * 60 * 60;
                    }
                    DateTime h;
                    _tm.tm_gmtoff = h.tm().tm_gmtoff;
                    _tm.tm_year = _dt.year() - 1900;
                    _tm.tm_mon = _dt.month() - 1;
                    _tm.tm_mday = _dt.day();
                    _tm.tm_hour = _dt.hour();
                    _tm.tm_min = _dt.minute();
                    _tm.tm_sec = _dt.second();
                    _dt.weekday(_tm.tm_wday);
                    _dt.gmtoffset(_tm.tm_gmtoff);
                    _dt.yearday(_tm.tm_yday);
                    _tm.tm_isdst = -1;
                    _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
                }
            }
        }
        ~DateTime() = default;
        void update() {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(_tm.tm_year + 1900);
            _dt.month(_tm.tm_mon + 1);
            _dt.day(_tm.tm_mday);
            _dt.hour(_tm.tm_hour);
            _dt.minute(_tm.tm_min);
            _dt.second(_tm.tm_sec);
            auto pTime = getNanoTime();
            _dt.milli(pTime.milli);
            _dt.micro(pTime.micro);
            _dt.nano(pTime.nano);
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
        }
        void update(int Year, int Month, int Day, int Hour, int Minute, int Second) {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(Year);
            _dt.month(Month);
            _dt.day(Day);
            _dt.hour(Hour);
            _dt.minute(Minute);
            _dt.second(Second);
            _tm.tm_year = _dt.year() - 1900;
            _tm.tm_mon = _dt.month() - 1;
            _tm.tm_mday = _dt.day();
            _tm.tm_hour = _dt.hour();
            _tm.tm_min = _dt.minute();
            _tm.tm_sec = _dt.second();
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
        }
        void update(dateTimeSplit_t dateTime) {
            _ctime = std::chrono::system_clock::now();
            auto timet  = std::chrono::system_clock::to_time_t(_ctime);
            _tm = *localtime(&timet);
            _dt.year(dateTime.Year);
            _dt.month(dateTime.Month);
            _dt.day(dateTime.Day);
            _dt.hour(dateTime.Hour);
            _dt.minute(dateTime.Minute);
            _dt.second(dateTime.Second);
            _tm.tm_year = _dt.year() - 1900;
            _tm.tm_mon = _dt.month() - 1;
            _tm.tm_mday = _dt.day();
            _tm.tm_hour = _dt.hour();
            _tm.tm_min = _dt.minute();
            _tm.tm_sec = _dt.second();
            _tm.tm_isdst = -1;
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _dt.timeZone(getTimeZone());
        }
        void update(EZTools::EZString ISO8601) {
            if (!ISO8601.empty()) {
                std::vector<EZTools::EZString> d;
                if (ISO8601.regexMatch(iso8601DateTime)) {
                    if (ISO8601.regexMatch(" ")) {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+) ([0-9]+):([0-9]+):([0-9]+)$");
                    } else if (ISO8601.regexMatch("Z")) {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([Z])");
                    } else {
                        d = ISO8601.regexExtract(
                                "^([0-9]+)-([0-9]+)-([0-9]+)T([0-9]+):([0-9]+):([0-9]+)([-+])([0-9]+):([0-9]+)$");
                    }
                    _dt.year(d.at(1).asInt());
                    _dt.month(d.at(2).asInt());
                    _dt.day(d.at(3).asInt());
                    _dt.hour(d.at(4).asInt());
                    _dt.minute(d.at(5).asInt());
                    _dt.second(d.at(6).asInt());
                    std::stringstream ss;
                    if (d.size() > 7) {
                        if (d.at(7) == "Z") {
                            ss << "0";
                        } else {
                            ss << d.at(7) << d.at(8);
                        }
                    }
                    if (d.size() > 7) {
                        EZTools::EZString g(ss.str());
                        _tm.tm_gmtoff = g.asInt() * 60 * 60;
                    }
                    DateTime h;
                    _tm.tm_gmtoff = h.tm().tm_gmtoff;
                    _tm.tm_year = _dt.year() - 1900;
                    _tm.tm_mon = _dt.month() - 1;
                    _tm.tm_mday = _dt.day();
                    _tm.tm_hour = _dt.hour();
                    _tm.tm_min = _dt.minute();
                    _tm.tm_sec = _dt.second();
                    _dt.weekday(_tm.tm_wday);
                    _dt.gmtoffset(_tm.tm_gmtoff);
                    _dt.yearday(_tm.tm_yday);
                    _tm.tm_isdst = -1;
                    _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
                }
            }
        }
        DateTime_t dateTime() { return _dt; }
        [[nodiscard]] struct tm tm() const { return _tm; }
        std::chrono::system_clock::time_point time_point() { return _ctime; }
        bool isLeapYear() { return _dt.isLeapYear(); }
        int daysInMonth() { return DaysInMonth(_dt.month(), this->isLeapYear()); }
        int daysToEndOfMonth() { return this->daysInMonth() - _dt.day(); }
        int daysToEndOfYear() {
            return DaysInYear(_dt.year(), this->isLeapYear()) - _dt.yearday();
        }
        EZTools::EZString formatted(EZTools::EZString format) {
            EZTools::EZString output = format;
            int h = _dt.hour();
            bool isTwelveHour = output.regexMatch("%AMPM");
            if (isTwelveHour) {
                if (_dt.isAM()) {
                    output.regexReplace("%AMPM", "am");
                } else {
                    output.regexReplace("%AMPM", "pm");
                }
                if (h > 12) {
                    h = h - 12;
                }
            }
            if (h == 0 && isTwelveHour) { h = 12; }
            output.regexReplace("%Y", EZTools::EZString(_dt.year()));
            std::stringstream ss;
            ss << std::setfill('0') << std::setw(2) << _dt.month();
            output.regexReplace("%MO", ss.str());
            ss.str(std::string());
            ss << std::setfill('0') << std::setw(2) << _dt.day();
            output.regexReplace("%D", ss.str());
            ss.str(std::string());
            ss << std::setfill('0') << std::setw(2) << h;
            output.regexReplace("%H", ss.str());
            ss.str(std::string());
            ss << std::setfill('0') << std::setw(2) << _dt.minute();
            output.regexReplace("%MI", ss.str());
            ss.str(std::string());
            ss << std::setfill('0') << std::setw(2) << _dt.second();
            output.regexReplace("%S", ss.str());
            output.regexReplace("%Z", _dt.timeZone());
            ss.str(std::string());
            EZTools::EZString gmt(_dt.gmtoffset());
            gmt.regexReplace("[+-]","");
            if (_dt.gmtoffset() < 0) {
                ss << "-" << std::setfill('0') << std::setw(2) << gmt.asInt() << ":00";
            } else {
                ss << "+" << std::setfill('0') << std::setw(2) << gmt.asInt() << ":00";
            }
            output.regexReplace("%G", ss.str());
            ss.str(std::string());
            output.regexReplace("%g", EZTools::EZString(_dt.gmtoffset()));
            output.regexReplace("%WD", EZTools::intToDay(_tm.tm_wday));
            output.regexReplace("%MN", EZTools::intToMonth(_dt.month()));
            ss.str(std::string());
            ss << _dt.day() << EZTools::dateSuffix(_dt.day());
            output.regexReplace("%x", ss.str());
            output.regexReplace("%mil", _dt.milli());
            output.regexReplace("%mic", _dt.micro());
            output.regexReplace("%nan", _dt.nano());
            output.regexReplace("%mo", _dt.month());
            output.regexReplace("%d", _dt.day());
            output.regexReplace("%h", _dt.hour());
            output.regexReplace("%mi", _dt.minute());
            output.regexReplace("%s", _dt.second());
            return output;
        }
        int hour() {
            return _tm.tm_hour;
        }
        int minute() {
            return _tm.tm_min;
        }
        int second() {
            return _tm.tm_sec;
        }
        int offset() {
            return _tm.tm_gmtoff;
        }
        EZTools::EZString fancyWithSeconds() {
            return formatted("%WD %MN %x, %Y %H:%MI:%S %AMPM");
        }
        EZTools::EZString fancy() {
            return formatted("%WD %MN %x, %Y %H:%MI %AMPM");
        }
        EZTools::EZString ymdt() {
            return formatted("%Y-%MO-%D %H:%MI:%S");
        }
        EZTools::EZString time() {
            return formatted("%H:%MI:%S");
        }
        EZTools::EZString timeAMPMNoSeconds() {
            return formatted("%H:%MI %AMPM");
        }
        EZTools::EZString ymdto() {
            return formatted("%Y-%MO-%D %H:%MI:%S %g");
        }
        EZTools::EZString iso8601() {
            return formatted("%Y-%MO-%DT%H:%MI:%S%G");
        }
        EZTools::EZString stamp() {
            return formatted("%Y%MO%D%H%MI%S%mil%mic%nan");
        }
        EZTools::EZString ymdtWithMilliSeconds() {
            return formatted("%Y-%MO-%D %H:%MI:%S.%mil");
        }
        EZTools::EZString ymdtWithMicroSeconds() {
            return formatted("%Y-%MO-%D %H:%MI:%S.%mil.%mic");
        }
        EZTools::EZString ymdtWithNanoSeconds() {
            return formatted("%Y-%MO-%D %H:%MI:%S.%mil.%mic.%nan");
        }
        EZTools::EZString dowString() {
            return EZTools::intToDay(_dt.weekday());
        }
        int dowInt() const { return _dt.weekday(); }
        EZTools::EZString monthString() { return EZTools::intToMonth(_dt.month()); }
        int monthInt() { return _dt.month(); }
        int year() { return _dt.year(); }
        int day() { return _dt.day(); }
        bool yearIsEven() {
            return (_dt.year() % 2 == 0);
        }
        bool isYearIn(std::vector<int> years) {
            for (auto& y: years) {
                if (y == _dt.year()) {
                    return true;
                }
            }
            return false;
        }
        bool isDayIn(std::vector<int> days) {
            for (auto& d: days) {
                if (d == _dt.day()) {
                    return true;
                }
            }
            return false;
        }
        bool isMonthIn(std::vector<int> months) {
            for (auto& m: months) {
                if (m == _dt.month()) {
                    return true;
                }
            }
            return false;
        }

        friend bool operator< (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime < rhs._ctime;
        }
        friend bool operator<= (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime <= rhs._ctime;
        }
        friend bool operator> (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime > rhs._ctime;
        }
        friend bool operator>= (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime >= rhs._ctime;
        }
        friend bool operator== (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime == rhs._ctime;
        }
        friend bool operator!= (const DateTime& lhs, const DateTime& rhs) {
            return lhs._ctime != rhs._ctime;
        }
        friend TimePeriod operator-(DateTime lhs, const DateTime & rhs) {
            auto epoch1 = rhs._ctime.time_since_epoch();
            auto value1 = std::chrono::duration_cast<std::chrono::seconds>(epoch1);
            auto epoch2 = lhs._ctime.time_since_epoch();
            auto value2 = std::chrono::duration_cast<std::chrono::seconds>(epoch2);
            auto an = std::chrono::duration_cast<std::chrono::seconds>(value1 - value2);
            return timePeriodFromSeconds(an);
        }
        DateTime& operator-=(const TimePeriod & rhs) {
            if (rhs.years() > 0) {
                _tm.tm_year -= rhs.years();
            }
            if (rhs.days() > 0) {
                _tm.tm_mday -= rhs.days();
            }
            if (rhs.hours() > 0) {
                _tm.tm_hour -= rhs.hours();
            }
            if (rhs.minutes() > 0) {
                _tm.tm_min -= rhs.minutes();
            }
            if (rhs.seconds() > 0) {
                _tm.tm_sec -= rhs.seconds();
            }
            updateFromInternalTM();
            return *this;
        }
        friend DateTime operator-(DateTime lhs, const TimePeriod & rhs) {
            lhs -= rhs;
            return lhs;
        }
        DateTime& operator+=(const TimePeriod & rhs) {
            if (rhs.years() > 0) {
                _tm.tm_year += rhs.years();
            }
            if (rhs.days() > 0) {
                _tm.tm_mday += rhs.days();
            }
            if (rhs.hours() > 0) {
                _tm.tm_hour += rhs.hours();
            }
            if (rhs.minutes() > 0) {
                _tm.tm_min += rhs.minutes();
            }
            if (rhs.seconds() > 0) {
                _tm.tm_sec += rhs.seconds();
            }
            updateFromInternalTM();
            return *this;
        }
        friend DateTime operator+(DateTime lhs, const TimePeriod & rhs) {
            lhs += rhs;
            return lhs;
        }
        void makeLocal() {
            DateTime now;
            EZTools::EZString os(now.dateTime().gmtoffset() - this->dateTime().gmtoffset());
            bool isNeg = false;
            if (os.regexMatch("-")) {
                isNeg = true;
            }
            os.regexReplace("[+-]", "");
            auto tp = EZDateTime::timeperiodFromString("h"+os);
            if (isNeg) {
                *this -= tp;
            } else {
                *this += tp;
            }
        }

    private:
        std::chrono::system_clock::time_point _ctime;
        DateTime_t _dt;
        struct tm _tm;
        void updateFromInternalTM() {
            time_t _time_t = mktime(&_tm);
            _dt.year(_tm.tm_year + 1900);
            _dt.month(_tm.tm_mon + 1);
            _dt.day(_tm.tm_mday);
            _dt.hour(_tm.tm_hour);
            _dt.minute(_tm.tm_min);
            _dt.second(_tm.tm_sec);
            _dt.weekday(_tm.tm_wday);
            _dt.gmtoffset(_tm.tm_gmtoff);
            _dt.yearday(_tm.tm_yday);
            _ctime = std::chrono::system_clock::from_time_t(std::mktime(&_tm));
        }
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

    inline dateTimeSplit_t setTimeForDay(DateTime ezdatetime, EZTools::EZString time) {
        dateTimeSplit_t t;
        if (time.regexMatch("^([0-9]+):([0-9]+):([0-9]+)$")) {
            auto sp = time.regexExtract("^([0-9]+):([0-9]+):([0-9]+)$");
            t.Year = ezdatetime.year();
            t.Month = ezdatetime.monthInt();
            t.Day = ezdatetime.day();
            t.Hour = sp.at(1).asInt();
            t.Minute = sp.at(2).asInt();
            t.Second = sp.at(3).asInt();
        }
        return t;
    }

}


#endif //EZT_EZDATETIME_H
