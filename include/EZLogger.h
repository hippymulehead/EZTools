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

#ifndef EZLOGGER_H
#define EZLOGGER_H

#include "EZTools.h"
#include "EZLinux.h"
#include "EZConfig.h"
#include <iostream>
#include <fstream>

using namespace EZDateTimeFunctions;
using namespace EZTools;
using namespace EZLinux;
using namespace std;

enum EZLogLevel {
    QUITE = 4,
    CRITICAL = 3,
    WARNING = 2,
    INFO = 1,
    DEBUG = 0
};

class EZLogger {
public:
    EZLogger(EZString logFileName, EZLogLevel defaultLogLevel);
    explicit EZLogger(EZString configFileName);
    ~EZLogger();
    void writeLevel(EZLogLevel writeLevel);
    void tempLogLevel(EZLogLevel tempLogLevel);
    void defaultLogLevel();
    string LogLevelAsString();

private:
    ofstream _os;
    ofstream _null;
    EZLogLevel _defaultLogLevel;
    EZLogLevel _currentLogLevel;
    EZLogLevel _writeLevel;
    EZString _logFileName;

    template<typename T>
    friend ostream& operator<<(EZLogger& log, T op) {
        if (log._writeLevel >= log._currentLogLevel) {
            EZSystemTime dt;
            log._os << dt.ymdt() << " " << log.LogLevelAsString() << op;
            return log._os;
        }
        return log._null;
    }
};

#endif //EZLOGGER_H
