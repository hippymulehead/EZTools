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

#ifndef EZT_EZLINUX_H
#define EZT_EZLINUX_H

#pragma once

#include <chrono>
#include <thread>
#include <map>
#include <zconf.h>
#include <pwd.h>
#include <grp.h>
#include <iomanip>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fstream>
#include <cstdlib>
#include <unistd.h>
#include <cstdio>
#include <fcntl.h>
#include <dirent.h>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <sstream>
#include "EZTools.h"
#include "nlohmann/json.hpp"

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

//    inline void execute(char **argv) {
//        pid_t  pid;
//        int    childStatus;
//
//        if ((pid = fork()) < 0) {     /* fork a childPID process           */
//            printf("*** ERROR: forking childPID process failed\n");
//            exit(1);
//        }
//        else if (pid == 0) {          /* for the childPID process:         */
//            if (execvp(*argv, argv) < 0) {     /* execute the command  */
//                printf("*** ERROR: exec failed\n");
//                exit(1);
//            }
//        } else {
//            while (wait(&childStatus) != pid);
//        }
//    }

//    inline bool execVP(EZTools::EZString command) {
//        auto sp = command.splitAtFirst(" ");
//        EZTools::EZString args;
//        if (sp.size() > 1) {
//            args = sp.at(1);
//        }
//        const char* c = strcpy(new char[args.length() + 1], args.c_str());
//        return execvp(sp.at(0).c_str(), c);
//    }

    inline EZTools::EZReturn<EZTools::EZString> exec(EZTools::EZString command) {
        EZTools::EZReturn<EZTools::EZString> ezReturn;
        std::stringstream output;
        ezReturn.location("EZLinux::exec");
        char path[1035];
        std::stringstream ss;
        ss << command << ";echo $?";
        auto fp = popen(ss.str().c_str(), "r");
        if (fp == nullptr) {
            ezReturn.message("");
            ezReturn.exitCode(42);
            return ezReturn;
        }
        while (fgets(path, sizeof(path)-1, fp) != nullptr) {
            output << path;
        }
        pclose(fp);
        EZTools::EZString ous = output.str();

        while (ous.regexMatch("\n$")) {
            ous.regexReplace("\n$", "");
        }
        auto sp = ous.split("\n");
        std::vector<EZTools::EZString> g;
        EZTools::EZString ec = sp.back();
        sp.erase(sp.end());
        std::stringstream ops;
        for (auto& o : sp) {
            ops << o << std::endl;
        }
        ezReturn.exitCode(ec.asInt());
        if (ezReturn.exitCode() != 0) {
            std::stringstream sts;
            sts << ops.str() << "Command: '"+command+"' failed exitcode: "+ec;
            ezReturn.message(sts.str());
            return ezReturn;
        }
        ezReturn.data = ops.str();
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

//    inline EZTools::EZReturn<EZTools::EZString> exec(EZTools::EZString command) {
//        EZTools::EZReturn<EZTools::EZString> ezReturn;
//        FILE *fp;
//        std::stringstream output;
//        char path[1035];
//        std::stringstream ss;
//        ss << command << " 2>&1";
//        fp = popen(ss.str().c_str(), "r");
//        if (fp == nullptr) {
//            ezReturn.message("");
//            ezReturn.exitCode(42);
//            return ezReturn;
//        }
//        while (fgets(path, sizeof(path)-1, fp) != nullptr) {
//            output << path;
//        }
//        pclose(fp);
//        ezReturn.data = output.str();
//        ezReturn.wasSuccessful(true);
//        return ezReturn;
//    }
//
//    inline std::vector<EZTools::EZString> userGroups(EZTools::EZString username) {
//        std::stringstream ss;
//        ss << "groups " << username;
//        auto res = exec(ss.str());
//        auto ts = res.data.split(" ");
//        return ts;
//    }
//
//    inline EZTools::EZReturn<EZTools::EZString> subProcess(EZTools::EZString command) {
//        EZTools::EZReturn<EZTools::EZString> ezReturn;
//        std::vector<EZTools::EZString> cv;
//        std::vector<std::string> a;
//        EZTools::EZString c;
//        if (command.regexMatch("^stat --printf=")) {
//            auto ct = command.regexExtract("(stat) (--printf=\"[^\"]*\")");
//            c = ct.at(0);
//            a.emplace_back(ct.at(1));
//        } else {
//            cv = command.split(" ");
//            c = cv.at(0);
//            command.regexReplace(c+" ","");
//            auto aa = command.split(" ");
//            for (auto ap: aa) {
//                a.push_back(ap.str());
//            }
//        }
//        return ezReturn;
//    }

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
        res.location("EZLinux::systemdList");
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
        ret.location("EZLinux::getSystemdService");
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
        res.location("EZLinux::getSystemdSub");
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
        auto res = EZLinux::exec("whois "+ipaddress);
        std::cout << res.data << std::endl;
        return ret;
    }

    enum RPMType_t {
        YUM,
        DNF,
        DEB,
        UNKNOWN
    };

    class OSRelease {
    public:
        OSRelease() {
            EZTools::EZString strbuf;
            std::ifstream MyReadFile("/etc/os-release");
            while (getline (MyReadFile, strbuf)) {
                strbuf.regexReplace("\"","");
                if (strbuf.regexMatch("^NAME=")) {
                    auto da = strbuf.split("=");
                    _name = da.at(1);
                } else if (strbuf.regexMatch("^VERSION_ID=")) {
                    auto da = strbuf.split("=");
                    _version_id = da.at(1);
                } else if (strbuf.regexMatch("^VERSION=")) {
                    auto da = strbuf.split("=");
                    _version = da.at(1);
                } else if (strbuf.regexMatch("^ID=")) {
                    auto da = strbuf.split("=");
                    _id = da.at(1);
                } else if (strbuf.regexMatch("^PRETTY_NAME=")) {
                    auto da = strbuf.split("=");
                    _pretty_name = da.at(1);
                } else if (strbuf.regexMatch("^ID_LIKE=")) {
                    auto da = strbuf.split("=");
                    _id_like = da.at(1);
                }
            }
            MyReadFile.close();
        }
        ~OSRelease() = default;
        EZTools::EZString name() { return _name; }
        EZTools::EZString id() { return _id; }
        EZTools::EZString versionID() { return _version_id; }
        EZTools::EZString version() { return _version; }
        EZTools::EZString prettyName() { return _pretty_name; }
        EZTools::EZString idLike() {return _id_like; }
        RPMType_t packageManagerName() {
            if (_name.lower().regexMatch("fedora")) {
                return DNF;
            } else if (_name.lower().regexMatch("centos")) {
                return _version_id.asInt() < 8 ? YUM : DNF;
            } else if (_id_like.lower().regexMatch("debian") || _name.lower().regexMatch("debian")) {
                return DEB;
            }
            return UNKNOWN;
        }
        EZTools::EZString packageManagerNameAsString() {
            if (_name.lower().regexMatch("fedora")) {
                return "DNF";
            } else if (_name.lower().regexMatch("centos")) {
                return _version_id.asInt() < 8 ? "YUM" : "DNF";
            } else if (_id_like.lower().regexMatch("debian") || _name.lower().regexMatch("debian")) {
                return "DEB";
            }
            return "UNKNOWN";
        }
    private:
        EZTools::EZString _name;
        EZTools::EZString _version;
        EZTools::EZString _id;
        EZTools::EZString _id_like;
        EZTools::EZString _version_id;
        EZTools::EZString _pretty_name;
    };

//    inline unsigned checkDNFUpdates() {
//        auto mc = exec("dnf makecache");
//        auto cu = exec("dnf check-update");
//        auto cuList = cu.data.split("\n");
//        std::vector<EZTools::EZString> yep;
//        for (auto& c : cuList) {
//            if (!c.regexMatch("^Last metadata expiration check") && !c.empty()) {
//                yep.emplace_back(c);
//            }
//        }
//        return yep.size();
//    }
//
//    inline unsigned checkYUMUpdates() {
//        auto mc = exec("yum makecache");
//        auto cu = exec("yum check-update");
//        auto cuList = cu.data.split("\n");
//        std::vector<EZTools::EZString> yep;
//        for (auto& c : cuList) {
//            if (!c.regexMatch("^Last metadata expiration check") && !c.empty()) {
//                yep.emplace_back(c);
//            }
//        }
//        return yep.size();
//    }

    struct Package_t {
        EZTools::EZString packageBaseName;
        EZTools::EZString packageVersion;
        EZTools::EZString packageArc;
        EZTools::EZString packageFullName;
        EZTools::EZString packageRepo;
    };

    inline void printVector(std::vector<EZTools::EZString> v) {
        for (auto vv : v) {
            std::cout << vv << std::endl;
        }
    }

    inline std::vector<EZLinux::Package_t> yumCreatePackage_t(std::vector<EZTools::EZString> data,
                                                              EZTools::EZString regexIgnoreLine = "") {
        std::vector<EZLinux::Package_t> yep;
        bool hitTop = false;
        for (auto& c : data) {
            if (!c.regexMatch(regexIgnoreLine) && !c.empty() && hitTop) {
                EZLinux::Package_t p;
                c.removeExtraSpaces();
                auto sp1 = c.split(" ");
                auto sp2 = sp1.at(0).split(".");
                p.packageBaseName = sp2.at(0);
                p.packageArc = sp2.at(1);
                p.packageVersion = sp1.at(1);
                p.packageRepo = sp1.at(2).regexReplace("[@]", "");
                std::stringstream s;
                s << p.packageBaseName << "." << p.packageVersion << "." << p.packageArc;
                p.packageFullName = s.str();
                yep.push_back(p);
            }
            if (c.empty()) {
                hitTop = true;
            }
        }
        return yep;
    }

    inline std::vector<Package_t> createPackage_t(std::vector<EZTools::EZString> data) {
        std::vector<Package_t> yep;
        bool hitend = false;
        for (auto& c : data) {
            if (!c.regexMatch("^Last metadata expiration check:")
                && !c.regexMatch("Installed Packages")
                && !c.empty() && !hitend) {
                Package_t p;
                c.removeExtraSpaces();
                if (c.regexMatch("Available Packages")) {
                    hitend = true;
                } else {
                    auto sp1 = c.split(" ");
                    auto sp2 = sp1.at(0).split(".");
                    p.packageBaseName = sp2.at(0);
                    p.packageArc = sp2.at(1);
                    p.packageVersion = sp1.at(1);
                    p.packageRepo = sp1.at(2).regexReplace("[@]", "");
                    std::stringstream s;
                    s << p.packageBaseName << "." << p.packageVersion << "." << p.packageArc;
                    p.packageFullName = s.str();
                    yep.push_back(p);
                }
            }
        }
        return yep;
    }

    enum RPMList_t {
        INSTALLED,
        AVAILABLE
    };

    inline std::pair<unsigned, std::vector<Package_t>> RPMList(RPMType_t type, RPMList_t comm) {
        std::pair<unsigned, std::vector<Package_t>> res;
        EZTools::EZString pm = "dnf";
        if (type == YUM) {
            pm = "yum";
        }
        std::stringstream s;
        if (comm == INSTALLED) {
            s << pm << " list installed";
        } else {
            s << pm << " list available";
        }
        auto mc = exec(s.str());
        auto cuList = mc.data.split("\n");
        auto yep = createPackage_t(cuList);
        res.first = yep.size();
        res.second = yep;
        return res;
    }

    inline std::vector<EZLinux::Package_t> checkRPMUpdates(EZTools::EZString data, EZLinux::RPMType_t type, EZTools::EZString &yumIgnore) {
        auto cuList = data.split("\n");
        if (type == EZLinux::DNF) {
            auto res = EZLinux::createPackage_t(cuList);
            return res;
        } else if (type == EZLinux::YUM) {
            std::vector<EZLinux::Package_t> res;
            if (yumIgnore.empty()) {
                res = yumCreatePackage_t(cuList);
            } else {
                res = yumCreatePackage_t(cuList, yumIgnore);
            }
            return res;
        }
        std::vector<EZLinux::Package_t> derp;
        return derp;
    }

    inline std::vector<Package_t> checkDEBUpdates() {
        std::vector<Package_t> res;
        //FIXME: For obvious reasons that your mom warned you about
        return res;
    }

    inline std::vector<EZLinux::Package_t> checkUpdates(EZLinux::RPMType_t type, EZTools::EZString yumIgnore = EZTools::EZString()) {
        std::vector<EZLinux::Package_t> res;
        EZTools::EZString pm = "dnf";
        if (type == EZLinux::YUM) {
            pm = "yum";
        }
        std::stringstream s;
        std::stringstream p;
        s << pm << " makecache";
        p << pm << " check-update";
        auto mc = EZLinux::exec(s.str());
        auto cu = EZLinux::exec(p.str());
        std::vector<EZLinux::Package_t> yep;
        res = checkRPMUpdates(cu.data, type, yumIgnore);
        return res;
    }

    inline EZTools::EZString whoami() {
        auto res = exec("whoami");
        return res.data.chomp();
    }

    inline EZTools::EZString whoamireally() {
        char username[254];
        getlogin_r(username, 254);
        EZTools::EZString un = username;
        return un;
    }

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZLinux");
        std::stringstream ss;
        auto env = environmentVar("PATH");
        if (env.empty()) {
            res.functionTest("environmentVar(\"PATH\")");
            res.message("Not found");
            return res;
        }
        ss << "\tenvironmentVar(\"PATH\"): " << env << std::endl;
        auto p = path();
        if (p.empty()) {
            res.functionTest("path()");
            res.message("Empty");
            return res;
        }
        ss << "path()" << std::endl;
        for (auto& pp : p) {
            ss << "\t" << pp << std::endl;
        }
        auto h = hostname();
        if (h.empty()) {
            res.functionTest("hostname()");
            res.message("Empty");
            return res;
        }
        ss << "hostname(): " << h << std::endl;
        ss << "runningAsRoot(): " << std::boolalpha << runningAsRoot() << std::endl;
        ss << "uid(): " << uid() << std::endl;
        auto w = whoami();
        if (w.empty()) {
            res.functionTest("whoami()");
            res.message("Empty");
            return res;
        }
        ss << "whoami(): " << w << std::endl;
        auto ls = exec("ls");
        if (!ls.wasSuccessful()) {
            res.functionTest("exec(\"ls\")");
            res.message(ls.message());
            return res;
        }
        ss << "exec(\"ls\"):" << std::endl;
        ss << ls.data << std::endl;
        DiskInfo d("/");
        ss << "DiskInfo:" << std::endl;
        ss << "\tasGigs(): " << d.asGigs() << std::endl;
        ss << "\tasMegs(): " << d.asMegs() << std::endl;
        ss << "\tasKb(): " << d.asKb() << std::endl;
        ss << "\tasBytes(): " << d.asBytes() << std::endl;
        ss << "\td.free().string: " << d.free().string << std::endl;
        ss << "\td.free().valueInSeconds: " << d.free().value << std::endl;
        ss << "\td.total().string: " << d.total().string << std::endl;
        ss << "\td.total().valueInSeconds: " << d.total().value << std::endl;
        ss << "\td.used().string: " << d.used().string << std::endl;
        ss << "\td.used().valueInSeconds: " << d.used().value << std::endl;
        ss << "\td.percentFree().string: " << d.percentFree().string << std::endl;
        ss << "\td.percentFree().valueInSeconds: " << d.percentFree().value << std::endl;
        Mounts m;
        ss << "Mounts" << std::endl;
        for (auto& mm: m.list()) {
            ss << "\t" << mountsAsString(mm.type) << " " << mm.mount << std::endl;
        }
        auto sdl = systemdList();
        if (!sdl.wasSuccessful()) {
            res.functionTest("systemdList()");
            res.message(sdl.message());
            return res;
        }
        ss << "systemdList()";
        for (auto& s : sdl.data) {
            ss << "\t'" << s.active() << "' '" << s.unit() << "' '" << s.sub() << "' '"
               << s.description() << "'" << std::endl;
        }
        auto sss = getSystemdService("NetworkManager");
        if (!sss.wasSuccessful()) {
            res.functionTest("getSystemdService(\"NetworkManager\")");
            res.message(sss.message());
            return res;
        }
        ss << "\tgetSystemdService(\"NetworkManager\"): " << sss.data.active() << " "
           << sss.data.unit() << std::endl;
        ss << "getSystemdSub(\"active\")" << std::endl;
        auto sub = getSystemdSub("active");
        if (!sub.wasSuccessful()) {
            res.functionTest("getSystemdSub(\"active\")");
            res.message(sub.message());
            return res;
        }
        for (auto& a: sub.data) {
            ss << "\t" << a.unit() << std::endl;
        }
        res.output(ss.str());
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<std::vector<EZTools::EZString>> groups() {
        EZTools::EZReturn<std::vector<EZTools::EZString>> g;
        auto res = exec("groups");
        if (!res.wasSuccessful()) {
            g.message("Can't run groups command");
            return g;
        }
        g.data = res.data.split(" ");
        g.wasSuccessful(true);
        return g;
    }

    inline std::vector<EZTools::EZString> getDirs(EZTools::EZString base, bool includeHidden = false) {
        std::vector<EZTools::EZString> re;
        struct dirent *entry = nullptr;
        DIR *dp = nullptr;
        dp = opendir(base.c_str());
        if (dp != nullptr) {
            while ((entry = readdir(dp))) {
                int t = int(entry->d_type);
                EZTools::EZString s = EZTools::EZString(entry->d_name);
                if (t == 4) {
                    if (includeHidden) {
                        re.emplace_back(entry->d_name);
                    } else if (!s.regexMatch("^\\.")) {
                        re.emplace_back(entry->d_name);
                    }
                }
            }
        }
        closedir(dp);
        return re;
    }

    inline EZTools::EZReturn<bool> changeDir(EZTools::EZString dir) {
        EZTools::EZReturn<bool> re;
        if (chdir(dir.c_str()) != 0) {
            std::stringstream ss;
            ss << "Could not chdir to " << dir;
            re.message(ss.str());
            return re;
        }
        re.wasSuccessful(true);
        return re;
    }

    struct perms_t {
        bool sticky = false;
        bool dir = false;
        bool ownerRead = false;
        bool ownerWrite = false;
        bool ownerExec = false;
        bool groupRead = false;
        bool groupWrite = false;
        bool groupExec = false;
        bool otherRead = false;
        bool otherWrite = false;
        bool otherExec = false;
        EZTools::EZString owner;
        EZTools::EZString group;
        EZTools::EZString permsString;
        EZTools::EZString pathName;
        bool isThere = false;
    };

    // This is a limited URI used in execAndPassControl() Don't call direct.  Use EZFiles::URI instead.
    class URI {
    public:
        URI() {
            _p.clear();
            _p.append("/");
        }
        explicit URI(EZTools::EZString path) {
            _perms.pathName = path;
            _perms.isThere = std::filesystem::exists(std::filesystem::status(_perms.pathName.c_str()));
            std::filesystem::path pp(_perms.pathName.str());
            _p = pp;
            auto p = std::filesystem::status(_p.string()).permissions();
            struct stat FileInfo {};
            struct passwd *UserDatabaseEntry  = getpwuid(FileInfo.st_uid);
            struct group  *GroupDatabaseEntry = getgrgid(FileInfo.st_gid);
            _perms.owner = UserDatabaseEntry->pw_name;
            _perms.group = GroupDatabaseEntry->gr_name;
            if ((p & std::filesystem::perms::sticky_bit) != std::filesystem::perms::none) {
                _perms.sticky = true;
            }
            if (this->isDirectory()) {
                _perms.dir = true;
            }
            if ((p & std::filesystem::perms::owner_read) != std::filesystem::perms::none) {
                _perms.ownerRead = true;
            }
            if ((p & std::filesystem::perms::owner_write) != std::filesystem::perms::none) {
                _perms.ownerWrite = true;
            }
            if ((p & std::filesystem::perms::owner_exec) != std::filesystem::perms::none) {
                _perms.ownerExec = true;
            }
            if ((p & std::filesystem::perms::group_read) != std::filesystem::perms::none) {
                _perms.groupRead = true;
            }
            if ((p & std::filesystem::perms::group_write) != std::filesystem::perms::none) {
                _perms.groupWrite = true;
            }
            if ((p & std::filesystem::perms::group_exec) != std::filesystem::perms::none) {
                _perms.groupExec = true;
            }
            if ((p & std::filesystem::perms::others_read) != std::filesystem::perms::none) {
                _perms.otherRead = true;
            }
            if ((p & std::filesystem::perms::others_write) != std::filesystem::perms::none) {
                _perms.otherWrite = true;
            }
            if ((p & std::filesystem::perms::others_exec) != std::filesystem::perms::none) {
                _perms.otherExec = true;
            }
        }
        ~URI() = default;
        bool exists() {
            return std::filesystem::exists(_p);
        }
        bool isRegularFile() {
            return std::filesystem::is_regular_file(_p);
        }
        bool isDirectory() {
            return std::filesystem::is_directory(_p);
        }
        bool isBlockDevice() {
            return std::filesystem::is_block_file(_p);
        }
        bool isCharactorFile() {
            return std::filesystem::is_character_file(_p);
        }
        bool isFifo() {
            return std::filesystem::is_fifo(_p);
        }
        bool isOther() {
            return std::filesystem::is_other(_p);
        }
        bool isSocket() {
            return std::filesystem::is_socket(_p);
        }
        bool isSymlink() {
            return std::filesystem::is_symlink(_p);
        }
        EZTools::EZString path() { return _perms.pathName; }
        bool canRead() {
            if (isThere()) {
                auto me = EZLinux::whoami();
                if (_perms.owner == me && _perms.ownerRead) {
                    return true;
                }
                auto groups = EZLinux::groups();
                for (auto &g: groups.data) {
                    if (g == _perms.group && _perms.groupRead) {
                        return true;
                    }
                }
                if (_perms.otherRead) {
                    return true;
                }
                return false;
            } else {
                return false;
            }
        }
        bool canWrite() {
            if (isThere()) {
                auto me = EZLinux::whoami();
                if (_perms.owner == me && _perms.ownerWrite) {
                    return true;
                }
                auto groups = EZLinux::groups();
                for (auto &g: groups.data) {
                    if (g == _perms.group && _perms.groupWrite) {
                        return true;
                    }
                }
                if (_perms.otherWrite) {
                    return true;
                }
                return false;
            } else {
                auto bp = _perms.pathName.splitAtLast("/");
                URI uri(bp.at(0));
                return uri.canWrite();
            }
        }
        bool canExec() {
            if (isThere()) {
                auto me = EZLinux::whoami();
                if (_perms.owner == me && _perms.ownerExec) {
                    return true;
                }
                auto groups = EZLinux::groups();
                for (auto &g: groups.data) {
                    if (g == _perms.group && _perms.groupExec) {
                        return true;
                    }
                }
                if (_perms.otherExec) {
                    return true;
                }
                return false;
            } else {
                return false;
            }
        }
        bool isThere() {
            return _perms.isThere;
        }
    private:
        std::filesystem::path _p;
        perms_t _perms;
    };

    inline char* new_c_str_from_string(const std::string& str) {
        char* c_str = new char[str.length()+1];
        std::copy(str.begin(), str.end(), c_str);
        c_str[str.length()] = '\0';
        return c_str;
    }

    inline void delete_c_str(char* c_str) {
        delete[] c_str;
    }

    struct PInfo_T {
        pid_t childPID;
        int childStatus = 0;
        int childExitCode = 0;
    };

    template<typename It>
    inline PInfo_T exec_vp_backend(std::string file, It begin, It end) {
        std::vector<char*> argv;
        argv.push_back(new_c_str_from_string(file));
        std::transform(begin, end, std::back_inserter(argv), new_c_str_from_string);
        argv.push_back(nullptr);
        PInfo_T t{};
        if(!(t.childPID = fork())) {
            t.childExitCode = execvp(file.c_str(), &argv[0]);
        }
        waitpid(t.childPID, &t.childStatus, 0);
        std::for_each(argv.begin(), argv.end(), delete_c_str);
        return t;
    }

    EZTools::EZReturn<URI> checkFileThereInPath(EZTools::EZString command) {
        EZTools::EZReturn<URI> uri;
        auto p = EZLinux::path();
        for (auto& pn : p) {
            std::stringstream ss;
            ss << pn << "/" << command;
            if (exists(std::filesystem::status(ss.str()))) {
                URI u(ss.str());
                uri.data = u;
                uri.wasSuccessful(true);
                return uri;
            }
        }
        uri.message("File not found");
        return uri;
    }

    inline EZTools::EZReturn<PInfo_T> execAndPassControl(EZTools::EZString command) {
        EZTools::EZReturn<PInfo_T> res;
        std::istringstream command_stream(command);
        std::istream_iterator<std::string> command_iterator(command_stream);
        std::string file = *(command_iterator++);
        EZTools::EZString pa;
        EZTools::EZString tfile = file;
        if (tfile.regexMatch("^\\/|^\\.")) {
            URI uri(file);
            if (!uri.isThere() || !uri.canExec()) {
                res.message("Can't execute '" + file + "'");
                return res;
            }
            pa = uri.path();
        } else {
            auto u = checkFileThereInPath(file);
            if (u.data.canExec()) {
                pa = u.data.path();
            }
        }
        if (!pa.empty()) {
            auto re = exec_vp_backend(file, command_iterator, std::istream_iterator<std::string>());
            res.data.childStatus = re.childStatus;
            res.data.childExitCode = re.childExitCode;
            res.data.childPID = re.childPID;
            if (re.childExitCode == 0 && re.childStatus != 0) {
                res.exitCode(re.childExitCode);
                res.message("Status for '" + command + "' returned a " + EZTools::EZString(re.childStatus));
                res.wasSuccessful(false);
            } else if (re.childExitCode == 0) {
                res.wasSuccessful(true);
            } else {
                res.exitCode(re.childExitCode);
                std::stringstream ss;
                ss << "Running '" << command << "' Exit code : " << re.childExitCode << "-" << re.childStatus;
                res.message(ss.str());
            }
        } else {
            res.exitCode(-1);
            std::stringstream ss;
            ss << "Command not found: " << file;
            res.message(ss.str());
        }
        return res;
    }

}

#endif //EZT_EZLINUX_H
