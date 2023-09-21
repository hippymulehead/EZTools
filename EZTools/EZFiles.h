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

#ifndef EZ_EZFILES_H
#define EZ_EZFILES_H

#pragma once

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
#include <utility>
#include "EZTools.h"
#include "EZLinux.h"
#include "EZDateTime.h"
#include "EZUUID.h"
#include <bits/stdc++.h>
#include "nlohmann/json.hpp"

namespace EZFiles {

    inline std::vector<EZTools::EZString> splitPath(EZTools::EZString path) {
        std::vector<EZTools::EZString> list;
        if (path.regexMatch("[*]")) {
            auto l = path.splitAtLast("/");
            list.emplace_back(l.at(0));
            if (l.at(1).regexMatch("[*]")) {
                l.at(1).regexReplace("[*]", ".+");
            }
            list.emplace_back(l.at(1));
        } else {
            list.emplace_back(path);
            list.emplace_back(".+");
        }
        return list;
    }

    inline std::vector<EZTools::EZString> getDirList(EZTools::EZString dir) {
        std::vector<EZTools::EZString> list;
        auto spp = splitPath(dir);
        for (const auto & file : std::filesystem::directory_iterator(spp.at(0).str())) {
            EZTools::EZString l(file.path().string());
            if (l.regexMatch(spp.at(1))) {
                list.emplace_back(l);
            }
        }
        sort(list.begin(), list.end());
        return list;
    }

    inline EZTools::EZString tempDir() {
        auto d = std::filesystem::temp_directory_path();
        if (d.empty()) {
            return "/tmp/";
        } else {
            return d.string();
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

    inline bool isFileThere(const std::string& name) {
        return exists(std::filesystem::status(name));
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

    template <typename TP>
    std::time_t to_time_t(TP tp) {
        using namespace std::chrono;
        auto sctp = time_point_cast<system_clock::duration>(tp - TP::clock::now()
                                                            + system_clock::now());
        return system_clock::to_time_t(sctp);
    }

    // The plan for 7 is to NOT store perms in URI but to pass them forward from std::filesystem
    // 6 will stay this way for it run for code compatability.

    class URI {
    public:
        URI() {
            _p.clear();
            _p.append("/");
        }
        explicit URI(EZTools::EZString path) {
            _perms.pathName = path;
            if (_perms.pathName.regexMatch("^\\~\\/")) {
                _perms.pathName.regexReplace("^\\~\\/", homeDir());
            } else if (_perms.pathName.regexMatch("^\\.\\/")) {
                _perms.pathName.regexReplace("^\\.\\/", currentDir());
            }
            _perms.isThere = isFileThere(_perms.pathName);
            std::filesystem::path pp(_perms.pathName.str());
            _p = pp;
            auto p = std::filesystem::status(_p.string()).permissions();
            struct stat FileInfo {};
            stat(_p.string().c_str(), &FileInfo);
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
            //FIXME:  This is NOT 100% right but covers most use cases
            std::stringstream ss;
            ss << (_perms.sticky ? "s" : "-")
               << (_perms.dir ? "d" : "-")
               << (_perms.ownerRead ? "r" : "-")
               << (_perms.ownerWrite ? "w" : "-")
               << (_perms.ownerExec ? "x" : "-")
               << (_perms.groupRead ? "r" : "-")
               << (_perms.groupWrite ? "w" : "-")
               << (_perms.groupExec ? "x" : "-")
               << (_perms.otherRead ? "r" : "-")
               << (_perms.otherWrite ? "w" : "-")
               << (_perms.otherExec ? "x" : "-");
            _perms.permsString = ss.str();
        }
        ~URI() = default;
        void setPath(EZTools::EZString path) {
            _perms.pathName = std::move(path);
            if (_perms.pathName.regexMatch("^~/")) {
                _perms.pathName.regexReplace("^~/", homeDir());
            } else if (_perms.pathName.regexMatch("^./")) {
                _perms.pathName.regexReplace("^./", currentDir());
            }
            _perms.isThere = isFileThere(_perms.pathName);
            _p.clear();
            _p.append(_perms.pathName.str());
            auto p = std::filesystem::status(_p.string()).permissions();
            struct stat FileInfo {};
            stat(_p.string().c_str(), &FileInfo);
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
            //FIXME:  This is NOT 100% right but covers most use cases
            std::stringstream ss;
            ss << (_perms.sticky ? "s" : "-")
               << (_perms.dir ? "d" : "-")
               << (_perms.ownerRead ? "r" : "-")
               << (_perms.ownerWrite ? "w" : "-")
               << (_perms.ownerExec ? "x" : "-")
               << (_perms.groupRead ? "r" : "-")
               << (_perms.groupWrite ? "w" : "-")
               << (_perms.groupExec ? "x" : "-")
               << (_perms.otherRead ? "r" : "-")
               << (_perms.otherWrite ? "w" : "-")
               << (_perms.otherExec ? "x" : "-");
            _perms.permsString = ss.str();
        }
        void update() {
            EZTools::EZString pn = _perms.pathName;
            _perms.isThere = isFileThere(_perms.pathName);
            if (!_perms.isThere) {
                _perms = {};
                _perms.pathName = pn;
            } else {
                auto p = std::filesystem::status(_p.string()).permissions();
                struct stat FileInfo;
                stat(_p.string().c_str(), &FileInfo);
                struct passwd *UserDatabaseEntry = getpwuid(FileInfo.st_uid);
                struct group *GroupDatabaseEntry = getgrgid(FileInfo.st_gid);
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
                //FIXME:  This is NOT 100% right but covers most use cases
                std::stringstream ss;
                ss << (_perms.sticky ? "s" : "-")
                   << (_perms.dir ? "d" : "-")
                   << (_perms.ownerRead ? "r" : "-")
                   << (_perms.ownerWrite ? "w" : "-")
                   << (_perms.ownerExec ? "x" : "-")
                   << (_perms.groupRead ? "r" : "-")
                   << (_perms.groupWrite ? "w" : "-")
                   << (_perms.groupExec ? "x" : "-")
                   << (_perms.otherRead ? "r" : "-")
                   << (_perms.otherWrite ? "w" : "-")
                   << (_perms.otherExec ? "x" : "-");
                _perms.permsString = ss.str();
            }
        }
        EZDateTime::DateTime lastModified() {
            if (isThere()) {
                std::filesystem::directory_entry q(_perms.pathName.str());
                std::filesystem::file_time_type ftime = std::filesystem::last_write_time(q);
                std::time_t tt = to_time_t(ftime);
                struct tm *_tm;
                _tm = localtime(&tt);
                std::stringstream ss;
                ss << _tm->tm_year + 1900 << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mon + 1
                   << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mday << " ";
                ss << std::setfill('0') << std::setw(2) << _tm->tm_hour << ":";
                ss << std::setfill('0') << std::setw(2) << _tm->tm_min << ":";
                ss << std::setfill('0') << std::setw(2) << _tm->tm_sec + 1;
                EZDateTime::DateTime dt(ss.str());
                return dt;
            } else {
                return EZDateTime::DateTime("0000-00-00 00:00:00");
            }
        }
        EZDateTime::DateTime created() {
            struct stat t_stat{};
            stat(_perms.pathName.c_str(), &t_stat);
            struct tm * _tm = localtime(&t_stat.st_ctime);
            std::stringstream ss;
            ss << _tm->tm_year + 1900 << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mon + 1
               << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mday << " ";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_hour << ":";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_min << ":";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_sec + 1;
            EZDateTime::DateTime dt(ss.str());
            return dt;
        }
        EZDateTime::DateTime lastAccess() {
            struct stat t_stat{};
            stat(_perms.pathName.c_str(), &t_stat);
            struct tm * _tm = localtime(&t_stat.st_atim.tv_sec);
            std::stringstream ss;
            ss << _tm->tm_year + 1900 << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mon + 1
               << "-" << std::setfill('0') << std::setw(2) << _tm->tm_mday << " ";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_hour << ":";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_min << ":";
            ss << std::setfill('0') << std::setw(2) << _tm->tm_sec + 1;
            EZDateTime::DateTime dt(ss.str());
            return dt;
        }
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
        uintmax_t sizeBites() {
            if (isThere()) {
                return std::filesystem::file_size(_p);
            } else {
                return 0;
            }
        }
        double sizeK() {
            if (isThere()) {
                return std::filesystem::file_size(_p) / 1024.0;
            } else {
                return 0;
            }
        }
        double sizeMegs() {
            if (isThere()) {
                return std::filesystem::file_size(_p) / 1024.0 / 1024.0;
            } else {
                return 0;
            }
        }
        double sizeGigs() {
            if (isThere()) {
                return std::filesystem::file_size(_p) / 1024.0 / 1024.0 / 1024.0;
            } else {
                return 0;
            }
        }
        EZTools::EZString permsAsString() {
            if (isThere()) {
                return _perms.permsString;
            } else {
                return {};
            }
        }
        perms_t perms() {
            return _perms;
        }
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
        EZTools::STATFILETYPE fileType() {
            if (isRegularFile()) { return EZTools::REG; }
            if (isSymlink()) { return EZTools::LNK; }
            if (isDirectory()) { return EZTools::DIR; }
            if (isCharactorFile()) { return EZTools::CHAR; }
            if (isBlockDevice()) { return EZTools::BLK; }
            if (isFifo()) { return EZTools::FIFO; }
            if (isSocket()) { return EZTools::SOCK; }
            if (isOther()) { return EZTools::OTHER; }
            return EZTools::UNK;
        }
        EZTools::EZString fileTypeAsString() {
            switch (fileType()) {
                case EZTools::REG:
                    return "Regular File";
                case EZTools::LNK:
                    return "SymLink";
                case EZTools::DIR:
                    return "Directory";
                case EZTools::CHAR:
                    return "Char Device";
                case EZTools::BLK:
                    return "Block Device";
                case EZTools::FIFO:
                    return "Fifo Device";
                case EZTools::SOCK:
                    return "Unix Socket";
                case EZTools::OTHER:
                    return "Other";
                case EZTools::UNK:
                    return "Unknown Type";
            }
        }
    private:
        std::filesystem::path _p;
        perms_t _perms;
    };

    struct URINewDirs_t {
        EZFiles::URI source;
        EZFiles::URI newDirs;
    };

    inline URINewDirs_t makeURIPair(EZTools::EZString base, EZTools::EZString newDirs) {
        URINewDirs_t t;
        t.source.setPath(base);
        t.newDirs.setPath(newDirs);
        return t;
    }

    inline EZTools::EZReturnNoData copyFile(EZFiles::URI *source, EZFiles::URI *destination, bool overwrite = false) {
        EZTools::EZReturnNoData res;
        res.location("EZFiles::copyFile ");
        if (!source->canRead()) {
            std::stringstream ss;
            ss << res.location() << "Can't read " << source->path();
            res.message(ss.str());
            return res;
        }
        if (!overwrite && destination->isThere()) {
            std::stringstream ss;
            ss << res.location() << source->path() << " is there but overwrite is false";
            res.message(ss.str());
            return res;
        }
        auto bp = destination->path().splitAtLast("/");
        EZTools::EZString baseName = bp.at(0);
        EZFiles::URI base(baseName);
        if (!base.canWrite()) {
            std::stringstream ss;
            ss << res.location() << source->path() << " is not writeable by " << EZLinux::whoami();
            res.message(ss.str());
            return res;
        }
        std::filesystem::copy_options co = std::filesystem::copy_options::none;
        if (overwrite) {
            co = std::filesystem::copy_options::update_existing;
        }
        std::filesystem::copy(source->path().str(), destination->path().str(), co);
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<EZTools::EZString> copyFileToEZString(URI *source) {
        EZTools::EZReturn<EZTools::EZString> res;
        source->update();
        if (source->isDirectory()) {
            res.message("Can't copy a directory to a string");
            return res;
        }
        res.location("EZFiles::copyFileToEZString");
        if (!source->isThere() || !source->canRead()) {
            res.message(source->path() + " is not readable");
            return res;
        }
        if (source->fileType() == EZTools::STATFILETYPE::REG || source->fileType() == EZTools::STATFILETYPE::LNK) {
            std::ifstream infile(source->path().str());
            if (!infile.is_open()) {
                res.message("Couldn't open file: " + source->path());
                return res;
            }
            std::stringstream outfile;
            outfile << infile.rdbuf();
            infile.close();
            res.wasSuccessful(true);
            res.data = outfile.str();
        } else {
            res.message(source->path() + " is not a regular file");
            return res;
        }
        return res;
    }

    inline EZTools::EZReturnNoData makeDirectories(EZFiles::URINewDirs_t dirs) {
        EZTools::EZReturnNoData res;
        EZFiles::URI base(dirs.source);
        EZFiles::URI newDirs(dirs.newDirs);
        if (!base.canWrite()) {
            std::stringstream ss;
            ss << "Can't write to " << base.path();
            res.message(ss.str());
            return res;
        }
        std::stringstream dn;
        dn << base.path() << dirs.newDirs.path();
        if (!std::filesystem::create_directories(dn.str())) {
            std::stringstream ss;
            ss << "Could not create " << base.path() << newDirs.path();
            res.message(ss.str());
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    EZTools::EZReturnNoData copyDirectory(EZFiles::URI &source, EZFiles::URI &destination, bool updateExisting = false,
                                          bool dirsOnly = false) {
        EZTools::EZReturnNoData res;
        if (!source.canRead()) {
            std::stringstream ss;
            ss << source.path() << " is not readable by " << EZLinux::whoami();
            res.message(ss.str());
            return res;
        }
        EZTools::EZString dp = destination.path().regexReplace("/$","");
        auto d = dp.splitAtLast("/");
        EZTools::EZString baseDir = d.at(0);
        EZFiles::URI bd(baseDir);
        if (!bd.canWrite()) {
            std::stringstream ss;
            ss << destination.path() << " is not writeable";
            res.message(ss.str());
            return res;
        }
        auto copyOptions = std::filesystem::copy_options::none;
        if (updateExisting) {
            copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::overwrite_existing;
        } else if (updateExisting & dirsOnly) {
            copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::directories_only |
                          std::filesystem::copy_options::overwrite_existing;
        } else if (dirsOnly) {
            copyOptions = std::filesystem::copy_options::recursive | std::filesystem::copy_options::directories_only;
        }
        std::filesystem::copy(source.path().str(), destination.path().str(), copyOptions);
        return res;
    }

    inline EZTools::EZReturn<std::vector<EZTools::EZString>> copyFileToVectorOfEZString(URI *source) {
        EZTools::EZReturn<std::vector<EZTools::EZString>> res;
        res.location("EZFiles::copyFileToVectorOfEZString");
        source->update();
        if (source->isDirectory()) {
            res.message("Can't copy a directory to a string");
            return res;
        }
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

    inline EZTools::EZReturnNoData copyStringToFile(EZTools::EZString stringToCopy, URI *destination,
                                                    bool append = false) {
        EZTools::EZReturnNoData res;
        if (destination->isDirectory()) {
            res.message("Can't copy a string to a directory");
            return res;
        }
        res.location("EZFiles::copyStringToFile");
        std::ofstream outfile;
        if (append) {
            outfile.open(destination->path().str(), std::ios_base::app);
        } else {
            outfile.open(destination->path().str());
        }
        if (!outfile.is_open()) {
            res.message("Couldn't write to file: " + destination->path());
            return res;
        }
        outfile << stringToCopy;
        outfile.close();
        res.wasSuccessful(true);
        destination->update();
        return res;
    }

    inline nlohmann::json readFileAsJSON(URI *source) {
        nlohmann::json root;
        auto st = copyFileToEZString(source);
        root = nlohmann::json::parse(st.data);
        return root;
    }

    inline EZTools::EZReturn<URI> backupFile(URI *source, EZTools::EZString extention) {
        std::stringstream ss;
        ss << source->path() << "." << extention;
        URI destination(ss.str());
        auto cres = copyFile(source, &destination,true);
        if (cres.wasSuccessful()) {
            EZTools::EZReturn<URI> res;
            res.data.setPath(ss.str());
            res.location("EZFiles::backupFile");
            res.wasSuccessful(true);
            return res;
        } else {
            EZTools::EZReturn<URI> res;
            res.location("EZFiles::backupFile");
            res.message(cres.message());
            return res;
        }
    }

    inline EZTools::EZReturn<bool> deleteFile(URI *source) {
        EZTools::EZReturn<bool> res;
        source->update();
        if (source->isDirectory() && !source->isSymlink()) {
            res.message("Can't delete a directory");
            return res;
        }
        res.location("EZFiles::deleteFile");
        int errorCode = remove(source->path().c_str());
        if (errorCode != 0) {
            res.message(std::strerror(errorCode));
            res.exitCode(errorCode);
            return res;
        }
        res.wasSuccessful(true);
        source->update();
        return res;
    }

    inline EZTools::EZReturnNoData deleteDirRecursively(EZFiles::URI &sourceDir) {
        EZTools::EZReturnNoData res;
        if (!sourceDir.isDirectory()) {
            std::stringstream ss;
            ss << sourceDir.path() << " is not a directory";
            res.message(ss.str());
            return res;
        }
        if (sourceDir.isSymlink()) {
            std::stringstream ss;
            ss << sourceDir.path() << " is a symlink, you should delete as a file";
            res.message(ss.str());
            return res;
        }
        if (!std::filesystem::remove_all(sourceDir.path().str())) {
            std::stringstream ss;
            ss << "Failed to remove " << sourceDir.path();
            res.message(ss.str());
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<bool> moveFile(URI *source, URI *destination, bool overwrite = false) {
        EZTools::EZReturn<bool> res;
        res.location("EZFiles::moveFile");
        auto cres = copyFile(source, destination, overwrite);
        if (cres.wasSuccessful()) {
            auto dres = deleteFile(source);
            if (dres.wasSuccessful()) {
                res.wasSuccessful(true);
                source->update();
                return res;
            } else {
                res.message(dres.message());
                destination->update();
                return res;
            }
        } else {
            res.message(cres.message());
            destination->update();
            return res;
        }
    }

    inline EZTools::EZReturn<bool> chownFile(URI *source, EZTools::EZString user_name, EZTools::EZString group_name) {
        uid_t uid;
        gid_t gid;
        struct passwd *pwd;
        struct group *grp;
        EZTools::EZReturn<bool> res;
        res.location("EZFiles::chownFile");
        pwd = getpwnam(user_name.c_str());
        if (pwd == nullptr) {
            res.message("Could not chown " + source->path() + " " + user_name + ":" + group_name);
            return res;
        }
        uid = pwd->pw_uid;
        grp = getgrnam(group_name.c_str());
        if (grp == nullptr) {
            res.message("Could not chown " + source->path() + " " + user_name + ":" + group_name);
            return res;
        }
        gid = grp->gr_gid;
        if (chown(source->path().c_str(), uid, gid) != -1) {
            res.wasSuccessful(true);
            source->update();
            return res;
        }
        res.message("Could not chown " + source->path() + " " + user_name + ":" + group_name);
        return res;
    }

    inline EZTools::EZReturn<bool> makedir(URI &dir, bool doALL = false) {
        EZTools::EZReturn<bool> res;
        res.location("EZFiles::makedir");
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
        dir.update();
        return res;
    }

    inline EZTools::EZReturn<bool> makeSymLink(URI &source, URI &desination) {
        EZTools::EZReturn<bool> ret;
        ret.location("EZFiles::makeSymLink");
        if (!source.isThere() || !source.canRead()) {
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
        source.update();
        return ret;
    }

    // This is not great but it's easy and it does it's job like my girlfriend in high school :FIXME
    inline EZTools::EZReturn<bool> chmod(URI &source, int mode) {
        EZTools::EZReturn<bool> res;
        res.location("EZFiles::chmod");
        if (!source.isThere()) {
            res.message(source.path()+" not found");
            return res;
        }
        if (source.canWrite()) {
            std::stringstream ss;
            ss << "/usr/bin/chmod " << mode << " " << source.path();
            auto ret = EZLinux::exec(ss.str());
            res.wasSuccessful(true);
            source.update();
            return res;
        }
        res.message(source.path()+" is not writeable");
        return res;
    }

    inline EZTools::EZReturn<double > testWriteSpeed(URI dir, int numberOfMegabytes) {
        EZTools::EZReturn<double> res;
        res.location("EZFiles::testWriteSpeed");
        if (dir.canWrite()) {
            EZDateTime::DateTime dt;
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
            deleteFile(&filename);
            t.stop();
            res.data = t.asMillisenconds();
            res.wasSuccessful(true);
            return res;
        } else {
            res.message(dir.path()+" is not writeable");
            return res;
        }
    }

    enum OstreamPerms {
        READ,
        APPEND,
        TRUNCATE
    };

    class TextFile {
    public:
        explicit TextFile(URI *uri, OstreamPerms perms) {
            _name = uri->path();
            _uri = uri;
            _perms = perms;
            if (perms == READ) {
                _file.open(_name, std::ios::in);
            } else if (perms == APPEND) {
                _file.open(_name, std::ios::out | std::ios::app);
            } else if (perms == TRUNCATE) {
                _file.open(_name, std::ios::out | std::ios::trunc);
            }
            uri->update();
        }
        ~TextFile() {
            if (_file.is_open()) {
                _file.flush();
                _file.close();
            }
            _uri->update();
        }
        void reload() {
            _name = _uri->path();
            if (_perms == READ) {
                _file.open(_name, std::ios::in);
            } else if (_perms == APPEND) {
                _file.open(_name, std::ios::out | std::ios::app);
            } else if (_perms == TRUNCATE) {
                _file.open(_name, std::ios::out | std::ios::trunc);
            }
            std::filesystem::path p(_name.str());
            _uri->update();
        }
        void setPerms(OstreamPerms perms) {
            if (_file.is_open()) {
                _file.flush();
                _file.close();
            }
            _uri->update();
            if (perms == READ) {
                _file.open(_name, std::ios::in);
            } else if (perms == TRUNCATE) {
                _file.open(_name, std::ios::out | std::ios::trunc);
            } else if (perms == APPEND) {
                _file.open(_name, std::ios::out | std::ios::app);
            }
            _uri->update();
        }
        EZTools::EZString path() { return _name; }
        URI *uri() { return _uri; }
        bool isThere() { return _uri->isThere(); }
        bool isWriteable() { return _uri->canWrite(); }
        bool isReadable() { return _uri->canRead(); }
        bool isOpen() { return _file.is_open(); }
        void close() {
            _file.close();
        }
        EZTools::EZString contents() {
            auto s = copyFileToEZString(_uri);
            _uri->update();
            return s.data;
        }
        std::vector<EZTools::EZString> vector() {
            auto s = copyFileToEZString(_uri);
            auto v = s.data.split("\n");
            return v;
        }
    private:
        URI* _uri;
        OstreamPerms _perms;
        EZTools::EZString _name;
        std::ofstream _file;

        template<typename T>
        friend std::ostream &operator<<(TextFile &tf, T op) {
            tf._file << op;
            return tf._file;
        }
    };

    inline EZTools::EZString TempFileName() {
        return "/tmp/EZTools-Temp_file";
    }

    class TempFile {
    public:
        explicit TempFile(URI *uri) {
            _tfu = uri;
            _name = uri->path();
            if (_name.regexMatch("^~/")) {
                _name.regexReplace("^~/", homeDir());
            } else if (_name.regexMatch("^./")) {
                _name.regexReplace("^./", currentDir());
            }
            _name = _name + "." + EZUUID::UUID_v4() + ".tmp";
            _file.open(_name, std::ios::out | std::ios::trunc);
            _tfu->setPath(_name);
        }
        ~TempFile() {
            if (_file.is_open()) {
                _file.flush();
                _file.close();
            }
            deleteFile(_tfu);
            _tfu->update();
        }
        void setPerms(OstreamPerms perms) {
            if (_file.is_open()) {
                _file.flush();
                _file.close();
            }
            _tfu->update();
            if (perms == READ) {
                _file.open(_name, std::ios::in);
            } else if (perms == TRUNCATE) {
                _file.open(_name, std::ios::out | std::ios::trunc);
            } else if (perms == APPEND) {
                _file.open(_name, std::ios::out | std::ios::app);
            }
            _tfu->update();
        }
        EZTools::EZString path() { return _name; }
        URI *uri() { return _tfu; }
        bool isThere() { return _tfu->isThere(); }
        bool isWriteable() { return _tfu->canWrite(); }
        bool isReadable() { return _tfu->canRead(); }
        bool isOpen() { return _file.is_open(); }
        void close() {
            _file.flush();
            _file.close();
        }
        EZTools::EZString contents() {
            auto s = copyFileToEZString(_tfu);
            _tfu->update();
            return s.data;
        }
        std::vector<EZTools::EZString> vector() {
            auto s = copyFileToEZString(_tfu);
            auto v = s.data.split("\n");
            _tfu->update();
            return v;
        }
        void clear() {
            setPerms(TRUNCATE);
            setPerms(APPEND);
        }
    private:
        EZTools::EZString _name;
        std::ofstream _file;
        URI* _tfu;

        template<typename T>
        friend std::ostream &operator<<(TempFile &tf, T op) {
            tf._file << op;
            tf._tfu->update();
            return tf._file;
        }
    };

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZFiles");
        std::stringstream ss;
        auto hd = homeDir();
        ss << "EZFiles::homeDir(): " << hd << std::endl;
        if (hd.empty()) {
            res.functionTest(R"(EZFiles::homeDir()");
            res.message("homeDir was empty");
            res.output(ss.str());
            return res;
        }
        auto td = tempDir();
        ss << "EZFiles::tempDir(): " << td << std::endl;
        if (td.empty()) {
            res.functionTest(R"(EZFiles::tempDir()");
            res.message("tempDir was empty");
            res.output(ss.str());
            return res;
        }
        auto cd = currentDir();
        ss << "EZFiles::currentDir(): " << cd << std::endl;
        if (cd.empty()) {
            res.functionTest(R"(EZFiles::currentDir()");
            res.message("currentDir was empty");
            res.output(ss.str());
            return res;
        }
        URI sf("/etc/fstab");
        ss << "EZFiles::URI(\"/etc/fstab\"): " << std::endl;
        ss << "EZFiles::URI(\"/etc/fstab\").isThere(): " << sf.isThere() << std::endl;
        if (!sf.isThere()) {
            res.functionTest(R"(EZFiles::URI.isThere()");
            res.message("URI.isThere() for /etc/fstab was false");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").canWrite(): " << sf.canWrite() << std::endl;
        if (sf.canWrite()) {
            res.functionTest(R"(EZFiles::URI.canWrite()");
            res.message("URI.canWrite() for /etc/fstab was true");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").canRead(): " << sf.canRead() << std::endl;
        if (!sf.canRead()) {
            res.functionTest(R"(EZFiles::URI.canRead()");
            res.message("URI.canRead() for /etc/fstab was false");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").canExec(): " << sf.canExec() << std::endl;
        if (sf.canExec()) {
            res.functionTest(R"(EZFiles::URI.canExec()");
            res.message("URI.canExec() for /etc/fstab was true");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").fileType(): " << EZTools::statfiletypeAsString(sf.fileType())
           << std::endl;
        if (sf.fileType() != EZTools::REG) {
            res.functionTest("EZFiles::URI.fileType()");
            res.message("URI.fileType() for /etc/fstab was not a regular file");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").created(): " << sf.created().ymdt() << std::endl;
        if (sf.created().ymdt().empty()) {
            res.functionTest("EZFiles::URI.created()");
            res.message("URI.created() for /etc/fstab was empty");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").lastModified(): " << sf.lastModified().ymdt() << std::endl;
        if (sf.lastModified().ymdt().empty()) {
            res.functionTest("EZFiles::URI.lastModified()");
            res.message("URI.lastModified() for /etc/fstab was empty");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").lastAccess(): " << sf.lastAccess().ymdt() << std::endl;
        if (sf.lastAccess().ymdt().empty()) {
            res.functionTest("EZFiles::URI.lastAccess()");
            res.message("URI.lastAccess() for /etc/fstab was empty");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::URI(\"/etc/fstab\").sizeBytes(): " << sf.sizeBites() << std::endl;
        if (sf.sizeBites() == 0) {
            res.functionTest("EZFiles::URI.sizeBytes()");
            res.message("URI.sizeBytes() for /etc/fstab was 0");
            res.output(ss.str());
            return res;
        }
        std::stringstream tfn;
        tfn << homeDir() << "TEST." << EZUUID::UUID_v4() << ".tmp" ;
        URI tmpFile(tfn.str());
        ss << tmpFile.path() << "\n";
        std::stringstream st;
        st << "This is a temp file\nwith two lines";
        ss << st.str() << std::endl;
        ss << "EZFiles::URI(" << tfn.str() << ")" << std::endl;
        ss << "EZFiles::copyStringToFile(tfn.str(), &tmpFile)" << std::endl;
        auto re = copyStringToFile(st.str(), &tmpFile);
        if (!re.wasSuccessful()) {
            res.functionTest("EZFiles::copyStringToFile(tfn.str(), &tmpFile)");
            res.message(re.message());
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::copyFileToVectorOfEZString(&tmpFile)" << std::endl;
        auto ree = copyFileToVectorOfEZString(&tmpFile);
        if (!ree.wasSuccessful()) {
            res.functionTest("EZFiles::copyFileToVectorOfEZString(&tmpFile)");
            res.message(ree.message());
            res.output(ss.str());
            return res;
        }
        if (ree.data.size() != 2) {
            res.functionTest("EZFiles::copyFileToVectorOfEZString(&tmpFile)");
            res.message("size was " + EZTools::EZString(ree.data.size()));
            res.output(ss.str());
            return res;
        }
        for (auto& sts : ree.data) {
            ss << sts << std::endl;
        }
        ss << "EZFiles::backupFile(&tmpFile,\"bak\")\n";
        auto rb = backupFile(&tmpFile,"bak");
        if (!rb.wasSuccessful()) {
            std::cout << "\n\nRUTROW\n\n" << std::endl;
            res.functionTest("EZFiles::backupFile(&tmpFile,\"bak\")");
            res.message(rb.message());
            res.output(ss.str());
            return res;
        }
        if (!rb.data.isThere()) {
            res.functionTest("EZFiles::backupFile(&tmpFile,\"bak\")");
            res.message(rb.data.path()+" was not found after creation");
            ss << rb.data.path()+" was not found after creation";
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::deleteFile(&rb.data)" << std::endl;
        auto rd = deleteFile(&rb.data);
        if (!rb.wasSuccessful()) {
            res.functionTest("EZFiles::deleteFile(&rb.data)");
            res.message(rd.message());
            res.output(ss.str());
            return res;
        }
        if (rb.data.isThere()) {
            res.functionTest("EZFiles::deleteFile(&rb.data)");
            res.message(rb.data.path() + " still there");
            res.output(ss.str());
            return res;
        }
        ss << "EZFiles::deleteFile(&tmpFile)" << std::endl;
        auto rdd = deleteFile(&tmpFile);
        if (!rb.wasSuccessful()) {
            res.functionTest("EZFiles::deleteFile(&tmpFile)");
            res.message(rdd.message());
            res.output(ss.str());
            return res;
        }
        if (tmpFile.isThere()) {
            res.functionTest("EZFiles::deleteFile(&tmpFile)");
            res.message(tmpFile.path() + " still there");
            res.output(ss.str());
            return res;
        }
        res.output(ss.str());
        res.wasSuccessful(true);
        return res;
    }

}

#endif //EZ_EZFILES_H