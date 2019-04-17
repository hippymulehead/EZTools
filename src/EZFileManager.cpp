//
// Created by mromans on 4/15/19.
//

#include "../include/EZFileManager.h"

EZSTATUS EZFileManager::copy(EZURI &URIToCopy, EZURI &URIToWrite, bool overwrite) {
    EZSTATUS stat;
    if (!URIToWrite.isLocal()) {
        stat.type(CURRENTLY_NOT_IMPLEMENTED);
        return stat;
    }
    if (!URIToCopy.readable()) {
        stat.type(SOURCE_NOT_READABLE);
        return stat;
    } else if (URIToWrite.exists() & !overwrite) {
        stat.type(WOULD_OVERWRITE_BUT_NOT_ALLOWED);
        return stat;
    }
    if (!URIToCopy.isLocal() & URIToWrite.isLocal()) {
        EZHTTP http;
        if (URIToWrite.useragent().empty()) {
            http.setUserAgent("EZFileManager v1.0 by Michael Romans of Romans Audio");
        } else {
            http.setUserAgent(URIToWrite.useragent());
        }
        http.setHasRealCert(URIToCopy.hasRealCert());
        for (auto i = 0; i < URIToCopy.headers().size(); i++) {
            http.addHeaderLine(URIToWrite.headers().at(i));
        }
        if (!URIToWrite.username().empty() & !URIToWrite.password().empty()) {
            http.setUsernamePassword(URIToWrite.username(), URIToWrite.password());
        }
        EZHTTPResponse r = http.get(URIToCopy.uri());
        if (r.isSuccess()) {
            ofstream dest(URIToWrite.path().generic_string(), ios::binary);
            dest << r.data.value();
            dest.close();
            if (dest) {
                stat.type(SUCCESS);
                return stat;
            } else {
                stat.type(UNKNOWN);
                return stat;
            }
        } else {
            stat.type(UNKNOWN);
            return stat;
        }
    } else if (URIToCopy.isLocal() & URIToWrite.isLocal()) {
        ifstream src(URIToCopy.path().generic_string(), ios::binary);
        ofstream dest(URIToWrite.path().generic_string(), ios::binary);
        dest << src.rdbuf();
        src.close();
        dest.close();
        if (src && dest) {
            stat.type(SUCCESS);
            return stat;
        } else {
            stat.type(UNKNOWN);
            return stat;
        }
    }
    stat.type(UNKNOWN);
    return stat;
}

EZSTATUS EZFileManager::remove(EZURI &URIToDelete) {
    EZSTATUS stat;
    if (!URIToDelete.isLocal()) {
        stat.type(FILE_NOT_LOCAL);
        return stat;
    }
    if (!URIToDelete.exists()) {
        stat.type(FILE_NOT_FOUND);
        return stat;
    }
    const EZString s = URIToDelete.uri();
    try {
        boost::filesystem::remove(URIToDelete.path());
    }
    catch (boost::filesystem::filesystem_error &e) {
        stat.type(PASSTHROUGH);
        stat.emessage(e.what());
        return stat;
    }
    stat.type(SUCCESS);
    return stat;
}

EZSTATUS EZFileManager::move(EZURI &URIToMove, EZURI &URIToMoveTo) {
    EZSTATUS stat;
    stat = this->copy(URIToMove, URIToMoveTo);
    if (!stat.success()) {
        return stat;
    }
    stat = this->remove(URIToMove);
    if (!stat.success()) {
        return stat;
    }
    stat.type(SUCCESS);
    return EZSTATUS();
}

EZSTATUS EZFileManager::backup(EZURI &URIToBackup, EZString newExtention) {
    EZSTATUS stat;
    if (!URIToBackup.isLocal()) {
        stat.type(FILE_NOT_LOCAL);
        return stat;
    }
    if (!URIToBackup.readable()) {
        stat.type(SOURCE_NOT_READABLE);
        return stat;
    }
    stringstream s;
    s << URIToBackup.path().generic_string() << "." << newExtention;
    EZURI backupfile(s.str());
    stat = this->copy(URIToBackup,backupfile, true);
    return stat;
}

EZSTATUS EZFileManager::readAsEZString(EZURI &URIToRead, EZString &storage) {
    EZSTATUS stat;
    if (!URIToRead.isLocal()) {
        EZHTTP http;
        if (URIToRead.useragent().empty()) {
            http.setUserAgent("EZFileManager v1.0 by Michael Romans of Romans Audio");
        } else {
            http.setUserAgent(URIToRead.useragent());
        }
        http.setHasRealCert(URIToRead.hasRealCert());
        for (auto i = 0; i < URIToRead.headers().size(); i++) {
            http.addHeaderLine(URIToRead.headers().at(i));
        }
        if (!URIToRead.username().empty() & !URIToRead.password().empty()) {
            http.setUsernamePassword(URIToRead.username(), URIToRead.password());
        }
        EZHTTPResponse r = http.get(URIToRead.uri());
        if (r.isSuccess()) {
            storage = r.data.value();
            stat.type(SUCCESS);
        } else {
            stat.type(PASSTHROUGH);
            stat.emessage(r.errorMessage());
        }
    }
    if (URIToRead.isLocal()) {
        ifstream src(URIToRead.path().generic_string(), ios::binary);
        stringstream ss;
        ss << src.rdbuf();
        src.close();
        if (src && 1) {
            storage = ss.str();
            stat.type(SUCCESS);
        } else {
            stat.type(UNKNOWN);
        }
    }
    return stat;
}

EZSTATUS EZFileManager::readAsJsoncpp(EZURI &URIToRead, Json::Value &storage) {
    EZSTATUS stat;
    if (!URIToRead.isLocal()) {
        EZHTTP http;
        if (URIToRead.useragent().empty()) {
            http.setUserAgent("EZFileManager v1.0 by Michael Romans of Romans Audio");
        } else {
            http.setUserAgent(URIToRead.useragent());
        }
        http.setHasRealCert(URIToRead.hasRealCert());
        for (auto i = 0; i < URIToRead.headers().size(); i++) {
            http.addHeaderLine(URIToRead.headers().at(i));
        }
        if (!URIToRead.username().empty() & !URIToRead.password().empty()) {
            http.setUsernamePassword(URIToRead.username(), URIToRead.password());
        }
        EZHTTPResponse r = http.get(URIToRead.uri());
        if (r.isSuccess()) {
            stringstream d;
            d << r.data.value();
            d >> storage;
            stat.type(SUCCESS);
        } else {
            stat.type(PASSTHROUGH);
            stat.emessage(r.errorMessage());
        }
    }
    if (URIToRead.isLocal()) {
        ifstream src(URIToRead.path().generic_string(), ios::binary);
        stringstream ss;
        ss << src.rdbuf();
        src.close();
        if (src && 1) {
            stringstream d;
            d << ss.str();
            d >> storage;
            stat.type(SUCCESS);
        } else {
            stat.type(UNKNOWN);
        }
    }
    return stat;
}

EZSTATUS EZFileManager::directoryContents(EZURI &URIDirectory, vector<EZURI> &storage, bool allFiles) {
    EZSTATUS stat;
    if (!URIDirectory.type() == EZdirectory) {
        stat.type(NOT_A_DIRECTORY);
    } else {
        for (boost::filesystem::directory_entry &x : boost::filesystem::directory_iterator(URIDirectory.path())) {
            try {
                EZURI f(x.path().generic_string());
                if (f.exists()) {
                    if (allFiles) {
                        storage.push_back(f);
                    } else {
                        if (!f.filename().beginsWith(".")) {
                            storage.push_back(f);
                        }
                    }
                }
                stat.type(SUCCESS);
            } catch (const boost::filesystem::filesystem_error& ex) {
                stat.type(PASSTHROUGH);
                stat.emessage(ex.what());
            }
        }
    }
    return stat;
}

EZSTATUS EZFileManager::changeOwnership(EZString resource, EZString username, EZString group) {
    EZSTATUS stat;
    uid_t          uid;
    gid_t          gid;
    struct passwd *pwd;
    struct group  *grp;

    pwd = getpwnam(username.c_str());
    if (pwd == nullptr) {
        stat.type(USERNAME_ERROR);
        return stat;
    }
    uid = pwd->pw_uid;

    grp = getgrnam(group.c_str());
    if (grp == nullptr) {
        stat.type(GROUP_ERROR);
        return stat;
    }
    gid = grp->gr_gid;

    int val = chown(resource.c_str(), uid, gid);
    if (val == 0) {
        stat.type(SUCCESS);
    } else {
        stat.type(CHOWN_ERROR);
    }
    return stat;
}


