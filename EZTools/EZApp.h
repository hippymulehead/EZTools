/*
Copyright (c) 2017-2022, Michael Romans of Romans Audio
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

#ifndef EZTOOLS_EZAPP_H
#define EZTOOLS_EZAPP_H

#pragma once

#include "EZTools.h"
#include "EZGetOpt.h"
#include "EZScreen.h"

namespace EZApp {

    class App {
    public:
        App(int argc, char *argv[], EZTools::EZString appName, bool Color = true,
              EZTools::EZString appVersion = VERSION) {
            _appName = appName;
            _appVersion = appVersion;
            EZScreen::initScreen(Color);
            getOpt.init(argc, argv, _appName, _appVersion);
        }

        ~App() = default;

        EZGetOpt::GetOpt getOpt;

        void close(EZTools::CLOSE exitType, EZTools::EZString message = "", int errorNo = 0) {
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

        void close(EZTools::CLOSE exitType, EZTools::EZReturnMetaData metaData) {
            if (!metaData.message.empty()) {
                if (EZScreen::color) {
                    EZScreen::consoleBell();
                    std::cout << foreground(EZScreen::BRIGHT_RED) << CON_BOLD_ON;
                    std::cout << "Error " << metaData.exitCode << " in " << metaData.location << std::endl
                              << foreground(EZScreen::BRIGHT_YELLOW) << CON_BOLD_OFF
                              << metaData.message << std::endl << CON_RESET << std::flush;
                } else {
                    std::cout << "Error " << metaData.exitCode << " in " << metaData.location << std::endl
                              << metaData.message << std::endl;
                }
            }
            switch (exitType) {
                case EZTools::CLOSE::SUCCESS:
                    exit(0);
                case EZTools::CLOSE::FAIL:
                    exit(1);
                case EZTools::CLOSE::ERROR:
                    exit(metaData.exitCode);
            }
        }

    private:
        EZTools::EZString _appName;
        EZTools::EZString _appVersion;
    };

}

#endif //EZTOOLS_EZAPP_H
