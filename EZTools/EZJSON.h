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

#ifndef EZT_EZJSON_H
#define EZT_EZJSON_H

#pragma once

#include "EZTools.h"
#include "EZCSV.h"
#include "nlohmann/json.hpp"

class EZJSON {
public:
    nlohmann::json root;
    EZJSON() = default;
    EZJSON(EZTools::EZString jsonString) {
        root = nlohmann::json::parse(jsonString);
    }
    EZJSON(EZCSV::CSVFile csvFile) {
        std::stringstream ss;
        ss << "[";
        for (auto line : csvFile.csv()) {
            ss << "{";
            for (unsigned long int i = 0; i < csvFile.columns(); i++) {
                if (line.at(i).isDouble()) {
                    ss << "\"" << csvFile.headers()[i] << "\": " << line[i];
                } else {
                    ss << "\"" << csvFile.headers()[i] << "\": \"" << line[i] << "\"";
                }
                if (i != csvFile.columns() - 1) {
                    ss << ",";
                }
            }
            ss << "},";
        }
        EZTools::EZString sss = ss.str();
        sss.regexReplace(",$", "");
        std::stringstream s;
        s << sss;
        s << "]";
        root = nlohmann::json::parse(s);
    }
    ~EZJSON() = default;
private:
    bool debug = true;
};

#endif //EZT_EZJSON_H
