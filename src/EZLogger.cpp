//
// Created by mromans on 7/29/19.
//

#include "EZLogger.h"

EZLogger::EZLogger(EZString logFileName, EZLogLevel defaultLogLevel) {
    _logFileName = logFileName;
    if (_logFileName.regexMatch("^~")) {
        _logFileName.regexReplace("^~", homeDir());
    }
    _os.open(_logFileName, ios::out | ios::app);
    if (!_os.is_open()) {
        cout << "Failed to open log file: " << _logFileName << endl;
        exit(EXIT_FAILURE);
    }
    _null.open("/dev/null", ios::out);
    _defaultLogLevel = defaultLogLevel;
    _currentLogLevel = _defaultLogLevel;
    _writeLevel = DEBUG;
}

EZLogger::EZLogger(EZString configFileName) {
    EZConfig confFile(configFileName);
    _logFileName = confFile.jsonRoot["filename"].asString();
    if (_logFileName.regexMatch("^~")) {
        _logFileName.regexReplace("^~", homeDir());
    }
    _os.open(_logFileName, ios::out | ios::app);
    if (!_os.is_open()) {
        cout << "Failed to open log file: " << _logFileName << endl;
        exit(EXIT_FAILURE);
    }
    _null.open("/dev/null", ios::out);
    string tlevel = confFile.jsonRoot["defaultLogLevel"].asString();
    if (tlevel == "CRITICAL") {
        _defaultLogLevel = CRITICAL;
    } else if (tlevel == "WARNING") {
        _defaultLogLevel = WARNING;
    } else if (tlevel == "INFO") {
        _defaultLogLevel = INFO;
    } else if (tlevel == "DEBUG") {
        _defaultLogLevel = DEBUG;
    } else {
        _defaultLogLevel = QUITE;
    }
    _currentLogLevel = _defaultLogLevel;
    _writeLevel = DEBUG;
}

EZLogger::~EZLogger() {
    _os.close();
    _null.close();
}

void EZLogger::writeLevel(EZLogLevel writeLevel) {
    _writeLevel = writeLevel;
}

void EZLogger::tempLogLevel(EZLogLevel tempLogLevel) {
    _currentLogLevel = tempLogLevel;
}

void EZLogger::defaultLogLevel() {
    _currentLogLevel = _defaultLogLevel;
}

string EZLogger::LogLevelAsString() {
    switch (_writeLevel) {
        case CRITICAL:
            return "CRITICAL: ";
        case WARNING:
            return "WARNING: ";
        case INFO:
            return "INFO: ";
        case DEBUG:
            return "DEBUG: ";
        default:
            return "";
    }
}
