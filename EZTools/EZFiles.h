//
// Created by michael.romans on 7/21/21.
//

#ifndef EZFILESTAT_EZFILES_H
#define EZFILESTAT_EZFILES_H

#include "EZTools.h"
#include <sys/stat.h>
#include <fstream>
#include <cstring>
#include <pwd.h>
#include <grp.h>
#include <zconf.h>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <fcntl.h>
#include "EZLinux.h"
#include "EZDateTime.h"

namespace EZFiles {

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

    struct filePerms_t {
        EZTools::EZString permissionsAsEZString;
        EZTools::EZString sticky;
        bool ow_read = false;
        bool ow_write = false;
        bool ow_exec = false;
        bool g_read = false;
        bool g_write = false;
        bool g_exec = false;
        bool ot_read = false;
        bool ot_write = false;
        bool ot_exec = false;
    };

    struct ezuri_t {
        EZTools::EZString path;
        long int size = 0;
        EZTools::EZString fileType;
        filePerms_t permissions;
        int gid = 0;
        EZTools::EZString groupName;
        int ownerID = 0;
        EZTools::EZString ownerName;
        EZTools::timeStamp_t createTimeStamp;
        EZTools::timeStamp_t accessTimeStamp;
        EZTools::timeStamp_t modTimeStamp;
        std::vector<EZTools::EZString> groups;
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
            _stat.path = _URILocation;

            std::stringstream ss;
            ss << EZTools::statString << "'" << _URILocation << "'";
            auto res = EZLinux::exec(ss.str());
            if (!res.message().regexMatch("cannot statx")) {
                std::vector<EZTools::EZString> lines = res.data.split("\n");
                for (auto &line : lines) {
                    if (line.regexMatch("^Bytes: ")) {
                        line.regexReplace("Bytes: ", "");
                        _stat.size = line.asLong();
                    } else if (line.regexMatch("^Type: ")) {
                        line.regexReplace("^Type: ", "");
                        _stat.fileType = line;
                    } else if (line.regexMatch("^Perms: ")) {
                        line.regexReplace("^Perms: ", "");
                        _stat.permissions.permissionsAsEZString = line;
                        auto p = line.regexSplit("");
                        _stat.permissions.sticky = p.at(1);
                        _stat.permissions.ow_read = p.at(2) != "-";
                        _stat.permissions.ow_write = p.at(3) != "-";
                        _stat.permissions.ow_exec = p.at(4) != "-";
                        _stat.permissions.g_read = p.at(5) != "-";
                        _stat.permissions.g_write = p.at(6) != "-";
                        _stat.permissions.g_exec = p.at(7) != "-";
                        _stat.permissions.ot_read = p.at(8) != "-";
                        _stat.permissions.ot_write = p.at(9) != "-";
                        _stat.permissions.ot_exec = p.at(10) != "-";
                    } else if (line.regexMatch("^GID: ")) {
                        line.regexReplace("^GID: ", "");
                        _stat.gid = line.asInt();
                    } else if (line.regexMatch("^Group: ")) {
                        line.regexReplace("^Group: ", "");
                        _stat.groupName = line;
                    } else if (line.regexMatch("^OwnerID: ")) {
                        line.regexReplace("^OwnerID: ", "");
                        _stat.ownerID = line.asInt();
                    } else if (line.regexMatch("^Owner: ")) {
                        line.regexReplace("^Owner: ", "");
                        _stat.ownerName = line;
                    } else if (line.regexMatch("^Create: ")) {
                        line.regexReplace("^Create: ", "");
                        if (line != "-") {
                            auto ts = line.regexSplit(" ");
                            _stat.createTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.createTimeStamp.time = m.at(0);
                            _stat.createTimeStamp.ms = m.at(1);
                            _stat.createTimeStamp.offset = ts.at(2);
                        }
                    } else if (line.regexMatch("^Access: ")) {
                        if (line != "-") {
                            line.regexReplace("^Access: ", "");
                            auto ts = line.regexSplit(" ");
                            _stat.accessTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.accessTimeStamp.time = m.at(0);
                            _stat.accessTimeStamp.ms = m.at(1);
                            _stat.accessTimeStamp.offset = ts.at(2);
                        }
                    } else if (line.regexMatch("^Mod: ")) {
                        if (line != "-") {
                            line.regexReplace("^Mod: ", "");
                            auto ts = line.regexSplit(" ");
                            _stat.modTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.modTimeStamp.time = m.at(0);
                            _stat.modTimeStamp.ms = m.at(1);
                            _stat.modTimeStamp.offset = ts.at(2);
                        }
                    }
                }
                if (_stat.fileType == "regular file") {
                    _fileType = EZTools::STATFILETYPE::REG;
                } else if (_stat.fileType == "directory") {
                    _fileType = EZTools::STATFILETYPE::REG;
                } else if (_stat.fileType == "symbolic link") {
                    _fileType = EZTools::STATFILETYPE::LNK;
                } else if (_stat.fileType == "character special file") {
                    _fileType = EZTools::STATFILETYPE::CHAR;
                } else if (_stat.fileType == "block special file") {
                    _fileType = EZTools::STATFILETYPE::BLK;
                } else {
                    _fileType = EZTools::STATFILETYPE::UNK;
                }

                if (_fileType == EZTools::STATFILETYPE::REG) {
                    std::vector<EZTools::EZString> p = _URILocation.splitAtLast("/");
                    _basepath = p.at(0) + "/";
                    _file = p.at(1);
                } else if (_fileType == EZTools::STATFILETYPE::DIR) {
                    _basepath = _URILocation;
                }
//            _baseStat.setPath(_basepath);
            }
        }
        ~URI() = default;
        void setPath(EZTools::EZString URILocation) {
            _URILocation = URILocation;
            if (_URILocation.regexMatch("^~/")) {
                _URILocation.regexReplace("^~/", homeDir());
            } else if (_URILocation.regexMatch("^./")) {
                _URILocation.regexReplace("^./", currentDir());
            }
            _stat.path = _URILocation;

            std::stringstream ss;
            ss << EZTools::statString << "'" << _URILocation << "'";
            auto res = EZLinux::exec(ss.str());
            if (!res.message().regexMatch("cannot statx")) {
                std::vector<EZTools::EZString> lines = res.data.split("\n");
                for (auto &line : lines) {
                    if (line.regexMatch("^Bytes: ")) {
                        line.regexReplace("Bytes: ", "");
                        _stat.size = line.asLong();
                    } else if (line.regexMatch("^Type: ")) {
                        line.regexReplace("^Type: ", "");
                        _stat.fileType = line;
                    } else if (line.regexMatch("^Perms: ")) {
                        line.regexReplace("^Perms: ", "");
                        _stat.permissions.permissionsAsEZString = line;
                        auto p = line.regexSplit("");
                        _stat.permissions.sticky = p.at(1);
                        _stat.permissions.ow_read = p.at(2) != "-";
                        _stat.permissions.ow_write = p.at(3) != "-";
                        _stat.permissions.ow_exec = p.at(4) != "-";
                        _stat.permissions.g_read = p.at(5) != "-";
                        _stat.permissions.g_write = p.at(6) != "-";
                        _stat.permissions.g_exec = p.at(7) != "-";
                        _stat.permissions.ot_read = p.at(8) != "-";
                        _stat.permissions.ot_write = p.at(9) != "-";
                        _stat.permissions.ot_exec = p.at(10) != "-";
                    } else if (line.regexMatch("^GID: ")) {
                        line.regexReplace("^GID: ", "");
                        _stat.gid = line.asInt();
                    } else if (line.regexMatch("^Group: ")) {
                        line.regexReplace("^Group: ", "");
                        _stat.groupName = line;
                    } else if (line.regexMatch("^OwnerID: ")) {
                        line.regexReplace("^OwnerID: ", "");
                        _stat.ownerID = line.asInt();
                    } else if (line.regexMatch("^Owner: ")) {
                        line.regexReplace("^Owner: ", "");
                        _stat.ownerName = line;
                    } else if (line.regexMatch("^Create: ")) {
                        line.regexReplace("^Create: ", "");
                        if (line != "-") {
                            auto ts = line.regexSplit(" ");
                            _stat.createTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.createTimeStamp.time = m.at(0);
                            _stat.createTimeStamp.ms = m.at(1);
                            _stat.createTimeStamp.offset = ts.at(2);
                        }
                    } else if (line.regexMatch("^Access: ")) {
                        if (line != "-") {
                            line.regexReplace("^Access: ", "");
                            auto ts = line.regexSplit(" ");
                            _stat.accessTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.accessTimeStamp.time = m.at(0);
                            _stat.accessTimeStamp.ms = m.at(1);
                            _stat.accessTimeStamp.offset = ts.at(2);
                        }
                    } else if (line.regexMatch("^Mod: ")) {
                        if (line != "-") {
                            line.regexReplace("^Mod: ", "");
                            auto ts = line.regexSplit(" ");
                            _stat.modTimeStamp.date = ts.at(0);
                            auto m = ts.at(1).regexSplit("[.]");
                            _stat.modTimeStamp.time = m.at(0);
                            _stat.modTimeStamp.ms = m.at(1);
                            _stat.modTimeStamp.offset = ts.at(2);
                        }
                    }
                }
                if (_stat.fileType == "regular file") {
                    _fileType = EZTools::STATFILETYPE::REG;
                } else if (_stat.fileType == "directory") {
                    _fileType = EZTools::STATFILETYPE::REG;
                } else if (_stat.fileType == "symbolic link") {
                    _fileType = EZTools::STATFILETYPE::LNK;
                } else if (_stat.fileType == "character special file") {
                    _fileType = EZTools::STATFILETYPE::CHAR;
                } else if (_stat.fileType == "block special file") {
                    _fileType = EZTools::STATFILETYPE::BLK;
                } else {
                    _fileType = EZTools::STATFILETYPE::UNK;
                }

                if (_fileType == EZTools::STATFILETYPE::REG) {
                    std::vector<EZTools::EZString> p = _URILocation.splitAtLast("/");
                    _basepath = p.at(0) + "/";
                    _file = p.at(1);
                } else if (_fileType == EZTools::STATFILETYPE::DIR) {
                    _basepath = _URILocation;
                }
//            _baseStat.setPath(_basepath);
            }
        }
        EZTools::EZString path() { return _URILocation; }
        EZTools::EZString basepath() { return _basepath; }
        EZTools::EZString filename() { return _file; }
        void update() {
            setPath(_URILocation);
//            _baseStat.update();
        }
        bool isThere() {
            bool rv = _stat.permissions.ow_read | _stat.permissions.ot_read;
            if (!rv & _stat.permissions.g_read) {
                auto fg = EZLinux::userGroups(EZLinux::whoami());
                for (auto &g : fg) {
                    if (g == _stat.groupName) {
                        rv = true;
                        break;
                    }
                }
            }
            return rv;
        }
        bool isReadable() {
            bool rv = _stat.permissions.ow_read | _stat.permissions.ot_read;
            if (!rv & _stat.permissions.g_read) {
                auto fg = EZLinux::userGroups(EZLinux::whoami());
                for (auto &g : fg) {
                    if (g == _stat.groupName) {
                        rv = true;
                        break;
                    }
                }
            }
            return rv;
        }
        bool isWriteable() {
            bool rv = _stat.permissions.ow_write | _stat.permissions.ot_write;
            if (!rv & _stat.permissions.g_write) {
                auto fg = EZLinux::userGroups(EZLinux::whoami());
                for (auto &g : fg) {
                    if (g == _stat.groupName) {
                        rv = true;
                        break;
                    }
                }
            }
            return rv;
        }
        bool basePathIsThere() {
            if(access(_basepath.c_str(), F_OK) == 0) {
                return true;
            } else {
                return false;
            }
        }
        bool basePathIsReadable() {
            if(access(_basepath.c_str(), R_OK) == 0) {
                return true;
            } else {
                return false;
            }
        }
        bool basePathIsWriteable() {
            if(access(_basepath.c_str(), W_OK) < 0) {
                return false;
            } else {
                return true;
            }
        }
        int fileOwnerPerms() {
            if (_stat.permissions.ow_exec) {
                return 7;
            } else if (_stat.permissions.ow_write) {
                return 5;
            } else if (_stat.permissions.ow_read) {
                return 4;
            }
            return 0;
        }
        int fileGroupPerms() {
            if (_stat.permissions.g_exec) {
                return 7;
            } else if (_stat.permissions.g_write) {
                return 5;
            } else if (_stat.permissions.g_read) {
                return 4;
            }
            return 0;
        }
        int fileOtherPerms() {
            if (_stat.permissions.ot_exec) {
                return 7;
            } else if (_stat.permissions.ot_write) {
                return 5;
            } else if (_stat.permissions.ot_read) {
                return 4;
            }
            return 0;
        }
        int filePermsAsInt() {
            std::stringstream ss;
            ss << fileOwnerPerms() << fileGroupPerms() << fileOtherPerms();
            EZTools::EZString s = ss.str();
            return s.asInt();
        }
//        int basepathOwnerPerms() { return _baseStat.owner(); }
//        int basepathGroupPerms() { return _baseStat.group(); }
//        int basepathOtherPerms() { return _baseStat.other(); }
//        int basepathPermsAsInt() {
//            std::stringstream ss;
//            ss << _baseStat.owner() << _baseStat.group() << _baseStat.other();
//            EZTools::EZString s = ss.str();
//            return s.asInt();
//        }
        EZTools::STATFILETYPE fileType() { return _fileType; }
        EZTools::EZString fileTypeAsEZString() { return _stat.fileType; }
        ezuri_t fileStat() { return _stat; }

    private:
        EZTools::STATFILETYPE _fileType;
        EZTools::EZString _URILocation;
        EZTools::EZString _basepath;
        EZTools::EZString _file;
        EZFiles::ezuri_t _stat;
//        EZFiles::ezuri_t _baseStat;
    };

    inline EZTools::EZReturn<bool> copyFile(URI &source, URI &destination, bool overwrite = false) {
        EZTools::EZReturn<bool> res;
        res.metaData.location = "EZFiles::copyFile";
        if (destination.isThere() && !overwrite) {
            res.message(destination.path() + " is there but you marked it as don't overwrite");
            return res;
        }
        std::ofstream outfile(destination.path());
        if (!outfile.is_open()) {
            res.message("Couldn't write to file: " + destination.path());
            return res;
        }
        if (!source.isThere() || !source.isReadable() || source.fileType() != EZTools::STATFILETYPE::REG) {
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
        res.metaData.location = "EZFiles::copyFileToEZString";
        if (!source.isThere() || !source.isReadable() || source.fileType() != EZTools::STATFILETYPE::REG) {
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
        res.metaData.location = "EZFiles::copyStringToFile";
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
        res.metaData.location = "EZFiles::copyFileToVectorOfEZString";
        auto ts = copyFileToEZString(source);
        if (!ts.wasSuccessful()) {
            res.message(ts.message());
            return res;
        }
        auto tv = ts.data.split("\n");
        res.data.swap(tv);
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<URI> backupFile(URI &source, EZTools::EZString extention) {
        EZTools::EZReturn<URI> res;
        res.metaData.location = "EZFiles::backupFile";
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
        res.metaData.location = "EZFiles::deleteFile";
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
        res.metaData.location = "EZFiles::moveFile";
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
        res.metaData.location = "EZFiles::chownFile";
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
        res.metaData.location = "EZFiles::makedir";
        std::stringstream message;
        if (doALL) {
            std::vector<EZTools::EZString> dirs = dir.path().split("/");
            std::stringstream ss;
            for (auto &d : dirs) {
                if (!d.empty()) {
                    ss << "/";
                    ss << d;
                    URI stat(ss.str());
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
        ret.metaData.location = "EZFiles::makeSymLink";
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
        res.metaData.location = "EZFiles::chmod";
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
        res.metaData.location = "EZFiles::testWriteSpeed";
        if (dir.basePathIsWriteable()) {
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

#endif //EZFILESTAT_EZFILES_H
