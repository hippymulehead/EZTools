//
// Created by mromans on 8/31/22.
//

#ifndef EZTOOLS_EZAPPFULL_H
#define EZTOOLS_EZAPPFULL_H

#pragma once

#include "EZApp.h"
#include "EZConfig.h"
#include "EZLogger.h"
#include "EZHTTP.h"
#include "EZScreen.h"

namespace EZAppFull {

class App : public EZApp::App {
    public:
    App(int argc, char *argv[], EZTools::EZString appName, bool Color = true, EZTools::EZString appVersion = VERSION)
                : EZApp::App(argc, argv, appName, Color, appVersion) {
            _appName = appName;
            _appVersion = appVersion;
            EZScreen::initScreen();
            getOpt.init(argc, argv, _appName, _appVersion);
        }
        ~App() = default;
        EZScreen::TerminalInput in;
        EZHTTP::HTTPClient http;
        EZConfig::Config config;
        EZLogger::Logger log;

    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

}

#endif //EZTOOLS_EZAPPFULL_H
