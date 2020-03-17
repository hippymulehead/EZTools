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

#ifndef EZT_EZAPPLICATION_H
#define EZT_EZAPPLICATION_H

#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <csignal>
#include "EZTools.h"
#include "EZGetOpt.h"
#include "EZOutput.h"
#include "EZHTTP.h"
#include "EZIP.h"
#include "EZConfig.h"
#include "EZLogger.h"
#include "EZNagios.h"

class EZApplication {
public:
    EZApplication(int argc, char* argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION) {
        _appName = appName;
        _appVersion = appVersion;
        getOpt->init(argc, argv, _appName, _appVersion);
    }
    ~EZApplication() {
        free(getOpt);
    }
    EZGetOpt::GetOpt *getOpt = new EZGetOpt::GetOpt;
    EZDateTime::CurrentDateTime *currentDateTime = new EZDateTime::CurrentDateTime;

private:
    EZTools::EZString _appName;
    EZTools::EZString _appVersion;
};

class EZNagiosPlugin : public EZApplication {
public:
    EZNagiosPlugin(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION)
            : EZApplication(argc, argv, appName, appVersion) {
        _appName = appName;
        _appVersion = appVersion;
        getOpt->init(argc, argv, _appName, _appVersion);
    }
    ~EZNagiosPlugin() {
        free(http);
    }
    //EZOutput::EZScreen::ScreenOutput out;
    EZHTTP::HTTP *http = new EZHTTP::HTTP("EZTools v4 Testing");
    //EZConfig::Config *config = new EZConfig::Config;
    //EZLogger::Logger log;
    //EZIP::LocalIPAddress *localIPAddress = new EZIP::LocalIPAddress();
    EZNagios::Nagios nagios;

private:
    EZTools::EZString _appName;
    EZTools::EZString _appVersion;
};

class EZTestApplication : public EZApplication {
public:
    EZTestApplication(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION)
            : EZApplication(argc, argv, appName, appVersion) {
        _appName = appName;
        _appVersion = appVersion;
        getOpt->init(argc, argv, _appName, _appVersion);
    }
    ~EZTestApplication() {
        free(http);
        free(config);
    }
    EZOutput::EZScreen::ScreenOutput out;
    EZHTTP::HTTP *http = new EZHTTP::HTTP("EZTools v4 Testing");
    EZConfig::Config *config = new EZConfig::Config;
    EZLogger::Logger log;
    //EZIP::LocalIPAddress *localIPAddress = new EZIP::LocalIPAddress();

private:
    EZTools::EZString _appName;
    EZTools::EZString _appVersion;
};

class EZDaemon : public EZApplication {
public:
    EZDaemon(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString pidFileLocation,
             EZTools::EZString appVersion = VERSION)
            : EZApplication(argc, argv, appName, appVersion) {
        _appName = appName;
        _appVersion = appVersion;
        getOpt->init(argc, argv, _appName, _appVersion);
        _pidFileLocation = pidFileLocation;
    }
    ~EZDaemon() {
        free(config);
    }
    EZConfig::Config *config = new EZConfig::Config;
    EZLogger::Logger log;
//    EZIP::LocalIPAddress *localIPAddress = new EZIP::LocalIPAddress();
    void daemonize() {
        EZFiles::URI pidfile(_pidFileLocation);
        if (pidfile.isThere()) {
            std::cout << "pid file already found at " << _pidFileLocation << std::endl;
        }
        pid_t pid, sid;
        pid = fork();
        std::stringstream ss;
        ss << getpid();
        if(pid > 0) {
            exit(EXIT_SUCCESS);
        } else if(pid < 0) {
            exit(EXIT_FAILURE);
        }
        umask(0);
        sid = setsid();
        if(sid < 0) {
            exit(EXIT_FAILURE);
        }
        if((chdir("/")) < 0) {
            exit(EXIT_FAILURE);
        }
        auto res = EZFiles::copyStringToFile(ss.str(), pidfile);
        if (!res.wasSuccessful()) {
            std::cout << res.message() << std::endl;
            _shouldExit = true;
        }
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
    }
    bool shouldExit() { return _shouldExit; }
    void shouldExit(bool quit) {
        _shouldExit = quit;
    }
    void signalHandler( int signum ) {
        shouldExit(true);
    }
    void cleanup() {
        EZFiles::URI pidfile(_pidFileLocation);
        EZFiles::deleteFile(pidfile);
    }

private:
    EZTools::EZString _appName;
    EZTools::EZString _appVersion;
    bool _shouldExit = false;
    EZTools::EZString _pidFileLocation;
};

#endif //EZT_EZAPPLICATION_H
