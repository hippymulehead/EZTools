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

#ifndef EZT_EZAPPLICATION_H
#define EZT_EZAPPLICATION_H

#include <cstdlib>
#include <sys/stat.h>
#include <unistd.h>
#include <csignal>
#include "EZTools.h"
#include "EZGetOpt.h"
#include "EZScreen.h"
#include "EZHTTP.h"
#include "EZIP.h"
#include "EZConfig.h"
#include "EZLogger.h"
#include "EZNagios.h"
#include "EZCGI.h"
#include "EZHTML.h"

namespace EZTools {

    class EZApplication {
    public:
        EZApplication(int argc, char *argv[], EZTools::EZString appName,
                EZTools::EZString appVersion = VERSION) {
            _appName = appName;
            _appVersion = appVersion;
            getOpt.init(argc, argv, _appName, _appVersion);
        }

        ~EZApplication() = default;

        EZGetOpt::GetOpt getOpt;

        virtual void close(EZTools::CLOSE exitType, EZTools::EZString message = "", int errorNo = 0) {
            if (!message.empty() & (exitType != EZTools::CLOSE::SUCCESS)) {
                std::cerr << message << std::endl;
            }
            switch (exitType) {
                case EZTools::CLOSE::SUCCESS:
                    exit(0);
                case EZTools::CLOSE::FAIL:
                    exit(1);
                case EZTools::CLOSE::ERROR:
                    exit(errorNo);
            }
        }

    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

    class EZNagiosPlugin : public EZApplication {
    public:
        EZNagiosPlugin(int argc, char *argv[], EZTools::EZString appName,
                EZTools::EZString appVersion = VERSION)
                : EZApplication(argc, argv, appName, appVersion) {
            _appName = appName;
            _appVersion = appVersion;
            getOpt.init(argc, argv, _appName, _appVersion);
        }

        ~EZNagiosPlugin() = default;

        EZHTTP::HTTP http;
        EZNagios::Nagios nagios;

    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

    class EZApp : public EZApplication {
    public:
        EZApp(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION)
                : EZApplication(argc, argv, appName, appVersion) {
            _appName = appName;
            _appVersion = appVersion;
            EZScreen::initScreen();
            getOpt.init(argc, argv, _appName, _appVersion);
        }

        ~EZApp() = default;

        EZScreen::TerminalInput in;
        EZHTTP::HTTP http;
        EZConfig::Config config;
        EZLogger::Logger log;

    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

    class EZSimpleApp : public EZApplication {
    public:
        EZSimpleApp(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION)
                : EZApplication(argc, argv, appName, appVersion) {
            _appName = appName;
            _appVersion = appVersion;
            EZScreen::initScreen();
            getOpt.init(argc, argv, _appName, _appVersion);
        }
        ~EZSimpleApp() = default;

        EZScreen::TerminalInput in;
        EZHTTP::HTTP http;
    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

    class SimpleApp : public EZApplication {
    public:
        SimpleApp(int argc, char *argv[], EZTools::EZString appName, EZTools::EZString appVersion = VERSION)
                : EZApplication(argc, argv, appName, appVersion) {
            _appName = appName;
            _appVersion = appVersion;
            getOpt.init(argc, argv, _appName, _appVersion);
        }
        ~SimpleApp() = default;

        EZHTTP::HTTP http;
    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

}

#endif //EZT_EZAPPLICATION_H
