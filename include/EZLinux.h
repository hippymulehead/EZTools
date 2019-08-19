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

#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cstdio>
#include <cstdlib>
#include <pwd.h>
#include <grp.h>
#include <iostream>
#include <string>
#include <vector>
#include <Poco/Path.h>
#include <Poco/File.h>
#include <Poco/Environment.h>
#include <Poco/Timestamp.h>
#include <Poco/Net/DNS.h>
#include <pwd.h>
#include <fstream>
#include <streambuf>
#include "EZTools.h"

using namespace std;
using namespace EZTools;

namespace EZLinux {

    typedef vector<EZString> PATHSTOR;
    enum EZSTATFILETYPE {UNK = 0, REG = 1, LNK = 2, DIR = 3, CHAR = 4, BLK = 5, FIFO = 6, SOCK = 7};

    inline EZString homeDir() {
        return Path::home();
    }

    inline EZString currentDir() {
        return Path::current();
    }

    inline EZString tempDir() {
        return Path::temp();
    }

    inline EZString nullDir() {
        return Path::null();
    }

    inline EZString environmentVar(EZString name, EZString defaultValue = "") {
        return Poco::Environment::get(name, defaultValue);
    }

    inline PATHSTOR path() {
        EZString pstring = Poco::Environment::get("PATH");
        PATHSTOR sp = pstring.split(":");
        return sp;
    }

    inline EZString hostname() {
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

    inline EZString exec(EZString command) {
        FILE *fp;
        stringstream output;
        char path[1035];
        fp = popen(command.c_str(), "r");
        if (fp == nullptr) {
            cout << "Failed to run " << command << endl;
            exit(EXIT_FAILURE);
        }
        while (fgets(path, sizeof(path)-1, fp) != nullptr) {
            output << path;
        }
        pclose(fp);
        return output.str();
    }

    inline EZError chownFile(EZString file_path, EZString user_name, EZString group_name) {
        uid_t          uid;
        gid_t          gid;
        struct passwd *pwd;
        struct group  *grp;
        pwd = getpwnam(user_name.c_str());
        if (pwd == nullptr) {
            EZError ret("Could not chown "+file_path+" "+user_name+":"+group_name);
            return ret;
        }
        uid = pwd->pw_uid;
        grp = getgrnam(group_name.c_str());
        if (grp == nullptr) {
            EZError ret("Could not chown "+file_path+" "+user_name+":"+group_name);
            return ret;
        }
        gid = grp->gr_gid;
        if (chown(file_path.c_str(), uid, gid) != -1) {
            return EZError();
        }
        EZError ret("Could not chown "+file_path+" "+user_name+":"+group_name);
        return  ret;
    }

    class EZFileStat {
    public:
        EZFileStat(EZString filename);
        ~EZFileStat() = default;
        int owner() { return _owner; }
        int group() { return _group; }
        int other() { return _other; }
        long size() { return _size; }
        EZSTATFILETYPE fileType();
        EZString fileTypeAsStr();
        int links() { return _links; }
        int inode() { return _inode; }
        int hardLinks() { return _hlinks; }
        int fileOwnerID() { return _fowner; }
        int fileGroupID() { return _gowner; }
        int deviceID() { return _devid; }
        int blockSize() { return _blocksize; }
        int numberOfBlocks() { return _blocks; }
        bool isThere() { return _isThere; }
        bool isReadable() { return _isReadable; }
        bool isWriteable() { return _isWriteable; }
        EZString permsAsStr();
        int permsAsInt();

    private:
        Poco::Path _path;
        Poco::File _file;
        bool _isReadable;
        bool _isWriteable;
        bool _isThere;
        int _owner = 0;
        int _group = 0;
        int _other = 0;
        long _size;
        long _links;
        long _hlinks;
        long _inode;
        int _fowner;
        int _gowner;
        int _devid;
        int _blocksize;
        int _blocks;
        int _FT = 0;
        string _fn;
    };

    inline EZString readFile(EZString filename) {
        EZFileStat stat(filename);
        EZString contents = "";
        if (stat.isThere() && stat.isReadable()) {
            ifstream t(filename);
            stringstream buffer;
            buffer << t.rdbuf();
            contents = buffer.str();
            return contents;
        } else {
            return contents;
        }
    }

    inline EZError copyFile(EZString source, EZString destination, bool overwrite = false) {
        EZFileStat sf(source);
        EZFileStat df(destination);
        if (!sf.isThere() || !sf.isReadable()) {
            EZError ret(source+" is not readable");
            return ret;
        }
        if (!overwrite && df.isThere()) {
            EZError ret(destination+" is there but can't be overwritten");
            return ret;
        }
        if (overwrite && df.isThere() && !df.isWriteable()) {
            EZError ret(destination+" is not writeable");
            return ret;
        }
        ifstream sourcef(source, ios::binary);
        ofstream destf(destination, ios::binary);
        destf << sourcef.rdbuf();
        sourcef.close();
        destf.close();
        return EZError();
    }

    inline EZError moveFile(EZString source, EZString destination, bool overwrite = false) {
        EZFileStat stat(source);
        if (!stat.isWriteable()) {
            EZError ret(source+" can not be removed");
            return ret;
        }
        EZError ret = copyFile(source, destination, overwrite);
        if (!ret.isSuccess()) {
            return ret;
        }
        if (remove(source.c_str()) == 0) {
            return EZError();
        }
        EZError rett(source+" is writeable but can not be removed");
        return rett;
    }

    inline EZError deleteFile(EZString fileToDelete) {
        EZFileStat fn(fileToDelete);
        if (!fn.isThere() || !fn.isReadable()) {
            EZError ret(fileToDelete+" is not there or not readable");
            return ret;
        }
        if (remove(fileToDelete.c_str()) == 0) {
            return EZError();
        }
        EZError rett(fileToDelete+" is readable but can not be removed");
        return rett;
    }

}

#endif //EZTOOLS_EZLINUX_H
