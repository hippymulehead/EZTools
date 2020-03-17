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
either expressed or implied, of the EZTools project.
*/

#ifndef EZT_EZCSV_H
#define EZT_EZCSV_H

#include "EZTools.h"
#include "EZFiles.h"

namespace EZCSV {

    typedef std::vector<std::vector<EZTools::EZString>> ezcsv;

    class CSVFile {
    public:
        CSVFile(EZTools::EZString filename, bool hasHeaders = false) {
            bool head = false;
            _filename = filename;
            _file.setPath(filename);
            if (_file.isThere() && _file.isReadable()) {
                auto data = EZFiles::copyFileToEZString(_file);
                auto rs = data.data.regexSplit("\r|\n");
                for (auto& line : rs) {
                    int ty = 0;
                    if (line.regexMatch("^'")) {
                        line.regexReplace("^'|'$", "");
                        ty = 1;
                    } else if (line.regexMatch("^\"|\"$")) {
                        line.regexReplace("^\"|\"$", "");
                        ty = 2;
                    }
                    std::vector<EZTools::EZString> st;
                    switch (ty) {
                        case 0:
                            st = line.regexSplit(",");
                            break;
                        case 1:
                            st = line.regexSplit("','");
                            break;
                        case 2:
                            st = line.regexSplit("\",\"");
                            break;
                    }
                    if (!head && hasHeaders) {
                        _headers = st;
                        head = true;
                    }
                    _csv.push_back(st);
                }
            }
        }
        ~CSVFile() = default;
        ezcsv csv() { return _csv; }
        std::vector<EZTools::EZString> headers() { return _headers; }
        void addHeaders(std::vector<EZTools::EZString> headers) {
            _headers = headers;
        }
        unsigned long int columns() { return _csv.at(0).size(); }
        unsigned long int rows() { return _csv.size(); }
    private:
        std::vector<EZTools::EZString> _headers;
        EZTools::EZString _filename;
        EZFiles::URI _file;
        ezcsv _csv;
    };

}
#endif //EZT_EZCSV_H
