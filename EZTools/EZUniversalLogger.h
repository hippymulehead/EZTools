/*
Copyright (c) 2017-2023, Michael Romans of Romans Audio
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

#ifndef EZTOOLS_EZUNIVERSALLOGGER_H
#define EZTOOLS_EZUNIVERSALLOGGER_H

#include "EZLogger.h"
#include "EZSyslog.h"
#include "EZFiles.h"
#include "EZDateTime.h"
#include "EZUUID.h"

namespace EZUniversalLogger {

    inline EZTools::EZString logline(std::vector<EZTools::EZString> line) {
        std::stringstream ss;
        for (auto& l : line) {
            ss << "\"" << l << "\" ";
        }
        EZTools::EZString out = ss.str();
        out.removeTrailingSpaces();
        return out;
    }

    enum LOGLEVEL {
        QUITE = 5,
        CRITICAL = 4,
        WARNING = 3,
        INFO = 2,
        DEBUG = 1
    };

    class Logger {
    public:
        Logger() = default;
        Logger(EZFiles::URI &logFileURI, LOGLEVEL defaultLogLevel) {
            _logFileName = logFileURI.path();
            if (_logFileName.regexMatch("^~")) {
                _logFileName.regexReplace("^~", EZFiles::homeDir());
            }
            _os.open(_logFileName, std::ios::out | std::ios::app);
            if (!_os.is_open()) {
                std::cout << "Failed to open log file: " << _logFileName << std::endl;
                exit(EXIT_FAILURE);
            }
            _null.open("/dev/null", std::ios::out);
            _defaultLogLevel = defaultLogLevel;
            _writeLevel = _defaultLogLevel;
        }
        explicit Logger(EZFiles::URI &configFile) {
            EZTools::EZString cf = configFile.path();
            EZConfig::Config _confFile(configFile);
            if (!_confFile.root["filename"].empty() && !_confFile.root["defaultLogLevel"].empty()) {
                _logFileName = _confFile.root["filename"].get<EZTools::EZString>();
                _os.open(_logFileName, std::ios::out | std::ios::app);
                if (!_os.is_open()) {
                    std::cout << "Failed to open log file: " << _logFileName << std::endl;
                    exit(EXIT_FAILURE);
                }
                _null.open("/dev/null", std::ios::out);
                EZTools::EZString tlevel = _confFile.root["defaultLogLevel"].get<EZTools::EZString>();
                if (tlevel == "CRITICAL") {
                    _defaultLogLevel = LOGLEVEL::CRITICAL;
                } else if (tlevel == "WARNING") {
                    _defaultLogLevel = LOGLEVEL::WARNING;
                } else if (tlevel == "INFO") {
                    _defaultLogLevel = LOGLEVEL::INFO;
                } else if (tlevel == "DEBUG") {
                    _defaultLogLevel = LOGLEVEL::DEBUG;
                } else {
                    _defaultLogLevel = LOGLEVEL::QUITE;
                }
                _writeLevel = _defaultLogLevel;
                _isOpen = true;
            } else {
                _isOpen = false;
            }
        }
        ~Logger() {
            _os << flush;
            _os.close();
            _null.close();
        }
        void init(EZFiles::URI &logFileURI, LOGLEVEL defaultLogLevel) {
            _logFileName = logFileURI.path();
            if (_logFileName.regexMatch("^~")) {
                _logFileName.regexReplace("^~", EZFiles::homeDir());
            }
            _os.open(_logFileName, std::ios::out | std::ios::app);
            if (!_os.is_open()) {
                std::cout << "Failed to open log file: " << _logFileName << std::endl;
                exit(EXIT_FAILURE);
            }
            _null.open("/dev/null", std::ios::out);
            _defaultLogLevel = defaultLogLevel;
            _writeLevel = _defaultLogLevel;
        }
        void writeLevel(LOGLEVEL writeLevel) { _writeLevel = writeLevel; }
        void defaultLogLevel() {
            _writeLevel = _defaultLogLevel;
        }
        std::string currentLogLevel() {
            switch (_writeLevel) {
                case LOGLEVEL::CRITICAL:
                    return "CRITICAL";
                case LOGLEVEL::WARNING:
                    return "WARNING";
                case LOGLEVEL::INFO:
                    return "INFO";
                case LOGLEVEL::DEBUG:
                    return "DEBUG";
                case LOGLEVEL::QUITE:
                    return "QUITE";
                default:
                    return "";
            }
        }
        bool isOpen() { return _isOpen; }
        void newID() {
            _id = EZUUID::UUID_v4();
        }
        EZTools::EZString currentLogID() {
            return _id + "|";
        }

    private:
        std::ofstream _os;
        std::ofstream _null;
        LOGLEVEL _defaultLogLevel;
        LOGLEVEL _writeLevel;
        EZTools::EZString _logFileName;
        unsigned long long _lineCounter = 0;
        bool _isOpen;
        EZTools::EZString _id = EZUUID::UUID_v4();

        std::string currentLogLevelForLogging() {
            switch (_writeLevel) {
                case LOGLEVEL::CRITICAL:
                    return "CRITICAL";
                case LOGLEVEL::WARNING:
                    return "WARNING";
                case LOGLEVEL::INFO:
                    return "INFO";
                case LOGLEVEL::DEBUG:
                    return "DEBUG";
                case LOGLEVEL::QUITE:
                    return "QUITE";
                default:
                    return "";
            }
        }

        template<typename T>
        friend std::ostream &operator<<(Logger &log, T op) {
            if (log._writeLevel >= log._defaultLogLevel && log._writeLevel != LOGLEVEL::QUITE) {
                EZDateTime::DateTime dt;
                log._os << dt.ymdtWithMilliSeconds() << " " << log.currentLogLevelForLogging() << " " << op;
                return log._os;
            } else {
                return log._null;
            }
        }
    };

};

#endif //EZTOOLS_EZUNIVERSALLOGGER_H
