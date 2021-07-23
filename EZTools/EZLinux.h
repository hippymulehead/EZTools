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

#ifndef EZT_EZLINUX_H
#define EZT_EZLINUX_H

#include <chrono>
#include <thread>
#include <map>
#include <zconf.h>
#include <iomanip>
#include <sys/statvfs.h>
#include "EZTools.h"
#include "subprocess.h"
#include <fstream>
#include "json.h"
#include<stdlib.h>
#include<unistd.h>
#include<stdio.h>
#include<fcntl.h>

namespace EZLinux {

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

    inline EZTools::PATHSTOR path() {
        auto pstring = environmentVar("PATH");
        EZTools::PATHSTOR sp = pstring.split(":");
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
        std::stringstream ss;
        ss << command << " 2>&1";
        fp = popen(ss.str().c_str(), "r");
        if (fp == nullptr) {
            ezReturn.message("");
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

    inline std::vector<EZTools::EZString> userGroups(EZTools::EZString username) {
        std::stringstream ss;
        ss << "groups " << username;
        auto res = exec(ss.str());
        auto ts = res.data.split(" ");
        return ts;
    }

    inline EZTools::EZReturn<EZTools::EZString> subProcess(EZTools::EZString command) {
        EZTools::EZReturn<EZTools::EZString> ezReturn;
        std::vector<EZTools::EZString> cv;
        std::vector<std::string> a;
        EZTools::EZString c;
        if (command.regexMatch("^stat --printf=")) {
            auto ct = command.regexExtract("(stat) (--printf=\"[^\"]*\")");
            c = ct.at(0);
            a.emplace_back(ct.at(1));
        } else {
            cv = command.split(" ");
            c = cv.at(0);
            command.regexReplace(c+" ","");
            auto aa = command.split(" ");
            for (auto ap: aa) {
                a.push_back(ap.str());
            }
        }
        return ezReturn;
    }

    struct DiskInfo_t {
        double value;
        EZTools::EZString type;
        EZTools::EZString string;
    };

    class DiskInfo {
    public:
        explicit DiskInfo(EZTools::EZString dir) {
            _dir = dir;
            struct statvfs stat;
            if (statvfs(dir.c_str(), &stat) != 0) {
                _diskfree = -1;
            }
            _diskfree = stat.f_bsize * stat.f_bavail;
            _diskTotal = stat.f_bsize * stat.f_blocks;
            _diskUsed = _diskTotal - _diskfree;
        }
        virtual ~DiskInfo() = default;
        void update() {
            struct statvfs stat;
            if (statvfs(_dir.c_str(), &stat) != 0) {
                _diskfree = -1;
            }
            _diskfree = stat.f_bsize * stat.f_bavail;
            _diskTotal = stat.f_bsize * stat.f_favail;
        }
        unsigned long asGigs() const { return _diskfree / 1073741824; }
        unsigned long asMegs() const { return _diskfree / 1024 / 1024; }
        unsigned long asKb() const { return _diskfree / 1024; }
        unsigned long asBytes() const { return _diskfree; }
        DiskInfo_t free() {
            DiskInfo_t t;
            if (_diskfree < 1024) {
                t.type = "b";
                t.value = _diskfree;
            } else if (_diskfree < 1048576) {
                t.type = "k";
                t.value = _diskfree / 1024;
            } else if (_diskfree < 1073741824) {
                t.type = "M";
                t.value = _diskfree / 1024 / 1024;
            } else if (_diskfree < 1099511627776) {
                t.type = "G";
                t.value = _diskfree / 1024 / 1024 / 1024;
            } else {
                t.type = "T";
                t.value = _diskfree / 1024 / 1024 / 1024 / 1024;
            }
            std::stringstream ss;
            ss << EZTools::round(t.value, 2) << t.type;
            t.string = ss.str();
            return t;
        }
        DiskInfo_t total() {
            DiskInfo_t t;
            if (_diskTotal < 1024) {
                t.type = "b";
                t.value = _diskTotal;
            } else if (_diskTotal < 1048576) {
                t.type = "k";
                t.value = _diskTotal / 1024;
            } else if (_diskTotal < 1073741824) {
                t.type = "M";
                t.value = _diskTotal / 1024 / 1024;
            } else if (_diskTotal < 1099511627776) {
                t.type = "G";
                t.value = _diskTotal / 1024 / 1024 / 1024;
            } else {
                t.type = "T";
                t.value = _diskTotal / 1024 / 1024 / 1024 / 1024;
            }
            std::stringstream ss;
            ss << EZTools::round(t.value, 2) << t.type;
            t.string = ss.str();
            return t;
        }
        DiskInfo_t used() {
            DiskInfo_t t;
            if (_diskUsed < 1024) {
                t.type = "b";
                t.value = _diskUsed;
            } else if (_diskUsed < 1048576) {
                t.type = "k";
                t.value = _diskUsed / 1024;
            } else if (_diskUsed < 1073741824) {
                t.type = "M";
                t.value = _diskUsed / 1024 / 1024;
            } else if (_diskUsed < 1099511627776) {
                t.type = "G";
                t.value = _diskUsed / 1024 / 1024 / 1024;
            } else {
                t.type = "T";
                t.value = _diskUsed / 1024 / 1024 / 1024 / 1024;
            }
            std::stringstream ss;
            ss << EZTools::round(t.value, 2) << t.type;
            t.string = ss.str();
            return t;
        }
        DiskInfo_t percentFree() {
            DiskInfo_t t;
            double p;
            p = _diskfree / _diskTotal * 100;
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << p << "%";
            t.type = "Pe";
            t.string = ss.str();
            t.value = p;
            return t;
        }
    private:
        double _diskfree = 0;
        double _diskTotal = 0;
        double _diskUsed = 0;
        EZTools::EZString _dir;
    };

    enum MOUNTS {
        EZ_LOCAL,
        EZ_REMOTE,
        EZ_TMPFS,
        EZ_FS_UNKNOWN
    };

    inline std::string mountsAsString(MOUNTS t) {
        switch (t) {
            case EZ_LOCAL:
                return "Local";
            case EZ_REMOTE:
                return "Remote";
            case EZ_TMPFS:
                return "TmpFS";
            case EZ_FS_UNKNOWN:
                return "Unknown";
        }
        return "Unknown";
    }

    struct Mounts_t {
        MOUNTS type;
        EZTools::EZString mount;
    };

    class Mounts {
    public:
        Mounts() {
            std::string lines;
            std::stringstream f;
            std::ifstream mtab ("/etc/mtab");
            if (mtab.is_open()) {
                while ( getline (mtab,lines) ) {
                    f << lines << '\n';
                }
                mtab.close();
            }
            EZTools::EZString ms = f.str();
            auto msv = ms.split("\n");
            for (auto& line : msv) {
                if (!line.empty()) {
                    auto data = line.split(" ");
                    if (data.at(0).regexMatch("^/|[:]|^tmpfs")) {
                        Mounts_t t;
                        t.type = EZ_FS_UNKNOWN;
                        if (data.at(0).regexMatch("^/")) {
                            t.type = EZ_LOCAL;
                        } else if (data.at(0).regexMatch(":")) {
                            t.type = EZ_REMOTE;
                        } else if (data.at(0).regexMatch("^tmpfs")) {
                            t.type = EZ_TMPFS;
                        }
                        t.mount = data.at(1);
                        _mounts.emplace_back(t);
                    }
                }
            }
        }
        ~Mounts() = default;
        std::vector<Mounts_t> list() { return _mounts; }
    private:
        std::vector<Mounts_t> _mounts;
    };

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
        res.metaData.location = "EZLinux::systemdList";
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
        ret.metaData.location = "EZLinux::getSystemdService";
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
        res.metaData.location = "EZLinux::getSystemdSub";
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

    inline EZTools::EZReturn<nlohmann::json> whois(EZTools::EZString ipaddress) {
        EZTools::EZReturn<nlohmann::json> ret;
        auto res = EZLinux::subProcess("whois "+ipaddress);
        std::cout << res.data << std::endl;
        return ret;
    }

//    inline EZTools::TEST_RETURN TEST() {
//        EZTools::TEST_RETURN res("EZLinux", false);
//        auto env = environmentVar("EDITOR");
//        if (env.empty()) {
//            res.functionTest("environmentVar(\"EDITOR\")");
//            res.message("Not found");
//            return res;
//        }
//        res.output << "\tenvironmentVar(\"EDITOR\"): " << env << std::endl;
//        auto p = path();
//        if (p.empty()) {
//            res.functionTest("path()");
//            res.message("Empty");
//            return res;
//        }
//        res.output << "path()" << std::endl;
//        for (auto& pp : p) {
//            res.output << "\t" << pp << std::endl;
//        }
//        auto h = hostname();
//        if (h.empty()) {
//            res.functionTest("hostname()");
//            res.message("Empty");
//            return res;
//        }
//        res.output << "hostname(): " << h << std::endl;
//        res.output << "runningAsRoot(): " << std::boolalpha << runningAsRoot() << std::endl;
//        res.output << "uid(): " << uid() << std::endl;
//        auto w = whoami();
//        if (w.empty()) {
//            res.functionTest("whoami()");
//            res.message("Empty");
//            return res;
//        }
//        res.output << "whoami(): " << w << std::endl;
//        auto ls = exec("ls");
//        if (!ls.wasSuccessful()) {
//            res.functionTest("exec(\"ls\")");
//            res.message(ls.message());
//            return res;
//        }
//        res.output << "exec(\"ls\"):" << std::endl;
//        res.output << ls.data << std::endl;
//        DiskInfo d("/");
//        res.output << "DiskInfo:" << std::endl;
//        res.output << "\tasGigs(): " << d.asGigs() << std::endl;
//        res.output << "\tasMegs(): " << d.asMegs() << std::endl;
//        res.output << "\tasKb(): " << d.asKb() << std::endl;
//        res.output << "\tasBytes(): " << d.asBytes() << std::endl;
//        res.output << "\td.free().string: " << d.free().string << std::endl;
//        res.output << "\td.free().value: " << d.free().value << std::endl;
//        res.output << "\td.total().string: " << d.total().string << std::endl;
//        res.output << "\td.total().value: " << d.total().value << std::endl;
//        res.output << "\td.used().string: " << d.used().string << std::endl;
//        res.output << "\td.used().value: " << d.used().value << std::endl;
//        res.output << "\td.percentFree().string: " << d.percentFree().string << std::endl;
//        res.output << "\td.percentFree().value: " << d.percentFree().value << std::endl;
//        Mounts m;
//        res.output << "Mounts" << std::endl;
//        for (auto& mm: m.list()) {
//            res.output << "\t" << mountsAsString(mm.type) << " " << mm.mount << std::endl;
//        }
//        auto sdl = systemdList();
//        if (!sdl.wasSuccessful()) {
//            res.functionTest("systemdList()");
//            res.message(sdl.message());
//            return res;
//        }
//        res.output << "systemdList()";
//        for (auto& s : sdl.data) {
//            res.output << "\t'" << s.active() << "' '" << s.unit() << "' '" << s.sub() << "' '"
//                       << s.description() << "'" << std::endl;
//        }
//        auto sss = getSystemdService("sssd");
//        if (!sss.wasSuccessful()) {
//            res.functionTest("getSystemdService(\"sssd\")");
//            res.message(sss.message());
//            return res;
//        }
//        res.output << "\tgetSystemdService(\"sssd\"): " << sss.data.active() << " "
//                   << sss.data.unit() << std::endl;
//        res.output << "getSystemdSub(\"active\")" << std::endl;
//        auto sub = getSystemdSub("active");
//        if (!sub.wasSuccessful()) {
//            res.functionTest("getSystemdSub(\"active\")");
//            res.message(sub.message());
//            return res;
//        }
//        for (auto& a: sub.data) {
//            res.output << "\t" << a.unit() << std::endl;
//        }
//        res.wasSuccessful(true);
//        return res;
//    }

}

#endif //EZT_EZLINUX_H
