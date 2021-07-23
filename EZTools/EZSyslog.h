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

#ifndef EZTOOLS_EZSYSLOG_H
#define EZTOOLS_EZSYSLOG_H

#include <ostream>
#include <streambuf>
#include <string>

#include <syslog.h>
#include "EZTools.h"

namespace EZSyslog {

    enum LOGLEVEL {
        CRITICAL = LOG_CRIT,
        WARNING = LOG_WARNING,
        ALERT = LOG_ALERT,
        INFO = LOG_INFO,
        DEBUG = LOG_DEBUG
    };

    inline EZTools::EZString logLevelAsEZSting(LOGLEVEL loglevel) {
        switch (loglevel) {
            case DEBUG:
                return "debug:";
            case INFO:
                return "info:";
            case ALERT:
                return "alert:";
            case WARNING:
                return "warning:";
            case CRITICAL:
                return "critical:";
            default:
                return "";
        }
    }

    enum type {
        auth   = LOG_AUTH,
        cron   = LOG_CRON,
        daemon = LOG_DAEMON,
        local0 = LOG_LOCAL0,
        local1 = LOG_LOCAL1,
        local2 = LOG_LOCAL2,
        local3 = LOG_LOCAL3,
        local4 = LOG_LOCAL4,
        local5 = LOG_LOCAL5,
        local6 = LOG_LOCAL6,
        local7 = LOG_LOCAL7,
        print  = LOG_LPR,
        mail   = LOG_MAIL,
        news   = LOG_NEWS,
        user   = LOG_USER,
        uucp   = LOG_UUCP,
    };

}

class syslog_stream;

class syslog_streambuf: public std::basic_streambuf<char> {
public:
    explicit syslog_streambuf(const std::string& name, EZSyslog::type type): std::basic_streambuf<char>() {
        openlog(name.size() ? name.data() : nullptr, LOG_PID, type);
    }
    ~syslog_streambuf() override { closelog(); }

protected:
    int_type overflow(int_type c = traits_type::eof()) override {
        if(traits_type::eq_int_type(c, traits_type::eof())) {
            sync();
        } else {
            buffer += traits_type::to_char_type(c);
        }
        return c;
    }

    int sync() override {
        if(buffer.size()) {
            syslog(level, "%s %s", EZSyslog::logLevelAsEZSting(level).c_str(), buffer.data());
            buffer.clear();
            level = ini_level;
        }
        return 0;
    }

    friend class syslog_stream;
    void set_level(EZSyslog::LOGLEVEL new_level) noexcept { level = new_level; }

private:
    static constexpr EZSyslog::LOGLEVEL ini_level = EZSyslog::LOGLEVEL::INFO;
    EZSyslog::LOGLEVEL level = ini_level;
    std::string buffer;
};

class syslog_stream: public std::basic_ostream<char> {
public:
    explicit syslog_stream(const std::string& name = std::string(), EZSyslog::type type = EZSyslog::user):
            std::basic_ostream<char>(&streambuf),
            streambuf(name, type)
    { }

    void writeLevel(EZSyslog::LOGLEVEL new_level) {
        streambuf.set_level(new_level);
    }

    syslog_stream& operator<<(EZSyslog::LOGLEVEL level) noexcept {
        streambuf.set_level(level);
        return (*this);
    }

private:
    syslog_streambuf streambuf;
};

#endif //EZTOOLS_EZSYSLOG_H
