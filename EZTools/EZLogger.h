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

#ifndef EZT_EZLOGGER_H
#define EZT_EZLOGGER_H

#include "EZFiles.h"
#include "EZDateTime.h"
#include "EZLinux.h"
#include "EZConfig.h"

namespace EZLogger {

    class Logger {
    public:
        Logger() = default;
        Logger(EZFiles::URI &logFileURI, EZTools::LOGLEVEL defaultLogLevel) {
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
            _currentLogLevel = _defaultLogLevel;
            _writeLevel = EZTools::LOGLEVEL::DEBUG;
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
                    _defaultLogLevel = EZTools::LOGLEVEL::CRITICAL;
                } else if (tlevel == "WARNING") {
                    _defaultLogLevel = EZTools::LOGLEVEL::WARNING;
                } else if (tlevel == "INFO") {
                    _defaultLogLevel = EZTools::LOGLEVEL::INFO;
                } else if (tlevel == "DEBUG") {
                    _defaultLogLevel = EZTools::LOGLEVEL::DEBUG;
                } else {
                    _defaultLogLevel = EZTools::LOGLEVEL::QUITE;
                }
                _currentLogLevel = _defaultLogLevel;
                _writeLevel = EZTools::LOGLEVEL::DEBUG;
                _isOpen = true;
            } else {
                _isOpen = false;
            }
        }
        ~Logger() {
            _os.close();
            _null.close();
        }
        void init(EZFiles::URI &logFileURI, EZTools::LOGLEVEL defaultLogLevel) {
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
            _currentLogLevel = _defaultLogLevel;
            _writeLevel = EZTools::LOGLEVEL::DEBUG;
        }
        void writeLevel(EZTools::LOGLEVEL writeLevel) { _writeLevel = writeLevel; }
        void tempLogLevel(EZTools::LOGLEVEL tempLogLevel) { _currentLogLevel = tempLogLevel; }
        void defaultLogLevel() { _currentLogLevel = _defaultLogLevel; }
        std::string LogLevelAsString() {
            switch (_writeLevel) {
                case EZTools::LOGLEVEL::CRITICAL:
                    return "CRITICAL: ";
                case EZTools::LOGLEVEL::WARNING:
                    return "WARNING: ";
                case EZTools::LOGLEVEL::INFO:
                    return "INFO: ";
                case EZTools::LOGLEVEL::DEBUG:
                    return "DEBUG: ";
                default:
                    return "";
            }
        }
        bool isOpen() { return _isOpen; }

    private:
        std::ofstream _os;
        std::ofstream _null;
        EZTools::LOGLEVEL _defaultLogLevel;
        EZTools::LOGLEVEL _currentLogLevel;
        EZTools::LOGLEVEL _writeLevel;
        EZTools::EZString _logFileName;
        unsigned long long _lineCounter = 0;
        bool _isOpen;

        template<typename T>
        friend std::ostream &operator<<(Logger &log, T op) {
            if (log._writeLevel >= log._currentLogLevel) {
                EZDateTime::CurrentDateTime dt;
                log._os << dt.ymdto() << " " << log.LogLevelAsString() << op;
                return log._os;
            } else {
                return log._null;
            }
        }
    };
}


#endif //EZT_EZLOGGER_H
