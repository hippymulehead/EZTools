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

#ifndef EZT_EZFILES_H
#define EZT_EZFILES_H

#include <sys/stat.h>
#include <fstream>
#include <cstring>
#include <pwd.h>
#include <grp.h>
#include <zconf.h>
#include "EZTools.h"
#include "EZLinux.h"
#include "EZDateTime.h"

namespace EZFiles {

    enum _BACKUP_T {
        BACKUP_PREPEND, BACKUP_EXTENTION
    };

    enum _STATFILE_T {
        UNK = 0, REG = 1, LNK = 2, DIR = 3, CHAR = 4, BLK = 5, FIFO = 6, SOCK = 7
    };

    inline EZTools::EZString tempDir() {
        auto d =  EZLinux::environmentVar("TMPDIR");
        if (d.empty()) {
            return "/tmp/";
        } else {
            return d;
        }
    }

    inline EZTools::EZString homeDir() {
        struct passwd *pw = getpwuid(getuid());
        return EZTools::EZString(pw->pw_dir)+"/";
    }

    inline EZTools::EZString currentDir() {
        char buff[FILENAME_MAX];
        getcwd( buff, FILENAME_MAX );
        std::string current_working_dir(buff);
        return current_working_dir+"/";
    }

    enum ACCESS_T{AC_THERE, AC_READ, AC_WRITE};

    inline bool accessCheck(ACCESS_T accessType, std::string filename) {
        int rval;
        switch (accessType) {
            case AC_THERE:
                rval = access(filename.c_str(), F_OK);
                return rval == 0;
            case AC_READ:
                rval = access (filename.c_str(), R_OK);
                return rval == 0;
            case AC_WRITE:
                rval = access (filename.c_str(), W_OK);
                return rval == 0;
        }
        return false;
    }

    class EZFileStat {
    public:
        EZFileStat() = default;
        EZFileStat(EZTools::EZString filename) {
            _filename = filename;
            _isThere = accessCheck(AC_THERE, filename);
            if (_isThere) {
                _isReadable = accessCheck(AC_READ, filename);
                _isWriteable = accessCheck(AC_WRITE, filename);
                struct stat fileStat;
                if (stat(filename.c_str(), &fileStat) < 0)
                    exit(EXIT_FAILURE);

                if (fileStat.st_mode & S_IRUSR) {
                    _owner = _owner + 4;
                }
                if (fileStat.st_mode & S_IWUSR) {
                    _owner = _owner + 2;
                }
                if (fileStat.st_mode & S_IXUSR) {
                    _owner = _owner + 1;
                }
                if (fileStat.st_mode & S_IRGRP) {
                    _group = _group + 4;
                }
                if (fileStat.st_mode & S_IWGRP) {
                    _group = _group + 2;
                }
                if (fileStat.st_mode & S_IXGRP) {
                    _group = _group + 1;
                }
                if (fileStat.st_mode & S_IROTH) {
                    _other = _other + 4;
                }
                if (fileStat.st_mode & S_IWOTH) {
                    _other = _other + 2;
                }
                if (fileStat.st_mode & S_IXOTH) {
                    _other = _other + 1;
                }

                if (S_ISREG(fileStat.st_mode) == 1) { _FT = 1; }
                if (S_ISLNK(fileStat.st_mode) == 1) { _FT = 2; }
                if (S_ISDIR(fileStat.st_mode) == 1) { _FT = 3; }
                if (S_ISCHR(fileStat.st_mode) == 1) { _FT = 4; }
                if (S_ISBLK(fileStat.st_mode) == 1) { _FT = 5; }
                if (S_ISFIFO(fileStat.st_mode) == 1) { _FT = 6; }
                if (S_ISSOCK(fileStat.st_mode) == 1) { _FT = 7; }

                _size = fileStat.st_blksize;
                _links = fileStat.st_nlink;
                _inode = fileStat.st_ino;
                _hlinks = fileStat.st_nlink;
                _fowner = fileStat.st_uid;
                _gowner = fileStat.st_gid;
                _devid = fileStat.st_rdev;
                _blocksize = fileStat.st_blksize;
                _blocks = fileStat.st_blocks;
            }
        }
        ~EZFileStat() = default;
        void setPath(EZTools::EZString filename) {
            _filename = filename;
            _owner = 0;
            _group = 0;
            _other = 0;
            _isThere = accessCheck(AC_THERE, filename);
            if (_isThere) {
                _isReadable = accessCheck(AC_READ, filename);
                _isWriteable = accessCheck(AC_WRITE, filename);
                struct stat fileStat;
                if (stat(filename.c_str(), &fileStat) < 0)
                    exit(EXIT_FAILURE);

                if (fileStat.st_mode & S_IRUSR) {
                    _owner = _owner + 4;
                }
                if (fileStat.st_mode & S_IWUSR) {
                    _owner = _owner + 2;
                }
                if (fileStat.st_mode & S_IXUSR) {
                    _owner = _owner + 1;
                }
                if (fileStat.st_mode & S_IRGRP) {
                    _group = _group + 4;
                }
                if (fileStat.st_mode & S_IWGRP) {
                    _group = _group + 2;
                }
                if (fileStat.st_mode & S_IXGRP) {
                    _group = _group + 1;
                }
                if (fileStat.st_mode & S_IROTH) {
                    _other = _other + 4;
                }
                if (fileStat.st_mode & S_IWOTH) {
                    _other = _other + 2;
                }
                if (fileStat.st_mode & S_IXOTH) {
                    _other = _other + 1;
                }

                if (S_ISREG(fileStat.st_mode) == 1) { _FT = 1; }
                if (S_ISLNK(fileStat.st_mode) == 1) { _FT = 2; }
                if (S_ISDIR(fileStat.st_mode) == 1) { _FT = 3; }
                if (S_ISCHR(fileStat.st_mode) == 1) { _FT = 4; }
                if (S_ISBLK(fileStat.st_mode) == 1) { _FT = 5; }
                if (S_ISFIFO(fileStat.st_mode) == 1) { _FT = 6; }
                if (S_ISSOCK(fileStat.st_mode) == 1) { _FT = 7; }

                _size = fileStat.st_blksize;
                _links = fileStat.st_nlink;
                _inode = fileStat.st_ino;
                _hlinks = fileStat.st_nlink;
                _fowner = fileStat.st_uid;
                _gowner = fileStat.st_gid;
                _devid = fileStat.st_rdev;
                _blocksize = fileStat.st_blksize;
                _blocks = fileStat.st_blocks;
            }
        }
        void update() { setPath(_filename); }
        int owner() { return _owner; }
        int group() { return _group; }
        int other() { return _other; }
        long size() { return _size; }
        _STATFILE_T fileType() {
            switch (_FT) {
                case 0:
                    return UNK;
                case 1:
                    return REG;
                case 2:
                    return LNK;
                case 3:
                    return DIR;
                case 4:
                    return CHAR;
                case 5:
                    return BLK;
                case 6:
                    return FIFO;
                case 7:
                    return SOCK;
                default:
                    return UNK;
            }
        }
        EZTools::EZString fileTypeAsStr() {
            switch (_FT) {
                case 0:
                    return "Unknown";
                case 1:
                    return "Regular";
                case 2:
                    return "Link";
                case 3:
                    return "Directory";
                case 4:
                    return "Character Device";
                case 5:
                    return "Block Device";
                case 6:
                    return "FIFO Device";
                case 7:
                    return "Socket";
                default:
                    return "Unknown";
            }
        }
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
        EZTools::EZString permsAsStr() {
            std::stringstream bs;
            bs << _owner << _group << _other;
            return bs.str();
        }
        int permsAsInt() {
            EZTools::EZString es = permsAsStr();
            return es.asInt();
        }

    private:
        EZTools::EZString _filename;
        bool _isReadable = false;
        bool _isWriteable = false;
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
    };

    class URI {
    public:
        URI() = default;
        URI(EZTools::EZString URILocation) {
            _URILocation = URILocation;
            if (_URILocation.regexMatch("^~/")) {
                _URILocation.regexReplace("^~/", homeDir());
            } else if (_URILocation.regexMatch("^./")) {
                _URILocation.regexReplace("^./", currentDir());
            }
            _stat.setPath(_URILocation);
            _fileType = _stat.fileType();
            if (_fileType == REG) {
                std::vector<EZTools::EZString> p = _URILocation.splitAtLast("/");
                _basepath = p.at(0) + "/";
                _file = p.at(1);
            } else if (_fileType == DIR) {
                _basepath = _URILocation;
            }
            _baseStat.setPath(_basepath);
        }
        ~URI() = default;
        void setPath(EZTools::EZString URILocation) {
            _URILocation = URILocation;
            if (_URILocation.regexMatch("^~/")) {
                _URILocation.regexReplace("^~/", homeDir());
            } else if (_URILocation.regexMatch("^./")) {
                _URILocation.regexReplace("^./", currentDir());
            }
            _stat.setPath(_URILocation);
            _fileType = _stat.fileType();
            if (_fileType == REG) {
                std::vector<EZTools::EZString> p = _URILocation.splitAtLast("/");
                _basepath = p.at(0) + "/";
                _file = p.at(1);
            } else if (_fileType == DIR) {
                _basepath = _URILocation;
            }
            _baseStat.setPath(_basepath);
        }
        EZTools::EZString path() { return _URILocation; }
        EZTools::EZString basepath() { return _basepath; }
        EZTools::EZString filename() { return _file; }
        void update() {
            _stat.update();
            _baseStat.update();
        }
        bool isThere() {
            return _stat.isThere();
        }
        bool isReadable() {
            return _stat.isReadable();
        }
        bool isWriteable() {
            return _stat.isWriteable();
        }
        bool basePathIsThere() {
            return _baseStat.isThere();
        }
        bool basePathIsReadable() {
            return _baseStat.isReadable();
        }
        bool basePathIsWriteable() {
            return _baseStat.isWriteable();
        }
        int fileOwnerPerms() { return _stat.owner(); }
        int fileGroupPerms() { return _stat.group(); }
        int fileOtherPerms() { return _stat.other(); }
        int filePermsAsInt() {
            std::stringstream ss;
            ss << _stat.owner() << _stat.group() << _stat.other();
            EZTools::EZString s = ss.str();
            return s.asInt();
        }
        int basepathOwnerPerms() { return _baseStat.owner(); }
        int basepathGroupPerms() { return _baseStat.group(); }
        int basepathOtherPerms() { return _baseStat.other(); }
        int basepathPermsAsInt() {
            std::stringstream ss;
            ss << _baseStat.owner() << _baseStat.group() << _baseStat.other();
            EZTools::EZString s = ss.str();
            return s.asInt();
        }
        _STATFILE_T fileType() { return _stat.fileType(); }
        EZTools::EZString fileTypeAsEZString() { return _stat.fileTypeAsStr(); }

    private:
        _STATFILE_T _fileType;
        EZTools::EZString _URILocation;
        EZTools::EZString _basepath;
        EZTools::EZString _file;
        EZFileStat _stat;
        EZFileStat _baseStat;
    };

    inline EZTools::EZReturn<bool> copyFile(URI &source, URI &destination, bool overwrite = false) {
        EZTools::EZReturn<bool> res;
        if (destination.isThere() && !overwrite) {
            res.message(destination.path() + " is there but you marked it as don't overwrite");
            return res;
        }
        std::ofstream outfile(destination.path());
        if (!outfile.is_open()) {
            res.message("Couldn't write to file: " + destination.path());
            return res;
        }
        if (!source.isThere() || !source.isReadable() || source.fileType() != REG) {
            res.message(source.path() + " is not readable or is not a regular file");
            return res;
        }
        std::ifstream infile(source.path());
        if (!infile.is_open()) {
            res.message("Couldn't open file: " + source.path());
            return res;
        }
        outfile << infile.rdbuf();
        infile.close();
        outfile.close();
        res.wasSuccessful(true);
        destination.update();
        return res;
    }

    inline EZTools::EZReturn<EZTools::EZString> copyFileToEZString(URI &source) {
        EZTools::EZReturn<EZTools::EZString> res;
        if (!source.isThere() || !source.isReadable() || source.fileType() != REG) {
            res.message(source.path() + " is not readable or is not a regular file");
            return res;
        }
        std::ifstream infile(source.path());
        if (!infile.is_open()) {
            res.message("Couldn't open file: " + source.path());
            return res;
        }
        std::stringstream outfile;
        outfile << infile.rdbuf();
        infile.close();
        res.wasSuccessful(true);
        res.data = outfile.str();
        return res;
    }

    inline EZTools::EZReturn<bool> copyStringToFile(EZTools::EZString stringToCopy, URI destination) {
        EZTools::EZReturn<bool> res;
        std::ofstream outfile(destination.path());
        if (!outfile.is_open()) {
            res.message("Couldn't write to file: " + destination.path());
            return res;
        }
        outfile << stringToCopy;
        outfile.close();
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<std::vector<EZTools::EZString>> copyFileToVectorOfEZString(URI &source) {
        EZTools::EZReturn<std::vector<EZTools::EZString>> res;
        auto ts = copyFileToEZString(source);
        if (!ts.wasSuccessful()) {
            res.message(ts.message());
            return res;
        }
        auto tv = ts.data.split("\n");
        res.data.swap(tv);
//        for (auto& line : tv) {
////            std::cout << line << std::endl;
//            res.data().emplace_back(line);
//        }
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<URI> backupFile(URI &source, EZTools::EZString extention) {
        EZTools::EZReturn<URI> res;
        std::stringstream ss;
        ss << source.path() << "." << extention;
        URI destination(ss.str());
        auto cres = copyFile(source, destination, true);
        if (cres.wasSuccessful()) {
            res.wasSuccessful(true);
            res.data = destination;
            return res;
        } else {
            res.message(cres.message());
            return res;
        }
    }

    inline EZTools::EZReturn<bool> deleteFile(URI &source) {
        EZTools::EZReturn<bool> res;
        if (!source.isWriteable()) {
            res.message(source.path() + " is not there or not readable");
            return res;
        }
        int errorCode = remove(source.path().c_str());
        if (errorCode != 0) {
            res.message(std::strerror(errorCode));
            res.exitCode(errorCode);
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<bool> moveFile(URI &source, URI &destination, bool overwrite = false) {
        EZTools::EZReturn<bool> res;
        auto cres = copyFile(source, destination, overwrite);
        if (cres.wasSuccessful()) {
            auto dres = deleteFile(source);
            if (dres.wasSuccessful()) {
                res.wasSuccessful(true);
                source.update();
                return res;
            } else {
                res.message(dres.message());
                return res;
            }
        } else {
            res.message(cres.message());
            return res;
        }
    }

    inline EZTools::EZReturn<bool> chownFile(URI &source, EZTools::EZString user_name, EZTools::EZString group_name) {
        uid_t uid;
        gid_t gid;
        struct passwd *pwd;
        struct group *grp;
        EZTools::EZReturn<bool> res;
        pwd = getpwnam(user_name.c_str());
        if (pwd == nullptr) {
            res.message("Could not chown " + source.path() + " " + user_name + ":" + group_name);
            return res;
        }
        uid = pwd->pw_uid;
        grp = getgrnam(group_name.c_str());
        if (grp == nullptr) {
            res.message("Could not chown " + source.path() + " " + user_name + ":" + group_name);
            return res;
        }
        gid = grp->gr_gid;
        if (chown(source.path().c_str(), uid, gid) != -1) {
            res.wasSuccessful(true);
            return res;
        }
        source.update();
        res.message("Could not chown " + source.path() + " " + user_name + ":" + group_name);
        return res;
    }

    inline EZTools::EZReturn<bool> makedir(URI &dir, bool doALL = false) {
        EZTools::EZReturn<bool> res;
        std::stringstream message;
        if (doALL) {
            std::vector<EZTools::EZString> dirs = dir.path().split("/");
            std::stringstream ss;
            for (auto &d : dirs) {
                if (!d.empty()) {
                    ss << "/";
                    ss << d;
                    EZFileStat stat(ss.str());
                    if (!stat.isThere()) {
                        if (mkdir(ss.str().c_str(), 0777) == -1) {
                            res.exitCode(errno);
                            std::stringstream m;
                            m << "Error: " << strerror(errno);
                            res.message(m.str());
                            return res;
                        }
                    }
                }
            }
        } else {
            if (!dir.isThere()) {
                if (mkdir(dir.path().c_str(), 0777) == -1) {
                    res.exitCode(errno);
                    std::stringstream m;
                    m << "Error: " << strerror(errno);
                    res.message(m.str());
                    return res;
                }
            }
        }
        return res;
    }

    inline EZTools::EZReturn<bool> makeSymLink(URI &source, URI &desination) {
        EZTools::EZReturn<bool> ret;
        if (!source.isThere() || !source.isReadable()) {
            ret.message(source.path() + " not there or not readable");
            ret.wasSuccessful(false);
            return ret;
        }
        if (desination.isThere()) {
            ret.message(desination.path() + " is already there");
            ret.wasSuccessful(false);
            return ret;
        }
        int errorCode = symlink(source.path().c_str(), desination.path().c_str());
        if (errorCode != 0) {
            ret.message(strerror(errorCode));
            ret.exitCode(errorCode);
            return ret;
        }
        ret.wasSuccessful(true);
        return ret;
    }

    // This is not great but it's easy and it does it's job like my girlfriend in high school :FIXME
    inline EZTools::EZReturn<bool> chmod(URI &source, int mode) {
        EZTools::EZReturn<bool> res;
        if (!source.isThere()) {
            res.message(source.path()+" not found");
            return res;
        }
        if (source.isWriteable()) {
            std::stringstream ss;
            ss << "/usr/bin/chmod " << mode << " " << source.path();
            auto ret = EZLinux::exec(ss.str());
            if (!ret.wasSuccessful()) {
                res.message("chmod of "+source.path()+" failed");
                return res;
            }
            res.wasSuccessful(true);
            return res;
        }
        res.message(source.path()+" is not writeable");
        return res;
    }

    inline EZTools::EZReturn<double > testWriteSpeed(URI dir, int numberOfMegabytes) {
        EZTools::EZReturn<double> res;
        if (dir.isWriteable()) {
            EZDateTime::CurrentDateTime dt;
            EZDateTime::Timer t;
            URI filename(dir.path() + dt.stamp() + ".tst");
            char payload[1024 * 1024]; // 1 MiB
            t.start();
            FILE *file = fopen(filename.path().c_str(), "w");
            for (auto i = 0; i != numberOfMegabytes; ++i) {
                fwrite(payload, 1, sizeof(payload), file);
            }
            fclose(file);
            filename.update();
            deleteFile(filename);
            t.stop();
            res.data = t.asMillisenconds();
            res.wasSuccessful(true);
            return res;
        } else {
            res.message(dir.path()+" is not writeable");
            return res;
        }
    }
}

#endif //EZT_EZFILES_H
