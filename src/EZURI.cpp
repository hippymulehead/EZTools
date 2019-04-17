//
// Created by mromans on 4/13/19.
//

#include "../include/EZURI.h"

EZURI::EZURI(EZString uri, bool hasRealCert) {
    if (uri.beginsWith("http://") | uri.beginsWith("https://")) {
        _isLocal = false;
        _uri = uri;
        _hasRealCert = hasRealCert;
        _type = uri.beginsWith("http://") ? EZhttp : EZhttps;
        EZHTTP http("EZURI v1.0 by Michael Romans of Romans Audio");
        http.setHasRealCert(hasRealCert);
        _exsists = http.there(uri);
        _readable = _exsists;
        _writable = false;
        _executable = false;
    } else {
        _isLocal = true;
        _filename = uri;
        try {
            if (EZLinux::isThere(_filename)) {
                _exsists = true;
                _readable = EZLinux::isReadable(_filename);
                if (boost::filesystem::is_directory(_filename)) {
                    _type = EZdirectory;
                } else if (boost::filesystem::is_regular_file(_filename)) {
                    _type = EZfile;
                }
            } else {
                stringstream s;
                s << _filename << " does not exist";
                _errorMessage = s.str();
            }
        }

        catch (const boost::filesystem::filesystem_error &ex) {
            stringstream s;
            s << _filename << ex.what();
            _errorMessage = s.str();
        }
    }
}

EZURI::EZURI(EZString uri, vector<EZString> headers, bool hasRealCert) {
    _uri = uri;
    _hasRealCert = hasRealCert;
    _headers = headers;
    _type = uri.beginsWith("http://") ? EZhttp : EZhttps;
    EZHTTP http("EZURI v1.0 by Michael Romans of Romans Audio");
    for (const auto & _header : _headers) {
        http.addHeaderLine(_header);
    }
    http.setHasRealCert(hasRealCert);
    _exsists = http.there(uri);
    _readable = _exsists;
    _writable = false;
    _executable = false;
}

EZURI::EZURI(EZString uri, vector<EZString> headers, EZString username, EZString password, bool hasRealCert) {
    _uri = uri;
    _hasRealCert = hasRealCert;
    _headers = headers;
    _username = username;
    _password = password;
    _type = uri.beginsWith("http://") ? EZhttp : EZhttps;
    EZHTTP http("EZURI v1.0 by Michael Romans of Romans Audio");
    for (const auto & _header : _headers) {
        http.addHeaderLine(_header);
    }
    http.setUsernamePassword(_username, _password);
    http.setHasRealCert(hasRealCert);
    _exsists = http.there(uri);
    _readable = _exsists;
    _writable = false;
    _executable = false;
}

EZString EZURI::typeAsString() {
    switch(_type) {
        case EZdirectory :
            return "directory";
        case EZfile :
            return "file";
        case EZhttp :
            return "http";
        case EZhttps :
            return "https";
        case EZunknown:
            return "unknown";
    }
    return "unknown";
}

uintmax_t EZURI::size() {
    return boost::filesystem::file_size(_filename);
}

//vector<EZURI> EZURI::directoryContents() {
//    vector<EZURI> c;
//    if (_type == EZdirectory) {
//        for (boost::filesystem::directory_entry &x : boost::filesystem::directory_iterator(_filename)) {
//            try {
//                EZURI f(x.path().generic_string());
//                if (f.exists()) {
//                    c.push_back(f);
//                }
//            } catch (const boost::filesystem::filesystem_error& ex) {
//                cout << "ERROR DIR: " << ex.what() << endl;
//            }
//        }
//    }
//    return c;
//}

EZPERMS EZURI::perms() {
    EZPERMS perms;
    boost::filesystem::perms p = boost::filesystem::status(_filename.generic_string()).permissions();
    if (p & boost::filesystem::perms::owner_read) {
        perms.owner = perms.owner + 4;
    }
    if (p & boost::filesystem::perms::owner_write) {
        perms.owner = perms.owner + 2;
    }
    if (p & boost::filesystem::perms::owner_exe) {
        perms.owner = perms.owner + 1;
    }
    if (p & boost::filesystem::perms::set_uid_on_exe) {
        perms.stickyBit = perms.stickyBit + 4;
    }
    if (p & boost::filesystem::perms::group_read) {
        perms.group = perms.group + 4;
    }
    if (p & boost::filesystem::perms::group_write) {
        perms.group = perms.group + 2;
    }
    if (p & boost::filesystem::perms::group_exe) {
        perms.group = perms.group + 1;
    }
    if (p & boost::filesystem::perms::set_gid_on_exe) {
        perms.stickyBit = perms.stickyBit + 2;
    }
    if (p & boost::filesystem::perms::others_read) {
        perms.other = perms.other + 4;
    }
    if (p & boost::filesystem::perms::others_write) {
        perms.other = perms.other + 2;
    }
    if (p & boost::filesystem::perms::others_exe) {
        perms.other = perms.other + 1;
    }
    return perms;
}

EZString EZURI::permsAsEZString() {
    EZPERMS perms = this->perms();
    stringstream s;
    s << perms.stickyBit << perms.owner << perms.group << perms.other;
    return s.str();
}

EZString EZURI::uri() {
    switch (_type) {
        case EZhttp:
            return _uri;
        case EZhttps:
            return _uri;
        default:
            return _filename.generic_string();
    }
}

bool EZURI::readable() {
    if (_type == EZhttps | _type == EZhttp) {
        return _readable;
    }
    return EZLinux::isReadable(_filename);
}

bool EZURI::writable() {
    if (_type == EZhttps | _type == EZhttp) {
        return _writable;
    }
    return EZLinux::isWritable(_filename);
}

bool EZURI::executable() {
    if (_type == EZhttps | _type == EZhttp) {
        return _executable;
    }
    return EZLinux::isExecutable(_filename);
}

bool EZURI::exists() {
    if (_type == EZhttps | _type == EZhttp) {
        return _exsists;
    }
    return EZLinux::isThere(_filename);
}
