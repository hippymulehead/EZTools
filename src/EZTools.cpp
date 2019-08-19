//
// Created by mromans on 5/24/19.
//

#include "EZTools.h"

EZTools::EZString::EZString() : string() {}
EZTools::EZString::EZString(const string &s) : string(s) {}
EZTools::EZString::EZString(const string &s, size_t n) : string(s, n) {}
EZTools::EZString::EZString(const char *s, size_t n) : string(s, n) {}
EZTools::EZString::EZString(const char *s) : string(s) {}
EZTools::EZString::EZString(size_t n, char c) : string(n, c) {}
EZTools::EZString::EZString(int i) {
    stringstream ss;
    ss << i;
    *this = EZString(ss.str());
}
EZTools::EZString::EZString(float f) {
    stringstream ss;
    ss << f;
    *this = EZString(ss.str());
}
EZTools::EZString::EZString(double d) {
    stringstream ss;
    ss << d;
    *this = EZString(ss.str());
}
EZTools::EZString::EZString(long l) {
    stringstream ss;
    ss << l;
    *this = EZString(ss.str());
}
EZTools::EZString::EZString(unsigned long l) {
    stringstream ss;
    ss << l;
    *this = EZString(ss.str());
}
template<class InputIterator>
EZTools::EZString::EZString(InputIterator first, InputIterator last) : string(first, last) {}
void EZTools::EZString::join(EZTools::EZString separator, vector<EZTools::EZString> &list) {
    assign(list[0]);
    for(uint i = 1; i < list.size(); i++) {
        append(separator);
        append(list[i]);
    }
}
unsigned long int EZTools::EZString::count(EZTools::EZString s) const {
    uint total = 0;
    size_t end,pos = 0;
    while(true) {
        end = find(s,pos);
        if(end == string::npos)
            break;
        pos = end + s.size();
        total++;
    }
    return total;
}
EZTools::EZString &EZTools::EZString::lower() {
    for(char & i : *this) {
        if(i >= 'A' && i <= 'Z') {
            i += ('a' - 'A');
        }
    }
    return *this;
}
EZTools::EZString &EZTools::EZString::upper() {
    for(char & i : *this) {
        if(i >= 'a' && i <= 'z') {
            i -= ('a' - 'A');
        }
    }
    return *this;
}
bool EZTools::EZString::isLower() const {
    if (length() == 0)
        return false;
    else {
        for(char i : *this) {
            if(i >= 'A' && i <= 'Z') {
                return false;
            }
        }
    }
    return true;
}
bool EZTools::EZString::isUpper() const {
    if (length() == 0)
        return false;
    else {
        for(char i : *this) {
            if(i >= 'a' && i <= 'z') {
                return false;
            }
        }
    }
    return true;
}
bool EZTools::EZString::isInt() const {
    int Result;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> Result) ) {
        Result = 0;
    }
    return Result != 0;
}
int EZTools::EZString::asInt() const {
    if (this->isInt()) {
        return stoi(this->str());
    }
    return 0;
}
bool EZTools::EZString::isLong() const {
    long Result;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> Result) ) {
        Result = 0;
    }
    return Result != 0;
}
int EZTools::EZString::asLong() const {
    if (this->isInt()) {
        return stol(this->str());
    }
    return 0;
}
bool EZTools::EZString::isUnsignedLong() const {
    unsigned long Result;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> Result) ) {
        Result = 0;
    }
    return Result != 0;
}
unsigned long EZTools::EZString::asUnsignedLong() const {
    if (this->isUnsignedLong()) {
        return stoul(this->str());
    }
    return 0;
}
bool EZTools::EZString::isUnsignedLongLong() const {
    unsigned long Result;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> Result) ) {
        Result = 0;
    }
    return Result != 0;
}
unsigned long long EZTools::EZString::asUnsignedLongLong() const {
    return this->isUnsignedLongLong() ? stoull(this->str()) : 0;
}
bool EZTools::EZString::isFloat() const {
    int Result = 1;
    float f;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> f) ) {
        Result = 0;
    }
    return Result != 0;
}
float EZTools::EZString::asFloat() const {
    if (this->isFloat()) {
        return stof(this->str());
    }
    return 0.0;
}
bool EZTools::EZString::isDouble() const {
    int Result = 1;
    double f;
    if ((int)this->str()[0] < 10) {
        return false;
    }
    istringstream convert(this->str());
    if ( !(convert >> f) ) {
        Result = 0;
    }
    return Result != 0;
}
float EZTools::EZString::asDouble() const {
    if (this->isDouble()) {
        return stod(this->str());
    }
    return 0.0;
}
bool EZTools::EZString::isAlpha() const {
    if (length() == 0)
        return false;
    else {
        for(char i : *this) {
            if((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != ' ') {
                return false;
            }
        }
    }
    return true;
}
string EZTools::EZString::str() const {
    return *this;
}
bool EZTools::EZString::regexMatch(const EZTools::EZString &regexString) {
    EZString str = *this;
    Poco::RegularExpression regex(regexString);
    Poco::RegularExpression::MatchVec matchVec;
    return regex.match(str, 0, matchVec) != 0;
}
vector<EZTools::EZString> EZTools::EZString::regexExtract(EZTools::EZString regexString) const {
    EZString str = *this;
    RegularExpression regex(regexString);
    vector<string> retval;
    vector<EZString> ret;
    regex.split(str, retval, 0);
    for (auto &s : retval) {
        ret.push_back(s);
    }
    return ret;
}
EZTools::EZString& EZTools::EZString::sha256(EZTools::EZString secretKey) {
    Poco::HMACEngine<EZCrypto::SHA256Engine> hmac{secretKey};
    hmac.update(*this);
    stringstream ss;
    ss << Poco::DigestEngine::digestToHex(hmac.digest());
    *this = ss.str();
    return *this;
}
vector<EZTools::EZString> EZTools::EZString::split(EZTools::EZString delimiter) const {
    vector<EZString> ret;
    size_t pos = 0;
    string s = *this;
    while ((pos = s.find(delimiter)) != string::npos) {
        ret.emplace_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    ret.emplace_back(s);
    return ret;
}
vector<EZTools::EZString> EZTools::EZString::splitAtLast(EZTools::EZString &delimiter) const {
    auto splitpos = this->find_last_of(delimiter);
    vector<EZString> ret;
    ret.emplace_back(this->substr(0, splitpos));
    ret.emplace_back(this->substr(splitpos + 1, (this->size() - (splitpos + 1))));
    return ret;
}
EZTools::EZString &EZTools::EZString::regexReplace(EZTools::EZString regexString, EZTools::EZString replaceWith) {
    RegularExpression regex(regexString);
    regex.subst(*this, replaceWith, RegularExpression::RE_GLOBAL);
    return *this;
}
EZTools::EZString &EZTools::EZString::chomp() {
    auto pos = this->find("\n");
    *this = this->substr(0,pos);
    return *this;
}
EZTools::EZString &EZTools::EZString::chopAtLast(EZTools::EZString delimiter) {
    auto pos = this->find_last_of(delimiter) - delimiter.length();
    *this = this->substr(0,pos);
    return *this;
}
EZTools::EZString &EZTools::EZString::removeExtraSpaces() {
    while (this->regexMatch("  ")) {
        this->regexReplace("[ ][ ]", " ");
    }
    return *this;
}
EZTools::EZString &EZTools::EZString::removeOpeningSpaces() {
    while (this->regexMatch("^ ")) {
        this->regexReplace("^ ", "");
    }
    return *this;
}
EZTools::EZString &EZTools::EZString::removeTrailingSpaces() {
    while (this->regexMatch(" $")) {
        this->regexReplace(" $", "");
    }
    return *this;
}
EZTools::EZString &EZTools::EZString::asHex() {
    stringstream ss;
    EZString st = *this;
    for (auto i = 0; i < st.length(); i++) {
        ss << hex << (int) st[i];
    }
    *this = ss.str();
    return *this;
}
EZTools::EZString &EZTools::EZString::cat(EZTools::EZString value) {
    stringstream ss;
    ss << *this << value;
    *this = ss.str();
    return *this;
}
EZTools::EZString &
EZTools::EZString::replaceAll(const EZTools::EZString &from, const EZTools::EZString &to) {
    size_t start_pos = 0;
    while((start_pos = this->find(from, start_pos)) != std::string::npos) {
        this->replace(start_pos, from.length(), to);
        start_pos += to.length();
    }
    return *this;
}
template<class T>
void EZTools::EZKeyValueMap<T>::insert(EZString key, T value) {
    _map.insert(pair<EZString, T>(key, value));
}
template<class T>
T EZTools::EZKeyValueMap<T>::valueForKey(EZTools::EZString key) {
    return _map.find(key)->second;
}
template<class T>
void EZTools::EZKeyValueMap<T>::removeKey(EZTools::EZString key) {
    _map.erase(key);
}
EZTools::EZDBCreds::EZDBCreds(EZTools::EZString address, EZTools::EZString databaseName, EZTools::EZString username,
                              EZTools::EZString password) {
    _address = address;
    _databaseName = databaseName;
    _username = username;
    _password = password;
}

EZTools::EZError::EZError(EZTools::EZString errorMessage, bool isSuccess) {
    _errorMessage = errorMessage;
    _isSuccess = isSuccess;
}

EZDateTimeFunctions::EZSystemTime::EZSystemTime() {
    EZTools::EZString ccc;
    std::time_t ct = std::time(nullptr);
    ccc = ctime(&ct);
    ccc = ccc.regexReplace("  "," ");
    ccc = ccc.regexReplace("\n","");
    vector<EZTools::EZString> cc = ccc.split(" ");
    _weekday = cc.at(0);
    _month = cc.at(1);
    _monthday = cc.at(2);
    if (_monthday.length() < 2) {
        stringstream s;
        s << "0" << _monthday;
        _monthday = s.str();
    }
    _time = cc.at(3);
    vector<EZTools::EZString> tnsv = _time.split(":");
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

void EZDateTimeFunctions::EZSystemTime::update() {
    EZTools::EZString ccc;
    std::time_t ct = std::time(nullptr);
    ccc = ctime(&ct);
    ccc = ccc.regexReplace("  "," ");
    ccc = ccc.regexReplace("\n","");
    vector<EZTools::EZString> cc = ccc.split(" ");
    _weekday = cc.at(0);
    _month = cc.at(1);
    _monthday = cc.at(2);
    if (_monthday.length() < 2) {
        stringstream s;
        s << "0" << _monthday;
        _monthday = s.str();
    }
    _time = cc.at(3);
    vector<EZTools::EZString> tnsv = _time.split(":");
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

EZTools::EZString EZDateTimeFunctions::EZSystemTime::mdy() const {
    stringstream out;
    out << _monthNumber << _dateSep << _monthday << _dateSep << _year;
    EZTools::EZString oo(out.str());
    return oo;
}

EZTools::EZString EZDateTimeFunctions::EZSystemTime::ymd() const {
    stringstream out;
    out << _year << _dateSep << _monthNumber << _dateSep << _monthday;
    EZTools::EZString oo(out.str());
    return oo;
}

EZTools::EZString EZDateTimeFunctions::EZSystemTime::ymdt() const {
    stringstream out;
    out << _year << _dateSep << _monthNumber << _dateSep << _monthday << " " << _time;
    EZTools::EZString oo(out.str());
    return oo;
}

EZTools::EZString EZDateTimeFunctions::EZSystemTime::mdyt() const {
    stringstream out;
    out << _monthNumber << _dateSep << _monthday << _dateSep << _year << " " << _time;
    EZTools::EZString oo(out.str());
    return oo;
}

EZTools::EZString EZDateTimeFunctions::EZSystemTime::fancy() const {
    stringstream f;
    f << _month << " " << _monthday << ", " << _year << " " << _timeNoSec;
    return f.str();
}

EZTools::EZString EZDateTimeFunctions::EZSystemTime::fancyWithWeekDay() const {
    stringstream f;
    vector<EZTools::EZString> sp = _timeNoSec.split(":");
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

EZTools::EZString EZDateTimeFunctions::EZSystemTime::dtstamp() const {
    stringstream f;
    f << _year << _monthNumber << _monthday << _time;
    EZTools::EZString ff = f.str();
    return ff.regexReplace(":","");
}

void EZDateTimeFunctions::EZTimer::start() {
    _start.update();
}

void EZDateTimeFunctions::EZTimer::stop() {
    Timestamp::TimeDiff diff = _start.elapsed();
    _microseconds = diff;
    _milliseconds = static_cast<float>(_microseconds) / 1000.0;
    _seconds = _milliseconds / 1000;
    _minutes = _seconds / 60;
    _hours = _minutes / 60;
}

string EZDateTimeFunctions::EZTimestamp::now() {
    _now = Timestamp();
    return DateTimeFormatter::format(_now, "%Y-%m-%d %H:%M:%S");
}

void EZDateTimeFunctions::EZDateTime::setNWSDateTime(EZTools::EZString nwsdatetime) {
    if (!nwsdatetime.empty()) {
        // 2019-05-22T15:00:00-05:00
        vector<EZTools::EZString> dtbreakout;
        // Poco regex is broken.  Does not accept [0-9]
        dtbreakout = nwsdatetime.regexExtract("^(....)-(..)-(..)T(..):(..):(..)([+]|-)(..):(..)");
        int _year = dtbreakout.at(1).asInt();
        int _month = dtbreakout.at(2).asInt();
        int _day = dtbreakout.at(3).asInt();
        int _hour = dtbreakout.at(4).asInt();
        int _minute = dtbreakout.at(5).asInt();
        int _second = dtbreakout.at(6).asInt();
        _dt.assign(_year, _month, _day, _hour, _minute, _second, 0, 0);
    }
}

void EZDateTimeFunctions::EZDateTime::setMySQL(EZTools::EZString mysqldatetime) {
    if (!mysqldatetime.empty()) {
        // 2019-05-22T15:00:00-05:00
        vector<EZTools::EZString> dtbreakout;
        // Poco regex is broken.  Does not accept [0-9]
        dtbreakout = mysqldatetime.regexExtract("^(....)-(..)-(..) (..):(..):(..)");
        int _year = dtbreakout.at(1).asInt();
        int _month = dtbreakout.at(2).asInt();
        int _day = dtbreakout.at(3).asInt();
        int _hour = dtbreakout.at(4).asInt();
        int _minute = dtbreakout.at(5).asInt();
        int _second = dtbreakout.at(6).asInt();
        _dt.assign(_year, _month, _day, _hour, _minute, _second, 0, 0);
    }
}

EZRandom::EZRand::EZRand() {
    _rdev.seed();
}
