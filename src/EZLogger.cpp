//
// Created by mromans on 4/5/19.
//

#include "../include/EZLogger.h"

EZLogger::EZLogger(const char *baseDir, const char *logFile, EZLOGLEVEL level, EZLOGGERFLAGS flag) {
    _base = baseDir;
    _currentLevel = level;
    _flag = flag;
    stringstream cc;
    string pre = "/";
    switch(_flag) {
        case EZNONE:
            break;
        case EZSCREENLOG:
            _screen = true;
            break;
        case EZSCREENUSERLOG:
            _screen = true;
            _base = EZLinux::homedir();
            pre = "/.";
            break;
        case EZUSERLOG:
            _base = EZLinux::homedir();
            pre = "/.";
            break;
        default:
            break;
    }
    cc << _base << pre << logFile;
    _fp = fopen(cc.str().c_str(), "a");
}

void EZLogger::write(EZLOGLEVEL level, const char *format, ...) {
    if (level >= _currentLevel) {
        string llevel = LOGLEVELasstring(level);
        va_list arg;
        int done;
        _logTime.update();
        EZString nowTime = _logTime.ymdt();
        if (_screen) {
            printf("%s ", nowTime.c_str());
            printf("%s ", llevel.c_str());
            printf(format, arg);
        }
        fprintf(_fp, "%s %s ", nowTime.c_str(), llevel.c_str());
        va_start (arg, format);
        done = vfprintf (_fp, format, arg);
        if (done < 0) {
            syslog(LOG_ERR, "Can not write to log stream: %s, error: %s",
                   (_fp == stdout) ? "stdout" : _log_file_name, strerror(errno));
        }
        va_end (arg);
        done = fflush(_fp);
        if (done != 0) {
            syslog(LOG_ERR, "Can not fflush() log stream: %s, error: %s",
                   (_fp == stdout) ? "stdout" : _log_file_name, strerror(errno));
        }
    }
}