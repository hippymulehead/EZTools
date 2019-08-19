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
either expressed or implied, of the MRUtils project.
*/

#ifndef CPPRESTTEST_EZTOOLS_H
#define CPPRESTTEST_EZTOOLS_H

#include <random>
#include <vector>
#include <sstream>
#include <openssl/sha.h>
#include <Poco/Foundation.h>
#include <Poco/String.h>
#include <Poco/RegularExpression.h>
#include <Poco/HMACEngine.h>
#include <Poco/Crypto/DigestEngine.h>
#include <Poco/Crypto/RSAKey.h>
#include <Poco/Timestamp.h>
#include <Poco/DateTime.h>
#include <Poco/DateTimeFormat.h>
#include <Poco/DateTimeFormatter.h>
#include <Poco/DateTimeParser.h>
#include <Poco/Timespan.h>
#include <Poco/Random.h>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <map>
#include <iostream>

using namespace std;
using namespace Poco;
using namespace Poco::Crypto;
using Poco::Random;

/*
 * EZTools::EZCrypto
 */
namespace EZCrypto {

    class SHA256Engine : public Poco::Crypto::DigestEngine {
    public:
        enum {
            BLOCK_SIZE = 64,
            DIGEST_SIZE = 32
        };

        SHA256Engine() : DigestEngine("SHA256") {}
    };

    inline void makeRSAKeys(string basedir, string baseFileName, string password,
                            RSAKey::KeyLength keyLength = RSAKey::KeyLength::KL_4096,
                            RSAKey::Exponent exponent = RSAKey::Exponent::EXP_LARGE) {
        Poco::Crypto::initializeCrypto();
        Poco::Crypto::RSAKey key(keyLength, exponent);
        string pubFile(basedir + baseFileName + ".pub");
        string privFile(basedir + baseFileName + ".priv");
        key.save(pubFile, privFile, password);
        Poco::Crypto::uninitializeCrypto();
    }
}

namespace EZRandom {
/*
 * EZTools::EZCrypto
 */
    class EZRand {
    public:
        EZRand();
        ~EZRand() = default;
        UInt32 Int() { return _rdev.next(); }
        UInt32 Int(UInt32 upper) { return _rdev.next(upper); }
        char Char() { return _rdev.nextChar(); }
        float Float() { return _rdev.nextFloat(); }
        double Double() { return _rdev.nextDouble(); }
        bool Bool() { return _rdev.nextBool(); }

    private:
        Random _rdev;
    };
}


namespace EZTools {

/*
 * EZTools::EZString
 * EZString is an enhanced version of std::string.  It also acts as a kind of Any var as it has methods to init it's
 * self with lots of non string values.  You can then get the value back out safely to other types as well.
 * Many JSON libs don't do numeric types very well.  All numeric types in JSON should be considered floats but if
 * you have an int in jsoncpp and try to get it out as a float, it fails because there is no . in it.  EZString just
 * lets you do it.  So you read everything in to EZString, then output it as what you really need.
 * It also has a lot of convince string methods that make working with strings really easy and quick to code.
 */
    class EZString : public string {
    public:
        EZString();
        EZString(const string &s);
        EZString(const string &s, size_t n);
        EZString(const char *s, size_t n);
        EZString(const char *s);
        EZString(size_t n, char c);
        EZString(int i);
        EZString(float f);
        EZString(double d);
        EZString(long l);
        EZString(unsigned long l);
        EZString(unsigned long long l);
        template<class InputIterator>
        EZString(InputIterator first, InputIterator last);
        void join(EZString separator, vector<EZString> &list);
        unsigned long int count(EZString s) const;
        EZString& lower();
        EZString& upper();
        bool isLower() const;
        bool isUpper() const;
        bool isInt() const;
        int asInt() const;
        bool isLong() const;
        int asLong() const;
        bool isUnsignedLong() const;
        unsigned long asUnsignedLong() const;
        bool isUnsignedLongLong() const;
        unsigned long long asUnsignedLongLong() const;
        bool isFloat() const;
        float asFloat() const;
        bool isDouble() const;
        float asDouble() const;
        bool isAlpha() const;
        string str() const;

        // Poco regex is broken.  Does not accept [0-9]

        bool regexMatch(const EZString &regexString);
        vector<EZString> regexExtract(EZString regexString) const;
        EZString& sha256(EZString secretKey);
        vector<EZString> split(EZString delimiter) const;
        vector<EZString> splitAtLast(EZString &delimiter) const;
        //
        // Below here the methods are chainable and inplace.
        //
        EZString& regexReplace(EZString regexString, EZString replaceWith);
        EZString& chomp();
        EZString& chopAtLast(EZString delimiter);
        EZString& removeExtraSpaces();
        EZString& removeOpeningSpaces();
        EZString& removeTrailingSpaces();
        EZString& asHex();
        EZString& cat(EZString value);
        EZString& replaceAll(const EZString &from, const EZString &to);
    };

    class EZError {
    public:
        explicit EZError(EZString errorMessage, bool isSuccess = false);
        explicit EZError() { _isSuccess = true; }
        ~EZError() = default;
        bool isSuccess() { return _isSuccess; }
        EZString errorMessage() { return _errorMessage; }

    private:
        bool _isSuccess;
        EZString _errorMessage;
    };

/*
 * EZTools::EZDBCreds
 * Just a simple object to pass in your database creds for mysql
 */
    class EZDBCreds {
    public:
        EZDBCreds() = default;
        EZDBCreds(EZTools::EZString address, EZTools::EZString databaseName, EZTools::EZString username,
                  EZTools::EZString password);

        ~EZDBCreds() = default;
        EZTools::EZString address() { return _address; }
        EZTools::EZString databaseName() { return _databaseName; }
        EZTools::EZString username() { return _username; }
        EZTools::EZString password() { return _password; }

    private:
        EZTools::EZString _address;
        EZTools::EZString _databaseName;
        EZTools::EZString _username;
        EZTools::EZString _password;
    };

/*
 * EZTools::EZKeyValueMap
 */
    template <class T>
    class EZKeyValueMap {
    public:
        EZKeyValueMap() = default;
        ~EZKeyValueMap() = default;
        void insert(EZString key, T value);
        void removeKey(EZString key);
        T valueForKey(EZString key);
        size_t size() { return _map.size(); }
        size_t begin() { return 0; }
        size_t end() { return _map.size(); }

    private:
        map<EZString, T> _map;
    };

/*
 * EZTools::EZMiniVector
 */
    template <typename T>
    class EZMiniVector {
    public:
        EZMiniVector() {
            m_capacity = MIN_CAPACITY;
            m_size = 0;
            m_data = (T*)malloc(m_capacity * sizeof(*m_data));
            if (!m_data)
                throw std::bad_alloc();
        };
        ~EZMiniVector() { free(m_data); };

        T &operator[] (size_t index) {
            if (index >= m_size) {
                throw std::bad_exception();
            }
            return m_data[index];
        }
        void push_back(T value) {
            if (m_size >= m_capacity) {
                resize();
            }
            *(m_data + m_size++) = value;
        };
        void emplace_back(T value) {
            if (m_size >= m_capacity) {
                resize();
            }
            *(m_data + m_size++) = value;
        };
        T pop_back() {
            return *(m_data + --m_size);
        };
        T at(unsigned long index) {
            if (index >= m_size) {
                throw std::bad_exception();
            }
            return m_data[index];
        };
        size_t size() { return m_size; };
        size_t capacity() { return m_capacity; };
        bool empty() { return m_size == 0; };
        size_t begin() { return 0; }
        size_t end() { return size() - 1; }

    private:
        size_t m_size;
        size_t m_capacity;
        T* m_data;
        const int MIN_CAPACITY = 10;
        const int GROWTH_FACTOR = 2;
        void resize() {
            size_t capacity = m_capacity*GROWTH_FACTOR;
            T *tmp = (T*)realloc(m_data, capacity * sizeof(*m_data));
            if (!tmp)
                throw std::bad_alloc();
            m_data = tmp;
            m_capacity = capacity;
        };
    };

/*
 * UTILS
 */

    inline void longSleep(long minutes) {
        std::chrono::minutes timespan(minutes);
        std::this_thread::sleep_for(timespan);
    }

    inline void sleep(long seconds) {
        std::chrono::seconds timespan(seconds);
        std::this_thread::sleep_for(timespan);
    }

    inline void nanoSleep(long milliseconds) {
        std::chrono::milliseconds timespan(milliseconds);
        std::this_thread::sleep_for(timespan);
    }

    inline float round(float var, int precision) {
        stringstream s;
        s << setprecision(precision) << fixed << var;
        EZString ss = s.str();
        return ss.asFloat();
    }

}

/*
 * EZTools::EZDateTimeFunctions
 */
namespace EZDateTimeFunctions {

    class EZSystemTime {
    public:
        EZSystemTime();
        virtual ~EZSystemTime() = default;
        void update();
        EZTools::EZString weekday() const { return _weekday; }
        EZTools::EZString month() const { return _month; }
        EZTools::EZString monthNumber() const { return _monthNumber; }
        EZTools::EZString monthday() const { return _monthday; }
        EZTools::EZString time() const { return _time; }
        EZTools::EZString timeNoSec() const { return _timeNoSec; }
        EZTools::EZString year() const { return _year; }
        void setDateSeperator(EZTools::EZString dateSep) {_dateSep = dateSep; }
        EZTools::EZString mdy() const;
        EZTools::EZString ymd() const;
        EZTools::EZString ymdt() const;
        EZTools::EZString mdyt() const;
        EZTools::EZString fancy() const;
        EZTools::EZString fancyWithWeekDay() const;
        EZTools::EZString dtstamp() const;

    private:
        EZTools::EZString _weekday;
        EZTools::EZString _month;
        EZTools::EZString _monthNumber;
        EZTools::EZString _monthday;
        EZTools::EZString _time;
        EZTools::EZString _timeNoSec;
        EZTools::EZString _year;
        EZTools::EZString _dateSep = "/";
    };

    class EZDateTime {
    public:
        enum DTFORMAT {NWS_FORMAT, ISO8601_FORMAT, RFC1123_FORMAT, RFC1036_FORMAT, RFC850_FORMAT, RFC822_FORMAT};
        EZDateTime() = default;
        void setNWSDateTime(EZTools::EZString nwsdatetime);
        void setMySQL(EZTools::EZString mysqldatetime);
        EZDateTime(Timestamp);
        ~EZDateTime() = default;
        LocalDateTime datetime() { return _dt; }
        string format(string datetimeformatstring) {return DateTimeFormatter::format(_dt, datetimeformatstring);}
        string sortable() { return DateTimeFormatter::format(_dt, "%Y-%m-%d %H:%M:%S");};
        string webView() { return DateTimeFormatter::format(_dt, "%Y-%m-%d %h:%M:%S %a");}
        string pretty() { return DateTimeFormatter::format(_dt, "%w, %b %e, %Y %h:%M:%S %a");};

    private:
        DateTime _dt;
    };

    class EZTimestamp {
    public:
        EZTimestamp() = default;
        ~EZTimestamp() = default;
        string now();
    private:
        Timestamp _now;
    };

    class EZTimer {
    public:
        EZTimer() = default;
        virtual ~EZTimer() = default;
        void start();
        void stop();
        long asMicroseconds() { return _microseconds; }
        float asMillisenconds() { return _milliseconds; }
        float asSeconds() { return _seconds; }
        float asMinutes() { return  _minutes; }
        float asHours() { return _hours; }

    private:
        long _microseconds = 0;
        float _milliseconds = 0;
        float _seconds = 0;
        float _minutes = 0;
        float _hours = 0;
        Timestamp _start;
    };

}


#endif //CPPRESTTEST_EZTOOLS_H
