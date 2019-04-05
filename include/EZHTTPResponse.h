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

#ifndef EZTOOLS_EZHTTPRESPONSE_H
#define EZTOOLS_EZHTTPRESPONSE_H

#include <iostream>
#include <curl/curl.h>
#include <jsoncpp/json/reader.h>
#include <sstream>
#include "EZString.h"

using namespace std;

class SimpleHTTPDataType {
public:
    void value(EZString dat) { _data = dat; }
    EZString value() { return _data; }
    stringstream asStringstream() const {
        stringstream cc;
        cc << _data;
        return cc;
    }
    Json::Value asJSONCPP() const {
        thread_local Json::Value root;
        stringstream d;
        d << _data;
        d >> root;
        return root;
    }

private:
    EZString _data;
};

class EZHTTPResponse {
public:
    EZHTTPResponse() = default;
    virtual ~EZHTTPResponse() = default;
    bool isSuccess() { return _isSuccess; }
    void isSuccess(bool tf) { _isSuccess = tf; }
    CURLcode code() { return _code; }
    void code(CURLcode c) { _code = c; }
    EZString errorMessage() { return _errorMessage; }
    void errorMessage(EZString error) { _errorMessage = error; }
    SimpleHTTPDataType data;
    int responseTimeInMillis() { return _responseTime; }
    void responseTimeInMillis(int millis) { _responseTime = millis; }

private:
    bool _isSuccess = false;
    CURLcode _code = CURLE_OK;
    int _responseTime = 0;
    EZString _errorMessage;
};

#endif //EZTOOLS_EZHTTPRESPONSE_H