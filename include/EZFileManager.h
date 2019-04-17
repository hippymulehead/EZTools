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

#ifndef FILESYSTEM_EZFILEMANAGER_H
#define FILESYSTEM_EZFILEMANAGER_H

#include <jsoncpp/json/reader.h>
#include <vector>
#include <unistd.h>
#include "EZURI.h"
#include "EZSTATUS.h"

using namespace std;

class EZFileManager {
public:
    EZFileManager() = default;
    virtual ~EZFileManager() = default;
    EZSTATUS copy(EZURI &URIToCopy, EZURI &URIToWrite, bool overwrite = false);
    EZSTATUS move(EZURI &URIToMove, EZURI &URIToMoveTo);
    EZSTATUS remove(EZURI &URIToDelete);
    EZSTATUS backup(EZURI &URIToBackup, EZString newExtention);
    EZSTATUS readAsEZString(EZURI &URIToRead, EZString &storage);
    EZSTATUS readAsJsoncpp(EZURI &URIToRead, Json::Value &storage);
    EZSTATUS directoryContents(EZURI &URIDirectory, vector<EZURI> &storage, bool allFiles = false);
    EZSTATUS changeOwnership(EZString resource, EZString username, EZString group);
};

#endif //FILESYSTEM_EZFILEMANAGER_H
