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

#ifndef CPPRESTTEST_EZHTTP_H
#define CPPRESTTEST_EZHTTP_H

#include <iostream>
#include <curl/curl.h>
#if defined(DISTRO_fc)
#include <json/json.h>
#include <json/reader.h>
#else
#include <jsoncpp/json/json.h>
#include <jsoncpp/json/reader.h>
#endif
#include <bits/stdc++.h>
#include <sstream>
#include <thread>
#include "EZTools.h"

using namespace std;
using namespace EZTools;

namespace EZHTTPFunctions {

    class EZHTTPDataType {
    public:
        void value(EZString dat) { _data = dat; }
        EZString value() { return _data; }
        //stringstream asStringstream() const;
        void httpResponseCode(long dat) { _HTTPResponseCode = dat; }
        long httpResponseCode() { return _HTTPResponseCode; }
        Json::Value asJSONCPP() const;

    private:
        EZString _data;
        long _HTTPResponseCode;
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
        EZHTTPDataType data;
        int responseTimeInMillis() { return _responseTime; }
        void responseTimeInMillis(int millis) { _responseTime = millis; }

    private:
        bool _isSuccess = false;
        CURLcode _code = CURLE_OK;
        int _responseTime = 0;
        EZString _errorMessage;
    };

    class EZHTTP {
    public:
        EZHTTP() = default;
        explicit EZHTTP(EZString userAgent);
        virtual ~EZHTTP() = default;
        void setUserAgent(EZString userAgent);
        EZHTTPResponse get(EZString URL);
        void setUsernamePassword(EZString username, EZString password);
        void setHasRealCert(bool badWebsite);
        vector<EZString> headers() { return _headers; }
        void addHeaderLine(EZString line);
        bool there(EZString uri);

    protected:

    private:
        EZString _userAgent;
        EZString _username;
        EZString _password;
        bool _realCert = true;
        vector<EZString> _headers;
        bool _isThereFlag = false;

        static size_t writer(char *data, size_t size, size_t nmemb, EZString *writerData) {
            if (writerData == nullptr)
                return 0;
            writerData->append(data, size * nmemb);
            return size * nmemb;
        }

        EZString curlError(CURLcode code) {
            switch (code) {
                case CURLE_UNSUPPORTED_PROTOCOL:
                    return "Protocol not supported";
                case CURLE_FAILED_INIT:
                    return "Very early initialization code failed";
                case CURLE_URL_MALFORMAT:
                    return "The URL was not properly formatted";
                case CURLE_COULDNT_RESOLVE_HOST:
                    return "Couldn't resolve host";
                case CURLE_COULDNT_CONNECT:
                    return "Failed to connect() to host or proxy";
                case CURLE_REMOTE_ACCESS_DENIED:
                    return "Denied access to the resource given in the URL";
//            case CURLE_HTTP2:
//                return "A problem was detected in the HTTP2 framing layer";
                case CURLE_OUT_OF_MEMORY:
                    return "A memory allocation request failed";
                case CURLE_OPERATION_TIMEDOUT:
                    return "Operation timeout";
                case CURLE_SSL_CONNECT_ERROR:
                    return "A problem occurred somewhere in the SSL/TLS handshake";
                case CURLE_FUNCTION_NOT_FOUND:
                    return "Function not found. A required zlib function was not found.";
                case CURLE_BAD_FUNCTION_ARGUMENT:
                    return "Internal error. A function was called with a bad parameter.";
                case CURLE_INTERFACE_FAILED:
                    return "Interface error";
                case CURLE_TOO_MANY_REDIRECTS:
                    return "Too many redirects";
                case CURLE_GOT_NOTHING:
                    return "Got nothing from the server";
//            case CURLE_HTTP2_STREAM:
//                return "Stream error in the HTTP/2 framing layer.";
                default:
                    return "Other curl error, see response.code";
            }
        }
    };
};

#endif //CPPRESTTEST_EZHTTP_H