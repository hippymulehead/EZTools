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

#ifndef FILESYSTEM_EZURI_H
#define FILESYSTEM_EZURI_H

#include <iostream>
#include <sstream>
#include <vector>
#include <unistd.h>
#include <boost/filesystem.hpp>
#include "EZString.h"
#include "EZHTTP.h"
#include "EZLinux.h"

enum EZFMFILETYPE {EZdirectory, EZfile, EZhttp, EZhttps, EZunknown};

struct EZPERMS {
    int stickyBit = 0;
    int owner = 0;
    int group = 0;
    int other = 0;
};

class EZURI {
public:
    EZURI(EZString uri, bool hasRealCert = true);
    EZURI(EZString uri, vector<EZString> headers, bool hasRealCert = true);
    EZURI(EZString uri, vector<EZString> headers, EZString username, EZString password, bool hasRealCert = true);
    virtual ~EZURI() = default;
    bool type() { return _type; }
    EZString typeAsString();
    uintmax_t size();
    //vector<EZURI> directoryContents();
    EZString filename() { return _filename.filename().string(); }
    boost::filesystem::path path() { return _filename; }
    EZString errorMessage() { return _errorMessage; }
    bool readable();
    bool writable();
    bool executable();
    bool exists();
    EZPERMS perms();
    EZString permsAsEZString();
    EZString uri();
    bool isLocal() { return _isLocal; }
    bool hasRealCert() { return _hasRealCert; }
    vector<EZString> headers() { return _headers; }
    EZString username() { return _username; }
    EZString password() { return _password; }
    EZString useragent() { return _useragent; }
    void useragent(EZString userAgentString) { _useragent = userAgentString; }

private:
    bool _exsists = false;
    bool _writable = false;
    bool _readable = false;
    bool _executable = false;
    bool _isLocal;
    EZString _uri;
    boost::filesystem::path _filename;
    EZFMFILETYPE _type = EZunknown;
    EZString _errorMessage;
    bool _hasRealCert;
    vector<EZString> _headers;
    EZString _username;
    EZString _password;
    EZString _useragent;
};


#endif //FILESYSTEM_EZURI_H
