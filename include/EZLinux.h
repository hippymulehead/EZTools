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

#ifndef EZTOOLS_EZLINUX_H
#define EZTOOLS_EZLINUX_H

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <unistd.h>
#include <sys/types.h>
#include <grp.h>
#include <pwd.h>
#include <cstdio>
#include <stdio.h>
#include <stdlib.h>
#include <boost/filesystem.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <array>
#include <fcntl.h>
#include <errno.h>
#include <time.h>
#include <stdarg.h>
#include <syslog.h>
#include <sstream>
#include <fstream>
#include "EZString.h"
//#include "EZSystemTime.h"

using namespace std;
namespace fs = boost::filesystem;

namespace EZLinux {

    inline bool isReadable(const boost::filesystem::path& p) {
        return access(p.generic_string().c_str(), R_OK) == 0;
    }

    inline bool isWritable(const boost::filesystem::path& p) {
        return access(p.generic_string().c_str(), W_OK) == 0;
    }

    inline bool isExecutable(const boost::filesystem::path& p) {
        return access(p.generic_string().c_str(), X_OK) == 0;
    }

    inline bool isThere(const boost::filesystem::path& p) {
        return access(p.generic_string().c_str(), F_OK) == 0;
    }

    inline string homedir() {
        const char *homedir;

        if ((homedir = getenv("HOME")) == nullptr) {
            homedir = getpwuid(getuid())->pw_dir;
        }
        return homedir;
    }

    inline bool chownFile(EZString file_path, EZString user_name, EZString group_name) {
        uid_t          uid;
        gid_t          gid;
        struct passwd *pwd;
        struct group  *grp;

        pwd = getpwnam(user_name.c_str());
        if (pwd == nullptr) {
            return false;
        }
        uid = pwd->pw_uid;

        grp = getgrnam(group_name.c_str());
        if (grp == nullptr) {
            return false;
        }
        gid = grp->gr_gid;

        return chown(file_path.c_str(), uid, gid) != -1;
    }

    inline EZString systemHostname() {
        char hn[254];
        gethostname(hn, 254);
        EZString hostname = hn;
        return hostname;
    }

    inline bool runningAsRoot() {
        return getuid() == 0;
    }

    inline int uid() {
        return getuid();
    }

    inline EZString whoami() {
        char username[254];
        getlogin_r(username, 254);
        EZString un = username;
        return un;
    }

    inline EZString exec(const char *cmd) {
        std::array<char, 128> buffer;
        EZString result;
        std::shared_ptr<FILE> pipe(popen(cmd, "r"), pclose);
        if (!pipe) throw std::runtime_error("popen() failed!");
        while (!feof(pipe.get())) {
            if (fgets(buffer.data(), 128, pipe.get()) != nullptr)
                result += buffer.data();
        }
        return result;
    }

    inline EZString workingDir() {
        char cCurrentPath[FILENAME_MAX];
        if (!getcwd(cCurrentPath, sizeof(cCurrentPath))) {
            return "error";
        }
        cCurrentPath[sizeof(cCurrentPath) - 1] = '\0';
        return cCurrentPath;
    }

//    class FilePerms {
//    public:
//        FilePerms(EZString filename) {
//            _fn = filename;
//            fs::perms p = fs::status(_fn).permissions();
//            if (p & fs::perms::owner_read) {
//                _owner = _owner + 4;
//            }
//            if (p & fs::perms::owner_write) {
//                _owner = _owner + 2;
//            }
//            if (p & fs::perms::owner_exe) {
//                _owner = _owner + 1;
//            }
//            if (p & fs::perms::set_uid_on_exe) {
//                _stickyBit = _stickyBit + 4;
//            }
//            if (p & fs::perms::group_read) {
//                _group = _group + 4;
//            }
//            if (p & fs::perms::group_write) {
//                _group = _group + 2;
//            }
//            if (p & fs::perms::group_exe) {
//                _group = _group + 1;
//            }
//            if (p & fs::perms::set_gid_on_exe) {
//                _stickyBit = _stickyBit + 2;
//            }
//            if (p & fs::perms::others_read) {
//                _other = _other + 4;
//            }
//            if (p & fs::perms::others_write) {
//                _other = _other + 2;
//            }
//            if (p & fs::perms::others_exe) {
//                _other = _other + 1;
//            }
//        }
//
//        virtual ~FilePerms() = default;
//
//        int owner() { return _owner; }
//
//        int group() { return _group; }
//
//        int other() { return _other; }
//
//        int stickyBit() { return _stickyBit; }
//
//        EZString str() {
//            stringstream bs;
//            bs << _stickyBit << _owner << _group << _other;
//            return bs.str();
//        }
//
//        bool areEqualTo(int StickyBit, int Owner, int Group, int Other) {
//            return StickyBit != _stickyBit | Owner != _owner | Group != _group | Other != _other;
//        }
//
//    private:
//        int _owner = 0;
//        int _group = 0;
//        int _other = 0;
//        int _stickyBit = 0;
//        string _fn;
//    };

}

#endif //EZTOOLS_EZLINUX_H
