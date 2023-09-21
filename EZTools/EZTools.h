/*
EZTools Copyright (c) 2017-2023, Michael Romans of Romans Audio
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

#ifndef EZTOOLS_EZTOOLS_H
#define EZTOOLS_EZTOOLS_H

//#pragma once

#include <iostream>
#include <vector>
#include <map>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <limits>
#include <regex>

namespace EZTools {

    class TEST_RETURN {
    public:
        explicit TEST_RETURN(std::string TestName) {
            _testName = TestName;
            std::stringstream ss;
            ss << std::endl;
            ss << "-----------------------------------------" << std::endl;
            ss << TestName << " - TESTS" << std::endl;
            _output = ss.str();
        }
        void message(std::string Message) {
            _message = Message;
        }
        std::string message() {
            std::stringstream ss;
            ss << _testName << ": " << _functionTest << ": " << _message;
            return ss.str();
        }
        void functionTest(std::string f) {
            _functionTest = f;
        }
        std::string output() { return _output; }
        void output(std::string addText) {
            _output = _output + addText;
        }
        std::string testName() { return _testName; }
        void wasSuccessful(bool success) { _success = success; }
        bool wasSuccessful() { return _success; }
        std::string errorOutput() {
            std::stringstream ss;
            ss << _output << std::endl;
            ss << message() << std::endl;
            ss << "[[ ** FAILED ** ]]" << std::endl;
            return ss.str();
        }
        std::string successOutput() {
            std::stringstream ss;
            ss << _output << std::endl;
            ss << "[[ ** PASSED ** ]]" << std::endl;
            return ss.str();
        }
        std::string out() {
            if (_success) {
                return successOutput();
            } else {
                return errorOutput();
            }
        }
    private:
        std::string _output;
        std::string _functionTest;
        std::string _testName;
        std::string _message;
        bool _success = false;
    };

    enum CLOSE {
        SUCCESS, FAIL, ERROR
    };

    /*
     * EZTools::EZString
     * EZString is a drop in replacement for std::string with a lot of methods added on.  Conversion, RegEx, init
     * just to name a few.  This became an any for JSON but in a much more simple way than most.
     */
    class EZString : public std::string {
    public:
        EZString() : std::string() {}
        EZString(const std::string &s) : std::string(s) {}
        EZString(const std::string &s, size_t n) : std::string(s, n) {}
        EZString(const char *s, size_t n) : std::string(s, n) {}
        EZString(const char *s) : std::string(s) {}
        EZString(size_t n, char c) : std::string(n, c) {}
        EZString(int i) {
            std::stringstream ss;
            ss << i;
            *this = EZString(ss.str());
        }
        EZString(float f) {
            std::stringstream ss;
            ss << std::setprecision(std::numeric_limits<float>::digits10+1);
            ss << f;
            *this = EZString(ss.str());
        }
        EZString(double d) {
            std::stringstream ss;
            ss << std::setprecision(std::numeric_limits<float>::digits10+1);
            ss << d;
            *this = EZString(ss.str());
        }
        EZString(unsigned int l) {
            std::stringstream ss;
            ss << l;
            *this = EZString(ss.str());
        }
        EZString(long l) {
            std::stringstream ss;
            ss << l;
            *this = EZString(ss.str());
        }
        EZString(unsigned long l) {
            std::stringstream ss;
            ss << l;
            *this = EZString(ss.str());
        }
        EZString(unsigned long long l) {
            std::stringstream ss;
            ss << l;
            *this = EZString(ss.str());
        }
        template<class InputIterator>
        EZString(InputIterator first, InputIterator last) : std::string(first, last) {}

        void join(EZString separator, std::vector<EZString> &list) {
            assign(list[0]);
            for (uint i = 1; i < list.size(); i++) {
                append(separator);
                append(list[i]);
            }
        }
        unsigned long int count(EZString s) const {
            uint total = 0;
            size_t end, pos = 0;
            while (true) {
                end = find(s, pos);
                if (end == std::string::npos)
                    break;
                pos = end + s.size();
                total++;
            }
            return total;
        }
        EZString &lower() {
            for (char &i : *this)
                if (i >= 'A' && i <= 'Z') {
                    i += ('a' - 'A');
                };
            return *this;
        }
        EZString &upper() {
            for (char &i : *this) {
                if (i >= 'a' && i <= 'z') {
                    i -= ('a' - 'A');
                }
            }
            return *this;
        }
        bool isLower() const {
            if (length() == 0)
                return false;
            else {
                for (char i : *this) {
                    if (i >= 'A' && i <= 'Z') {
                        return false;
                    }
                }
            }
            return true;
        }
        bool isUpper() const {
            if (length() == 0)
                return false;
            else {
                for (char i : *this) {
                    if (i >= 'a' && i <= 'z') {
                        return false;
                    }
                }
            }
            return true;
        }
        bool isInt() const {
            int Result;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> Result)) {
                Result = 0;
            }
            return Result != 0;
        }
        int asInt() const {
            if (this->isInt()) {
                int i = 0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isLong() const {
            long Result;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> Result)) {
                Result = 0;
            }
            return Result != 0;
        }
        int asLong() const {
            if (this->isInt()) {
                long i = 0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isUnsignedLong() const {
            unsigned long Result;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> Result)) {
                Result = 0;
            }
            return Result != 0;
        }
        bool isUnsigned() const {
            unsigned long Result;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> Result)) {
                Result = 0;
            }
            return Result != 0;
        }
        unsigned asUnsigned() const {
            if (this->isUnsigned()) {
                unsigned long i = 0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0;
        }
        unsigned long asUnsignedLong() const {
            if (this->isUnsignedLong()) {
                unsigned long i = 0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isUnsignedLongLong() const {
            unsigned long Result;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> Result)) {
                Result = 0;
            }
            return Result != 0;
        }
        unsigned long long asUnsignedLongLong() const {
            unsigned long long i = 0;
            std::istringstream(this->str()) >> i;
            return i;
        }
        bool isFloat() const {
            int Result = 1;
            float f;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> f)) {
                Result = 0;
            }
            return Result != 0;
        }
        float asFloat() const {
            if (this->isFloat()) {
                float i = 0.0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0.0;
        }
        bool isDouble() const {
            int Result = 1;
            double f;
            if ((int) this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if (!(convert >> f)) {
                Result = 0;
            }
            return Result != 0;
        }
        bool isBool() const {
            if (this->str() == "1") {
                return true;
            } else if (this->str() == "0") {
                return false;
            }
            return this->str() == "true" || this->str() == "false";
        }
        bool asBool() const {
            if (this->isBool()) {
                if (this->str() == "1") {
                    return true;
                } else if (this->str() == "0") {
                    return false;
                }
                if (this->str() == "true") {
                    return true;
                }
            }
            return false;
        }
        float asDouble() const {
            if (this->isDouble()) {
                double i = 0.0;
                std::istringstream(this->str()) >> i;
                return i;
            }
            return 0.0;
        }
        bool isAlpha() const {
            if (length() == 0)
                return false;
            else {
                for (char i : *this) {
                    if ((i < 'a' || i > 'z') && (i < 'A' || i > 'Z') && i != ' ') {
                        return false;
                    }
                }
            }
            return true;
        }
        bool isNumeric() {
            if (length() == 0) {
                return false;
            } else {
                return this->regexMatch("^[0-9]+$");
            }
        }
        std::string str() const {
            return std::string(data());
        }

        bool regexMatch(const EZString &regexString, bool caseInsensitive = false) {
            EZString str = *this;
            bool found = false;
            if (caseInsensitive) {
                std::regex self_regex(regexString, std::regex_constants::ECMAScript | std::regex_constants::icase);
                if (regex_search(str, self_regex)) {
                    found = true;
                }
            } else {
                std::regex self_regex(regexString, std::regex_constants::ECMAScript);
                if (regex_search(str, self_regex)) {
                    found = true;
                }
            }
            return found;
        }

        std::vector<EZString> regexExtract(EZString regexString) const {
            std::vector<EZString> ret;
            std::regex e(regexString);
            std::sregex_iterator iter(this->begin(), this->end(), e);
            std::sregex_iterator end;
            while (iter != end) {
                for (unsigned i = 0; i < iter->size(); ++i) {
                    ret.emplace_back((*iter)[i].str());
                }
                ++iter;
            }
            return ret;
        }

        std::vector<EZString> split(EZString delimiter) const {
            std::vector<EZString> ret;
            size_t pos = 0;
            EZString s = *this;
            if (delimiter.empty()) {
                ret.push_back(s);
                for (int i = 0; i < s.length(); i++) {
                    ret.push_back(s.substr(i,1));
                }
            } else {
                while ((pos = s.find(delimiter)) != std::string::npos) {
                    ret.push_back(s.substr(0, pos));
                    s.erase(0, pos + delimiter.length());
                }
                ret.push_back(s);
            }
            return ret;
        }
        std::vector<EZString> splitAtLast(EZString delimiter) const {
            auto splitpos = this->find_last_of(delimiter);
            std::vector<EZString> ret;
            ret.push_back(this->substr(0, splitpos));
            ret.push_back(this->substr(splitpos + 1, (this->size() - (splitpos + 1))));
            return ret;
        }
        std::vector<EZString> splitAtFirst(EZString delimiter) const {
            auto splitpos = this->find_first_of(delimiter);
            std::vector<EZString> ret;
            ret.push_back(this->substr(0, splitpos));
            ret.push_back(this->substr(splitpos + 1, (this->size() - (splitpos + 1))));
            return ret;
        }

        //
        // Below here the methods are chainable and inplace.
        //

        EZString &regexReplace(EZString regexString, EZString replaceWith) {
            std::string str = this->str();
            EZString result;
            std::regex re(regexString);
            std::regex_replace(std::back_inserter(result), str.begin(),
                               str.end(), re, replaceWith);
            *this = result;
            return *this;
        }

        EZString &replaceNonPrintable(EZTools::EZString replacement) {
            this->regexReplace("[^\\x20-\\x7E]+", replacement);
            return *this;
        }

        EZString &removeNonPrintable() {
            this->regexReplace("[^\\x20-\\x7E]+", "");
            return *this;
        }

        EZString &chomp() {
            auto pos = this->find("\n");
            *this = this->substr(0, pos);
            return *this;
        }

        EZString &chopAtLast(EZString delimiter) {
            auto pos = this->find_last_of(delimiter) - delimiter.length();
            *this = this->substr(0, pos);
            return *this;
        }

        EZString &removeExtraSpaces() {
            while (this->regexMatch("  ")) {
                this->regexReplace("[ ][ ]", " ");
            }
            return *this;
        }

        EZString &removeOpeningSpaces() {
            while (this->regexMatch("^ ")) {
                this->regexReplace("^ ", "");
            }
            return *this;
        }

        EZString &removeTrailingSpaces() {
            while (this->regexMatch(" $")) {
                this->regexReplace(" $", "");
            }
            return *this;
        }

        EZString &asHex() {
            std::stringstream ss;
            EZString st = *this;
            for (auto i = 0; i < st.length(); i++) {
                ss << std::hex << (int) st[i];
            }
            *this = ss.str();
            return *this;
        }

        EZString &cat(EZString value) {
            std::stringstream ss;
            ss << *this << value;
            *this = ss.str();
            return *this;
        }

        EZString &replaceAll(const EZString &from, const EZString &to) {
            size_t start_pos = 0;
            while ((start_pos = this->find(from, start_pos)) != std::string::npos) {
                this->replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return *this;
        }
    };

    /*
     * EZTools::EZReturn
     * The idea here is to give a http style response to any call you make.  No need for throws or any of the complexity,
     * just return with EZReturn<datatype> and check .wasSuccessful
     */

    struct EZReturnMetaData {
        bool wasSuccessful = false;
        int exitCode = 0;
        EZTools::EZString message;
        EZTools::EZString location;
    };

    template<class T1 = bool>
    class EZReturn {
    public:
        EZReturn() = default;
        EZReturn(int exitCode, EZString message) {
            _exitCode = exitCode;
            _message = message;
        }
        ~EZReturn() = default;
        bool wasSuccessful() const { return _wasSuccessful; }
        void wasSuccessful(bool success) { _wasSuccessful = success; }
        int exitCode() const { return _exitCode; }
        void exitCode(int value) {_exitCode = value; }
        EZString message() const { return _message; }
        void message(EZString value) { _message = value; }
        void location(EZTools::EZString Location) { _location = Location; }
        EZString location() const { return _location; }
        explicit operator bool() const {
            return _wasSuccessful;
        }
        T1 data;
        EZReturnMetaData metaData() {
            EZReturnMetaData md;
            md.wasSuccessful = _wasSuccessful;
            md.exitCode = _exitCode;
            md.message = _message;
            md.location = _location;
            return md;
        }
    private:
        int _exitCode;
        EZTools::EZString _message;
        EZTools::EZString _location;
        bool _wasSuccessful = false;
    };

    // This is here for backwards compatability but isn't really needed anymore since I reworked EZReturn

    class EZReturnNoData {
    public:
        EZReturnNoData() = default;
        EZReturnNoData(int exitCode, EZString message) {
            _exitCode = exitCode;
            _message = message;
        }
        ~EZReturnNoData() = default;
        bool wasSuccessful() const { return _wasSuccessful; }
        void wasSuccessful(bool success) { _wasSuccessful = success; }
        int exitCode() const { return _exitCode; }
        void exitCode(int value) {_exitCode = value; }
        EZString message() const { return _message; }
        void message(EZString value) { _message = value; }
        void location(EZTools::EZString Location) { _location = Location; }
        EZString location() const { return _location; }
        explicit operator bool() const {
            return _wasSuccessful;
        }
        EZReturnMetaData metaData() {
            EZReturnMetaData md;
            md.wasSuccessful = _wasSuccessful;
            md.exitCode = _exitCode;
            md.message = _message;
            md.location = _location;
            return md;
        }
    private:
        int _exitCode;
        EZTools::EZString _message;
        EZTools::EZString _location;
        bool _wasSuccessful = false;
    };

    template<typename T>
    inline T round(T var, int precision) {
        T val;
        static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value,
                      "Can only round float or double");
        std::stringstream s;
        s << std::setprecision(precision) << std::fixed << var;
        EZString ss = s.str();
        T tmp;
        if (std::is_same<T, double>::value) {
            tmp = ss.asDouble();
        } else {
            tmp = ss.asFloat();
        }
        return tmp;
    }

    inline EZString version() { return VERSION; }

    typedef std::vector<std::vector<EZString> > ezcsv;

    inline EZString intToMonth(int monthNumber) {
        switch (monthNumber) {
            case 1:
                return "January";
            case 2:
                return "February";
            case 3:
                return "March";
            case 4:
                return "April";
            case 5:
                return "May";
            case 6:
                return "June";
            case 7:
                return "July";
            case 8:
                return "August";
            case 9:
                return "September";
            case 10:
                return "October";
            case 11:
                return "November";
            case 12:
                return "December";
            default:
                return "ERROR";
        }
    }

    inline int daysInMonth(int month, int year) {
        if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 ||
            month == 10 || month == 12) {
            return 31;
        } else if (month == 2) {
            if (!year % 4) {
                return 28;
            } else {
                return 29;
            }
        } else if (month == 4 || month == 6 || month == 9 || month == 11) {
            return 30;
        }
        return -1;
    }

    inline EZString intToDay(int dayNumber) {
        switch (dayNumber) {
            case 0:
                return "Sunday";
            case 1:
                return "Monday";
            case 2:
                return "Tuesday";
            case 3:
                return "Wednesday";
            case 4:
                return "Thursday";
            case 5:
                return "Friday";
            case 6:
                return "Saturday";
            default:
                return "";
        }
    }

    inline EZString dateSuffix(int dayNumber) {
        switch (dayNumber) {
            case 1:
            case 21:
            case 31:
                return "st";
            case 2:
            case 22:
                return "nd";
            case 3:
            case 23:
                return "rd";
            default:
                return "th";
        }
    }

    enum BACKUPTYPE {
        BACKUP_PREPEND, BACKUP_EXTENTION
    };

    enum STATFILETYPE {
        UNK = 0, REG = 1, LNK = 2, DIR = 3, CHAR = 4, BLK = 5, FIFO = 6, SOCK = 7, OTHER = 8
    };

    inline EZTools::EZString statfiletypeAsString(STATFILETYPE ft) {
        switch(ft) {
            case UNK:
                return "UNK";
            case REG:
                return "REG";
            case LNK:
                return "LNK";
            case DIR:
                return "DIR";
            case CHAR:
                return "CHAR";
            case BLK:
                return "BLK";
            case FIFO:
                return "FIFO";
            case SOCK:
                return "SOCK";
            case OTHER:
                return "OTHER";
        }
        std::stringstream ss;
        ss << "SHIT! " << ft;
        return ss.str() ;
    }


    enum ACCESSTYPE{AC_THERE, AC_READ, AC_WRITE, AC_ISDIR};

    typedef std::map<char, EZTools::EZString> EZOpts;

    enum CONNECTIONTYPE {HTTP, HTTPS};

    enum INTERFACETYPE {WIFI, ETHERNET, LOCALHOST};

    const EZString IPv4Regex = "^(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]).(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]).(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9]).(25[0-5]|2[0-4][0-9]|1[0-9][0-9]|[1-9]?[0-9])";
    const EZTools::EZString statString(R"(stat -L --printf="URI: %n\nBytes: %s\nType: %F\nPerms: %A\nGID: %g\nGroup: %G\nOwnerID: %u\nOwner: %U\nCreate: %w\nAccess: %x\nMod: %y\n" )");


    typedef std::vector<EZTools::EZString> PATHSTOR;

    enum LOGLEVEL {
        QUITE = 4,
        CRITICAL = 3,
        WARNING = 2,
        INFO = 1,
        DEBUG = 0
    };

    enum NAGIOS_EXIT_LEVELS {
        NAG_OK = 0,
        NAG_WARN = 1,
        NAG_CRIT = 2,
        NAG_UNKNOWN = 3
    };

    struct timeStamp_t {
        EZString date;
        EZString time;
        EZString ms;
        EZString offset;
    };

    struct EZProcess {
        EZTools::EZString UID;
        int PID;
        int PPID;
        int C;
        EZTools::EZString STIME;
        EZTools::EZString TTY;
        EZTools::EZString TIME;
        EZTools::EZString CMD;
        EZTools::EZString BASECMD;

        EZProcess(EZTools::EZString uid, int pid, int ppid, int c, EZTools::EZString stime, EZTools::EZString tty,
                  EZTools::EZString ptime, EZTools::EZString cmd, EZTools::EZString basecmd)
                : UID(move(uid)), PID(pid), PPID(ppid), C(c), STIME(move(stime)), TTY(move(tty)),TIME(move(ptime)),
                  CMD(move(cmd)), BASECMD(move(basecmd)) {};

        EZProcess() :
                UID(""), PID(0), PPID(0), C(0), STIME(""), TTY(""), TIME(""), CMD(""), BASECMD("") {};
    };

    template<typename TYPE, typename PRECISION>
    inline TYPE ConvertRange(TYPE val, TYPE old_low, TYPE old_high, TYPE new_low, TYPE new_high) {
        PRECISION t = (val - old_low) / (PRECISION)(old_high - old_low);
        return (TYPE)(new_low + t * (new_high - new_low));
    }

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZTools");
        std::stringstream ss;
        EZString stest = "Now is the time for all good programmers to learn Perl";
        if (stest != "Now is the time for all good programmers to learn Perl") {
            res.functionTest("EZTools::EZString(\"text\")");
            res.message("EZString stest = \"" + stest + "\"");
            return res;
        }
        ss << "EZString: " << stest << std::endl;
        if (!stest.regexMatch("^Now")) {
            res.functionTest("EZTools::EZString.regexMatch(\"regex\")");
            res.message("stest.regexMatch(\"^Now\")");
            return res;
        }
        ss << "EZString.regexMatch '^Now': " << std::boolalpha << stest.regexMatch("^Now") << std::endl;
        EZString stest2 = stest.regexReplace("Perl", "C++");
        if (!stest2.regexMatch("C++")) {
            res.functionTest(R"(EZTools::EZString.reegexReplace("text", "text"))");
            res.message("EZString stest2 = \"" + stest2 + "\"");
            return res;
        }
        ss << "EZString.regexReplace: " << stest2 << std::endl;
        EZString hn = "ne-vvaprd-tomcat-a01.cdc.nicusa.com";
        auto v = hn.regexExtract("^ne-v(tx|va)(prd|dev|tst)-(.+?)-([a-z])0(\\d+)");
        if (v.at(3) != "tomcat") {
            res.functionTest(R"(EZTools::EZString.reegexExtract("regex"))");
            res.message("EZString at(3) = \"tomcat\"");
            return res;
        }
        ss << "EZString regexExtract: .at(3) = 'tomcat'" << std::endl;
        EZString in(42);
        if (in != "42" || in.asInt() != 42 || !in.isInt()) {
            res.functionTest(R"(EZTools::EZString(int))");
            res.message("EZString in: " + in);
            return res;
        }
        ss << "EZString(int): " << in << std::endl;
        float f = 42.5;
        EZString fl(f);
        if (fl != "42.5" || fl.asFloat() != 42.5) {
            res.functionTest(R"(EZTools::EZString(float))");
            res.message("EZString fl: " + fl);
            return res;
        }
        ss << "EZString(float): " << fl << std::endl;
//        double d = 42.12345;
//        EZString db(d);
//        ss << db << std::endl;
//        if (db != "42.12345" || db.asDouble() != 42.12345) {
//            res.functionTest(R"(EZTools::EZString(double))");
//            res.message("EZString db: " + db);
//            return res;
//        }
//        ss << "EZString(double): " << db << std::endl;
        res.output(ss.str());
        res.wasSuccessful(true);
        return res;
    };

    class commanums : public std::numpunct<char> {
    protected:
        char do_thousands_sep() const override {
            return ',';
        }
        std::string do_grouping() const override {
            return "\03";
        }
    };

    inline EZTools::EZString numWithCommas(float n, int precision = 0) {
        std::stringstream ss;
        std::locale comma_locale(std::locale(), new commanums());
        ss.imbue(comma_locale);
        ss << std::setprecision(precision) << std::fixed << n;
        return ss.str();
    }

}

#endif //EZTOOLS_EZTOOLS_H
