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

#ifndef WEBTO_EZFILEMANAGER_H
#define WEBTO_EZFILEMANAGER_H

#include <fstream>
#include <iostream>
#include "EZLinux.h"
#include "EZHTTP.h"
#include "EZTools.h"

#if defined(DISTRO_fc)
#include <json/json.h>
#include <json/reader.h>
#else
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#endif

using namespace std;
using namespace EZTools;
using namespace EZLinux;
using namespace EZHTTPFunctions;

class EZURI {
public:
    explicit EZURI(EZString uri);
    ~EZURI() = default;
    bool isThere() { return _isThere; }
    bool isReadable() { return _isReadable; }
    bool isWriteable() { return _isWriteable; }
    int locationType() { return _locType; }
    EZString uri() { return _uri; }

private:
    EZString _uri;
    bool _isThere;
    bool _isReadable;
    bool _isWriteable;
    int _locType; // 0 = web, 1 = file;
};

class EZFileManager {
public:
    EZFileManager() = default;
    ~EZFileManager() = default;
    EZError webCopy(EZURI websource, Json::Value &jsonRoot);
    EZError webCopy(EZURI websource, EZURI filedest, bool overwrite = false);
    EZError fileCopy(EZURI filesource, EZURI filedest, bool overwrite = false);
    EZError fileMove(EZURI filesource, EZURI filedest, bool overwrite = false);

private:
    EZHTTPResponse _resp;
};


#endif //WEBTO_EZFILEMANAGER_H
