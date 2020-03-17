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

#ifndef EZT_EZCONFIG_H
#define EZT_EZCONFIG_H

#include "EZFiles.h"
#include "json.h"

namespace EZConfig {

    class Config {
    public:
        nlohmann::json root;
        Config() = default;
        explicit Config(EZFiles::URI &confFile) {
            _configFile.setPath(confFile.path());
            _isThere = _configFile.isThere();
            if (_isThere) {
                auto r = copyFileToEZString(_configFile);
                root = nlohmann::json::parse(r.data);
            }
        }
        ~Config() = default;
        void init(EZFiles::URI &confFile) {
            _configFile.setPath(confFile.path());
            _isThere = _configFile.isThere();
            if (_isThere) {
                auto r = copyFileToEZString(_configFile);
                root = nlohmann::json::parse(r.data);
            }
        }
        bool isThere() { return _isThere; }
        EZTools::EZString asString() { return root.dump(4); }
        static EZTools::EZReturn<bool> write() {
            EZTools::EZReturn<bool> res;
//    EZFileStat stat(_filename);
//    if (stat.isWriteable()) {
//        if (isThere()) {
//            _json.writeToFile(_filename, EZOVERWRITE);
//        } else {
//            _json.writeToFile(_filename, EZNEW);
//        }
//        res.wasSuccessful(true);
//        return res;
//    } else {
//        res.message(_filename+" is not writeable");
//        return res;
//    }
            return res;
        }

    private:
        EZFiles::URI _configFile;
        bool _isThere;
    };

};


#endif //EZT_EZCONFIG_H
