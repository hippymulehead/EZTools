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

#ifndef EZT_EZLINUX_H
#define EZT_EZLINUX_H

#include <chrono>
#include <thread>
#include <map>
#include <zconf.h>
#include "EZTools.h"

namespace EZLinux {

    typedef std::vector<EZTools::EZString> PATHSTOR;

    inline void EZLongSleep(long minutes) {
        std::chrono::minutes timespan(minutes);
        std::this_thread::sleep_for(timespan);
    }

    inline void EZSleep(long seconds) {
        std::chrono::seconds timespan(seconds);
        std::this_thread::sleep_for(timespan);
    }

    inline void EZNanoSleep(long milliseconds) {
        std::chrono::milliseconds timespan(milliseconds);
        std::this_thread::sleep_for(timespan);
    }

    inline EZTools::EZString environmentVar(EZTools::EZString name, EZTools::EZString defaultValue = "") {
        const char * val = std::getenv( name.c_str() );
        if ( val == nullptr ) {
            return defaultValue;
        }
        else {
            return EZTools::EZString(val);
        }
    }

    inline PATHSTOR path() {
        auto pstring = environmentVar("PATH");
        PATHSTOR sp = pstring.split(":");
        return sp;
    }

    inline EZTools::EZString hostname() {
        char hn[254];
        gethostname(hn, 254);
        EZTools::EZString hostname = hn;
        return hostname;
    }

    inline bool runningAsRoot() {
        return getuid() == 0;
    }

    inline int uid() {
        return getuid();
    }

    inline EZTools::EZString whoami() {
        char username[254];
        getlogin_r(username, 254);
        EZTools::EZString un = username;
        return un;
    }

    inline EZTools::EZReturn<EZTools::EZString> exec(EZTools::EZString command) {
        EZTools::EZReturn<EZTools::EZString> ezReturn;
        FILE *fp;
        std::stringstream output;
        char path[1035];
        fp = popen(command.c_str(), "r");
        if (fp == nullptr) {
            ezReturn.message("Failed to run " + command);
            ezReturn.exitCode(42);
            return ezReturn;
        }
        while (fgets(path, sizeof(path)-1, fp) != nullptr) {
            output << path;
        }
        pclose(fp);
        ezReturn.data = output.str();
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    class SystemdObject {
    public:
        SystemdObject() = default;
        SystemdObject(EZTools::EZString unit, EZTools::EZString load, EZTools::EZString active,
                EZTools::EZString sub, EZTools::EZString description) {
            _unit = unit;
            _load = load;
            _active = active;
            _sub = sub;
            _description = description;
        }
        ~SystemdObject() = default;
        void init(EZTools::EZString unit, EZTools::EZString load, EZTools::EZString active,
                  EZTools::EZString sub, EZTools::EZString description) {
            _unit = unit;
            _load = load;
            _active = active;
            _sub = sub;
            _description = description;
        }
        EZTools::EZString unit() { return _unit; }
        EZTools::EZString load() { return _load; }
        EZTools::EZString active() { return _active; }
        EZTools::EZString sub() { return _sub; }
        EZTools::EZString description() { return _description; }
    private:
        EZTools::EZString _unit;
        EZTools::EZString _load;
        EZTools::EZString _active;
        EZTools::EZString _sub;
        EZTools::EZString _description;
    };

    inline EZTools::EZReturn<std::vector<SystemdObject>> systemdList() {
        EZTools::EZReturn<std::vector<SystemdObject>> res;
        auto systemsdList = EZLinux::exec("/usr/bin/systemctl --no-pager");
        if (!systemsdList.wasSuccessful()) {
            res.message("Couldn't get systemd list");
            return res;
        }
        auto li = systemsdList.data.split("\n");
        for (auto& line : li) {
            line.removeNonPrintable();
            line.removeOpeningSpaces();
            if (line.regexMatch("^UNIT")) {

            } else if (line.empty()) {
                break;
            } else {
                line.removeExtraSpaces();
                auto sp = line.split(" ");
                std::stringstream d;
                for (int i = 4; i < sp.size(); i++) {
                    d << sp.at(i) << " ";
                }
                EZTools::EZString des = d.str();
                des.removeTrailingSpaces();
                SystemdObject sdo(sp.at(0), sp.at(1), sp.at(2), sp.at(3), des);
                res.data.push_back(sdo);
            }
        }
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<SystemdObject> getSystemdService(EZTools::EZString serviceName) {
        std::vector<SystemdObject> res;
        EZTools::EZReturn<SystemdObject> ret;
        auto sdl = systemdList();
        if (sdl.wasSuccessful()) {
            for (auto sdo : sdl.data) {
                if (sdo.unit() == serviceName+".service") {
                    res.push_back(sdo);
                }
            }
        } else {
            ret.message(sdl.message());
            return ret;
        }
        if (res.size() > 1) {
            ret.message("more than one service found");
            return ret;
        }
        if (res.empty()) {
            ret.message("service not found");
            return ret;
        }
        ret.wasSuccessful(true);
        ret.data.init(res.at(0).unit(), res.at(0).load(), res.at(0).active(),
                res.at(0).sub(), res.at(0).description());
        return ret;
    }

    inline EZTools::EZReturn<std::vector<SystemdObject>> getSystemdSub(EZTools::EZString subName) {
        EZTools::EZReturn<std::vector<SystemdObject>> res;
        auto sdl = systemdList();
        if (sdl.wasSuccessful()) {
            for (auto sdo : sdl.data) {
                if (sdo.sub() == subName) {
                    res.data.push_back(sdo);
                }
            }
        } else {
            res.message(sdl.message());
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

}

#endif //EZT_EZLINUX_H
