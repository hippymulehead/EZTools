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

#ifndef EZTOOLS_EZLOGGER_H
#define EZTOOLS_EZLOGGER_H

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syslog.h>
#include <stdarg.h>
#include <fstream>
#include <sstream>
#include "EZSystemTime.h"
#include "EZLinux.h"
#include <vector>

enum EZLOGLEVEL {
    EZDEBUG = 0,
    EZINFO = 1,
    EZWARNING = 2,
    EZCRITICAL = 3
};

enum EZLOGGERFLAGS {
    EZNONE,
    EZSCREENLOG,
    EZSCREENUSERLOG,
    EZUSERLOG
};

struct EZLoggerConfig {
    string logfileName;
    EZLOGLEVEL defaultLevel;
    bool toScreen;
    vector<string> flags;
};

class EZLogger {
public:
    EZLogger() = default;
    EZLogger(const char *baseDir, const char *logFile, EZLOGLEVEL level, EZLOGGERFLAGS flag);
    virtual ~EZLogger() {
        _log_file_name = nullptr;
    }
    void write(EZLOGLEVEL level, const char *format, ...);
    void close() { fclose(_fp); }
    void open(const char *logFile) { _fp = fopen (logFile, "a"); }
    void logLevel(EZLOGLEVEL level) { _currentLevel = level; }
    int isOpen() const { return _isOpen; }

protected:
    FILE *_fp = stdout;
    char *_log_file_name = nullptr;
    int _isOpen = 0;
    EZLOGLEVEL _currentLevel = EZDEBUG;
    EZSystemTime _logTime;
    EZLOGGERFLAGS _flag = EZNONE;
    bool _screen = false;
    bool _userlog = "";
    string _logname = "";
    string _base = "";

    string LOGLEVELasstring(EZLOGLEVEL level) {
        string retval = "";
        switch(level) {
            case EZDEBUG:
                retval = "DEBUG";
                break;
            case EZINFO:
                retval = "INFO";
                break;
            case EZWARNING:
                retval = "WARNING";
                break;
            case EZCRITICAL:
                retval = "CRITICAL";
                break;
            default:
                break;
        }
        return retval;
    }
};

#endif //EZTOOLS_EZLOGGER_H