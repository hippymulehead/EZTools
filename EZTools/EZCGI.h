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

#ifndef EZT_EZCGI_H
#define EZT_EZCGI_H

#pragma once

#include "EZTools.h"
#include "EZHTML.h"
#include "EZHTTP.h"

namespace EZCGI {

    enum CGICLOSE {
        SUCCESS,
        FAIL
    };

    struct EnvVar {
        EZTools::EZString name;
        EZTools::EZString value;
        EnvVar() = default;
        EnvVar(EZTools::EZString Name, EZTools::EZString Value) {
            name = Name;
            value = Value;
        }
    };

    class EZCGIApp {
    public:
        EZCGIApp(char *envp[], EZTools::EZString appName, EZTools::EZString contentType,
                 EZTools::EZString appVersion = VERSION) {
            _appName = appName;
            _appVersion = appVersion;
            for (char **env = envp; *env != 0; env++) {
                char *thisEnv = *env;
                EZTools::EZString t(thisEnv);
                auto tt = t.splitAtFirst("=");
                EnvVar y;
                if (tt.size() > 0) {
                    if (tt.size() == 2) {
                        EnvVar e(tt.at(0), tt.at(1));
                        y = e;
                    } else if (tt.size() == 1) {
                        EnvVar e(tt.at(0), "");
                        y = e;
                    }
                    if (!y.name.empty()) {
                        _envVars.emplace_back(y);
                    }
                }
            }
            EnvVar e;
            for (auto& n : _envVars) {
                if (n.name == "QUERY_STRING") {
                    e = n;
                }
            }
            auto v = e.value.split(";");
            for (auto& l : v) {
                EZTools::EZString one;
                EZTools::EZString two;
                auto sp = l.split("=");
                if (sp.size() == 1) {
                    one = sp.at(0);
                }
                if (sp.size() == 2) {
                    one = sp.at(0);
                    two = sp.at(1);
                }
                EnvVar vv(one, two);
                _vars.emplace_back(vv);
            }
            _contentType = contentType;
            std::cout << "Content-type: " << contentType << "\n\n";
            _title = _appName+" v"+_appVersion;
        }
        ~EZCGIApp() = default;
        EZTools::EZString title() { return _title; }
        EZTools::EZString getVar(EZTools::EZString var, EZTools::EZString def = "") {
            for (auto v : _vars) {
                if (v.name == var) {
                    return v.value;
                }
            }
            return def;
        }
        EZTools::EZString getEnvVar(EZTools::EZString envVar) {
            for (auto& e : _envVars) {
                if (e.name == envVar) {
                    return e.value;
                }
            }
            return EZTools::EZString();
        }
        bool isMobile() {
            auto browser = getEnvVar("HTTP_USER_AGENT");
            if (browser.lower().regexMatch("iphone|ipad|ipod|android")) {
                return true;
            }
            return false;
        }
        bool isChrome() {
            auto browser = getEnvVar("HTTP_USER_AGENT");
            if (browser.lower().regexMatch("chrome")) {
                return true;
            }
            return false;
        }
        bool isFirefox() {
            auto browser = getEnvVar("HTTP_USER_AGENT");
            if (browser.lower().regexMatch("firefox")) {
                return true;
            }
            return false;
        }
        bool isSafari() {
            auto browser = getEnvVar("HTTP_USER_AGENT");
            if (browser.lower().regexMatch("safari")) {
                return true;
            }
            return false;
        }
        bool isEdge() {
            auto browser = getEnvVar("HTTP_USER_AGENT");
            if (browser.lower().regexMatch("edge")) {
                return true;
            }
            return false;
        }
        EZHTML::HTMLOutput out;
        void close(CGICLOSE type) {
            switch (type) {
                case SUCCESS:
                    exit(EXIT_SUCCESS);
                case FAIL:
                    exit(EXIT_FAILURE);
            }
        }
        void contentType(EZTools::EZString type) { _contentType = type; }
        std::vector<EnvVar> env() { return _envVars; }
        std::vector<EnvVar> vars() { return _vars; }

    private:
        EZTools::EZString _appName;
        EZTools::EZString _title;
        EZTools::EZString _appVersion;
        std::vector<EnvVar> _vars;
        EZTools::EZString _contentType;
        std::vector<EnvVar> _envVars;
    };

}

#endif //EZT_EZCGI_H
