//
// Created by mromans on 8/23/23.
//

#ifndef SITE_EXPORT_EZZIP_H
#define SITE_EXPORT_EZZIP_H

#include "EZFiles.h"
#include "EZLinux.h"

namespace EZZipFiles {

    class ZipFile {
    public:
        explicit ZipFile(EZTools::EZString filename) {
            _zipFile.setPath(filename);
            _zipLocation.setPath("/usr/bin/zip");
            _unzipLocation.setPath("/usr/bin/unzip");
            if (_zipLocation.canExec()) {
                _canZip = true;
            }
            if (_unzipLocation.canExec()) {
                _canUnzip = true;
            }
        }
        ~ZipFile() = default;
        bool canZip() { return _canZip; }
        bool canUnzip() { return _canUnzip; }
        EZFiles::URI* uri() {return &_zipFile; };
        EZTools::EZReturnNoData unzip(EZTools::EZString dest = "") {
            EZTools::EZReturnNoData res;
            if (!_canUnzip) {
                res.message("unzip util not found at /usr/bin/unzip");
                return res;
            }
            std::stringstream ss;
            EZTools::EZString d;
            if (!dest.empty()) {
                EZFiles::URI de(dest);
                if (!de.canWrite()) {
                    res.message("Can't write to " + de.path());
                    return res;
                }
                d = "-d " + dest;
            }
            ss << _unzipLocation.path() << " " << _zipFile.path() << " " << d;
            auto p = EZLinux::execAndPassControl(ss.str());
            if (p.data.childExitCode != 0) {
                res.message(p.message());
                return res;
            }
            res.wasSuccessful(true);
            return res;
        }
        EZTools::EZReturnNoData zip(EZTools::EZString source) {
            EZTools::EZReturnNoData res;
            if (!_canZip) {
                res.message("zip util not found at /usr/bin/zip");
                return res;
            }
            source.regexReplace("/$","");
            auto sp = source.splitAtLast("/");
            auto cd = EZLinux::changeDir(sp.at(0));
            if(!cd) {
                res.message(cd.message());
                return res;
            }
            EZFiles::URI s(sp.at(1));
            if (!s.canRead()) {
                res.message("Can't read from "+ s.path());
                return res;
            }
            std::stringstream ss;
            ss << _zipLocation.path() << " -r " << _zipFile.path() << " " << sp.at(1);
            auto p = EZLinux::execAndPassControl(ss.str());
            if (!p) {
                res.message(p.message());
                return res;
            }
            res.wasSuccessful(true);
            return res;
        }
    private:
        EZFiles::URI _zipFile;
        EZFiles::URI _zipLocation;
        EZFiles::URI _unzipLocation;
        bool _canUnzip = false;
        bool _canZip = false;
    };

}

#endif //SITE_EXPORT_EZZIP_H
