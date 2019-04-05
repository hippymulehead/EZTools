//
// Created by mromans on 4/5/19.
//

#include "../include/EZSystemTime.h"

void EZSystemTime::update() {
    EZString ccc;
    std::time_t ct = std::time(nullptr);
    ccc = ctime(&ct);
    ccc = ccc.regexReplace("  "," ");
    ccc = ccc.regexReplace("\n","");
    vector<EZString> cc = ccc.splitBy(" ");
    _weekday = cc.at(0);
    _month = cc.at(1);
    _monthday = cc.at(2);
    if (_monthday.length() < 2) {
        stringstream s;
        s << "0" << _monthday;
        _monthday = s.str();
    }
    _time = cc.at(3);
    vector<EZString> tnsv = _time.splitBy(":");
    stringstream tns;
    tns << tnsv.at(0) << ":" << tnsv.at(1);
    _timeNoSec = tns.str();
    _year = cc.at(4);
    if (_month == "Jan") { _monthNumber = "01"; }
    if (_month == "Feb") { _monthNumber = "02"; }
    if (_month == "Mar") { _monthNumber = "03"; }
    if (_month == "Apr") { _monthNumber = "04"; }
    if (_month == "May") { _monthNumber = "05"; }
    if (_month == "Jun") { _monthNumber = "06"; }
    if (_month == "Jul") { _monthNumber = "07"; }
    if (_month == "Aug") { _monthNumber = "08"; }
    if (_month == "Sep") { _monthNumber = "09"; }
    if (_month == "Oct") { _monthNumber = "10"; }
    if (_month == "Nov") { _monthNumber = "11"; }
    if (_month == "Dec") { _monthNumber = "12"; }
}

EZString EZSystemTime::mdy() const {
    stringstream out;
    out << _monthNumber << _dateSep << _monthday << _dateSep << _year;
    EZString oo(out.str());
    return oo;
}

EZString EZSystemTime::ymd() const {
    stringstream out;
    out << _year << _dateSep << _monthNumber << _dateSep << _monthday;
    EZString oo(out.str());
    return oo;
}

EZString EZSystemTime::ymdt() const {
    stringstream out;
    out << _year << _dateSep << _monthNumber << _dateSep << _monthday << " " << _time;
    EZString oo(out.str());
    return oo;
}

EZString EZSystemTime::mdyt() const {
    stringstream out;
    out << _monthNumber << _dateSep << _monthday << _dateSep << _year << " " << _time;
    EZString oo(out.str());
    return oo;
}

EZString EZSystemTime::fancy() const {
    stringstream f;
    f << _month << " " << _monthday << ", " << _year << " " << _timeNoSec;
    return f.str();
}

EZString EZSystemTime::fancyWithWeekDay() const {
    stringstream f;
    vector<EZString> sp = _timeNoSec.splitBy(":");
    int _hour = sp.at(0).asInt();
    if (_hour > 12) {
        _hour = _hour - 12;
        stringstream s;
        if (_hour < 10) {
            s << "0" << _hour;
        } else {
            s << sp.at(0);
        }
        f << _weekday << ", " << _month << " " << _monthday << ", " << _year << " " << s.str() << ":" << sp.at(1) << " PM";
    } else {
        f << _weekday << ", " << _month << " " << _monthday << ", " << _year << " " << _timeNoSec << " AM";
    }
    return f.str();
}

EZString EZSystemTime::dtstamp() const {
    stringstream f;
    f << _year << _monthNumber << _monthday << _time;
    EZString ff = f.str();
    return ff.regexReplace(":","");
}