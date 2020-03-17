/*
Copyright (c) 2018-2019, Michael Romans of Romans Audio
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
either expressed or implied, of the EZTools project.
*/

#ifndef EZTOOLS_EZTOOLS_H
#define EZTOOLS_EZTOOLS_H

#include <iostream>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>

#ifdef CENTOS7
#include <Poco/RegularExpression.h>
#else
#include <regex>
#endif

namespace EZTools {
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
            ss << f;
            *this = EZString(ss.str());
        }
        EZString(double d) {
            std::stringstream ss;
            ss << d;
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
            for(uint i = 1; i < list.size(); i++) {
                append(separator);
                append(list[i]);
            }
        }
        unsigned long int count(EZString s) const {
            uint total = 0;
            size_t end,pos = 0;
            while(true) {
                end = find(s,pos);
                if(end == std::string::npos)
                    break;
                pos = end + s.size();
                total++;
            }
            return total;
        }
        EZString &lower() {
            for(char & i : *this) {
                if(i >= 'A' && i <= 'Z') {
                    i += ('a' - 'A');
                }
            }
            return *this;
        }
        EZString &upper() {
            for(char & i : *this) {
                if(i >= 'a' && i <= 'z') {
                    i -= ('a' - 'A');
                }
            }
            return *this;
        }
        bool isLower() const {
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
        bool isUpper() const {
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
        bool isInt() const {
            int Result;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> Result) ) {
                Result = 0;
            }
            return Result != 0;
        }
        int asInt() const {
            if (this->isInt()) {
                int i = 0;
                std::istringstream (this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isLong() const {
            long Result;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> Result) ) {
                Result = 0;
            }
            return Result != 0;
        }
        int asLong() const {
            if (this->isInt()) {
                long i = 0;
                std::istringstream (this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isUnsignedLong() const {
            unsigned long Result;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> Result) ) {
                Result = 0;
            }
            return Result != 0;
        }
        unsigned long asUnsignedLong() const {
            if (this->isUnsignedLong()) {
                unsigned long i = 0;
                std::istringstream (this->str()) >> i;
                return i;
            }
            return 0;
        }
        bool isUnsignedLongLong() const {
            unsigned long Result;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> Result) ) {
                Result = 0;
            }
            return Result != 0;
        }
        unsigned long long asUnsignedLongLong() const {
            unsigned long long i = 0;
            std::istringstream (this->str()) >> i;
            return i;
        }
        bool isFloat() const {
            int Result = 1;
            float f;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> f) ) {
                Result = 0;
            }
            return Result != 0;
        }
        float asFloat() const {
            if (this->isFloat()) {
                float i = 0.0;
                std::istringstream (this->str()) >> i;
                return i;
            }
            return 0.0;
        }
        bool isDouble() const {
            int Result = 1;
            double f;
            if ((int)this->str()[0] < 10) {
                return false;
            }
            std::istringstream convert(this->str());
            if ( !(convert >> f) ) {
                Result = 0;
            }
            return Result != 0;
        }
        bool isBool() const {
            return this->str() == "true" || this->str() == "false";
        }
        bool asBool() const {
            if (this->isBool()) {
                if (this->str() == "true") {
                    return true;
                }
            }
            return false;
        }
        float asDouble() const {
            if (this->isDouble()) {
                double i = 0.0;
                std::istringstream (this->str()) >> i;
                return i;
            }
            return 0.0;
        }
        bool isAlpha() const {
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
        std::string str() const {
            return std::string(data());
        }

#ifdef CENTOS7
        bool regexMatch(const EZString &regexString, bool caseInsensitive = false) {
            EZString str = *this;
            bool found = false;
            Poco::RegularExpression regex(regexString);
            Poco::RegularExpression::MatchVec mvec;
            int matches = regex.match(str,0,mvec);
            return matches > 0;
        }
#else
        bool regexMatch(const EZString &regexString, bool caseInsensitive = false) {
            EZString str = *this;
            bool found = false;
            if (caseInsensitive) {
                std::regex self_regex(regexString, std::regex_constants::ECMAScript | std::regex_constants::icase);
                if (regex_search(str,self_regex)) {
                    found = true;
                }
            } else {
                std::regex self_regex(regexString, std::regex_constants::ECMAScript);
                if (regex_search(str,self_regex)) {
                    found = true;
                }
            }
            return found;
        }

#endif
#ifdef CENTOS7
        std::vector<EZString> regexExtract(EZString regexString) const {
            EZString str = *this;
            Poco::RegularExpression regex(regexString);
            std::vector<std::string> retval;
            std::vector<EZString> ret;
            regex.split(str, retval, 0);
            for (auto &s : retval) {
                ret.push_back(s);
            }
            return ret;

        }
#else
        std::vector<EZString> regexExtract(EZString regexString) const {
            std::vector<EZTools::EZString> ret;
            std::regex e(regexString);
            std::sregex_iterator iter(this->begin(), this->end(), e);
            std::sregex_iterator end;
            while(iter != end) {
                for(unsigned i = 0; i < iter->size(); ++i) {
                    ret.push_back((*iter)[i].str());
                }
                ++iter;
            }
            return ret;
        }
#endif
        std::vector<EZString> split(EZString delimiter) const {
            std::vector<EZString> ret;
            size_t pos = 0;
            std::string s = *this;
            while ((pos = s.find(delimiter)) != std::string::npos) {
                ret.push_back(s.substr(0, pos));
                s.erase(0, pos + delimiter.length());
            }
            ret.push_back(s);
            return ret;
        }
        std::vector<EZString> splitAtLast(EZString delimiter) const {
            auto splitpos = this->find_last_of(delimiter);
            std::vector<EZString> ret;
            ret.push_back(this->substr(0, splitpos));
            ret.push_back(this->substr(splitpos + 1, (this->size() - (splitpos + 1))));
            return ret;
        }

#ifdef CENTOS7
        std::vector<EZTools::EZString> regexSplit(EZTools::EZString regexString) {
            EZString str = *this;
            Poco::RegularExpression regex(regexString);
            std::vector<std::string> retval;
            std::vector<EZString> ret;
            regex.split(str, retval, 0);
            for (auto &s : retval) {
                ret.push_back(s);
            }
            return ret;
        }
#else
        std::vector<EZTools::EZString> regexSplit(EZTools::EZString regexString) {
            EZTools::EZString str = this->str();
            std::vector<EZTools::EZString> res;
            std::regex rgx(regexString);
            std::sregex_token_iterator iter(str.begin(), str.end(), rgx, -1);
            std::sregex_token_iterator end;
            for (; iter != end; ++iter) {
                res.push_back(iter->str());
            }
            return res;
        }
#endif
        //
        // Below here the methods are chainable and inplace.
        //
#ifdef CENTOS7
        EZString &regexReplace(EZString regexString, EZString replaceWith) {
            Poco::RegularExpression regex(regexString);
            regex.subst(*this, replaceWith, Poco::RegularExpression::RE_GLOBAL);
            return *this;
        }
#else
        EZString &regexReplace(EZString regexString, EZString replaceWith) {
            std::string str = this->str();
            EZTools::EZString result;
            std::regex re(regexString);
            std::regex_replace(std::back_inserter(result), str.begin(),
                    str.end(), re, replaceWith);
            *this = result;
            return *this;
        }
#endif
        EZString &removeNonPrintable() {
            this->regexReplace("[^\\x20-\\x7E]+", "");
            return *this;
        }
        EZString &chomp() {
            auto pos = this->find("\n");
            *this = this->substr(0,pos);
            return *this;
        }
        EZString &chopAtLast(EZString delimiter) {
            auto pos = this->find_last_of(delimiter) - delimiter.length();
            *this = this->substr(0,pos);
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
            while((start_pos = this->find(from, start_pos)) != std::string::npos) {
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
    template<class T>
    class EZReturn {
    public:
        EZReturn() = default;
        EZReturn(int exitCode, EZString message, T Data) {
            _exitCode = exitCode;
            _message = message;
            data = Data;
        }
        ~EZReturn() = default;
        bool wasSuccessful() { return _wasSuccessful; }
        void wasSuccessful(bool success) { _wasSuccessful = success; }
        int exitCode() { return _exitCode; }
        void exitCode(int value) { _exitCode = value; }
        EZString message() { return _message; }
        void message(EZString value) { _message = value; }
        T data;

    private:
        int _exitCode = 0;
        EZString _message;
        bool _wasSuccessful = false;
    };

    /*
     * EZTools::round
     * Don't use this as it will probably go away.  I wrote this in a moment of weakness and just haven't had the
     * heart to take it out behind the barn and shoot it.
     */
    //fixme: This is crap...
    template<typename T>
    inline T round(T var, int precision) {
        T val;
//        static_assert(std::is_same<T, double>::value || std::is_same<T, float>::value,
//                      "Can only round float or double");
        std::stringstream s;
        s << std::setprecision(precision) << std::fixed << var;
        EZString ss = s.str();
        double tmp = ss.asDouble();
        // FIXME: CentOS 7 Doesn't have std::is_same<> :(
        return static_cast<T>(tmp);
    }

    inline EZTools::EZString version() { return VERSION; }

}

#endif //EZTOOLS_EZTOOLS_H
