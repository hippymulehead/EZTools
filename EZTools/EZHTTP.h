/*
Copyright (c) 2018-2020, Michael Romans of Romans Audio
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

#ifndef EZHTTP_EZHTTP2_H
#define EZHTTP_EZHTTP2_H

#include <cstdio>
#include <netdb.h>
#include <ifaddrs.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <curl/curl.h>
#include <map>
#include "EZTools.h"
#include "EZDateTime.h"

namespace EZHTTP {

    enum CONNECT_T {HTTP_T, HTTPS_T};

    class HTTPHeaders {
    public:
        HTTPHeaders() = default;
        HTTPHeaders(EZTools::EZString headerString) {
            auto hsplit = headerString.split("\n");
            for (auto& line : hsplit) {
                std::vector<EZTools::EZString> lb = line.split(":");
                if (lb.size() > 1) {
                    lb.at(1).regexReplace("^ ", "");
                    _headers.insert(std::pair<EZTools::EZString, EZTools::EZString>(lb.at(0), lb.at(1)));
                } else {
                    _headers.insert(std::pair<EZTools::EZString, EZTools::EZString>(lb.at(0), ""));
                }
            }
        }
        ~HTTPHeaders() = default;
        void init(EZTools::EZString headerString) {
            _headers.clear();
            auto hsplit = headerString.split("\n");
            for (auto& line : hsplit) {
                std::vector<EZTools::EZString> lb = line.split(":");
                if (lb.size() > 1) {
                    lb.at(1).regexReplace("^ ", "");
                    _headers.insert(std::pair<EZTools::EZString, EZTools::EZString>(lb.at(0), lb.at(1)));
                } else {
                    _headers.insert(std::pair<EZTools::EZString, EZTools::EZString>(lb.at(0), ""));
                }
            }
        }
        void addPair(EZTools::EZString key, EZTools::EZString value) {
            _headers.insert(std::pair<EZTools::EZString, EZTools::EZString>(key, value));
        }
        std::map<EZTools::EZString, EZTools::EZString> as_map() { return _headers; }
        EZTools::EZString as_string() {
            std::stringstream ss;
            for (auto& m : _headers) {
                if (m.second.empty()) {
                    ss << m.first << std::endl;
                } else {
                    ss << m.first << ": " << m.second << std::endl;
                }
            }
            return ss.str();
        }
        std::vector<std::string> as_stringList() {
            std::vector<std::string> reqHeader;
            auto sp = as_string().split("\n");
            for (auto& line : sp) {
                reqHeader.emplace_back(line);
            }
            return reqHeader;
        }
        int responseCode() {
            EZTools::EZString code;
            auto hsv = as_string().split("\n");
            for (auto& line : hsv) {
                if (line.regexMatch("^HTTP/")) {
                    auto csp = line.split(" ");
                    code = csp.at(1);
                }
            }
            return code.asInt();
        }
    private:
        std::map<EZTools::EZString, EZTools::EZString> _headers;
    };

    class HTTPResponse {
    public:
        HTTPResponse() = default;
        ~HTTPResponse() = default;
        void data(std::string data) { _data = data; }
        EZTools::EZString data() { return _data; }
        void responseCode(CURLcode code) { _code = code; }
        CURLcode responseCode() { return _code; }
        void wasSuccessful(bool success) { _success = success; }
        bool wasSuccessful() { return _success; }
        void message(std::string text) { _message = text; }
        EZTools::EZString message() { return _message; }
        void contentType(std::string type) { _contentType = type; }
        EZTools::EZString contentType() { return _contentType; }
        void headerSize(long size) { _headerSize = size; }
        long headerSize() { return _headerSize; }
        void headerString(EZTools::EZString headerstring) { _headers.init(headerstring); }
        void responseTimeInMillis(float t) { _t = t; }
        float responseTimeInMillis() { return _t; }
        HTTPHeaders headers() { return _headers; }
    private:
        EZTools::EZString _data;
        CURLcode _code;
        bool _success = false;
        EZTools::EZString _message;
        std::string _contentType;
        long _headerSize = 0;
        HTTPHeaders _headers;
        float _t;
    };

    class URI {
    public:
        enum URI_T{URI_HTTP, URI_HTTPS};
        URI() = default;
        URI(EZTools::EZString URILocation, int timeout = 15, EZTools::EZString username = "",
            EZTools::EZString password = "", bool hasRealCert = true) {
            _timeout = timeout;
            _hasRealCert = hasRealCert;
            _username = username;
            _password = password;
            EZTools::EZString temp = URILocation;
            _URILocation = URILocation;
            if (URILocation.regexMatch("^https://")) {
                _type = URI_HTTPS;
                _port = 443;
                temp.regexReplace("^https://", "");
            } else {
                _type = URI_HTTP;
                _port = 80;
                temp.regexReplace("^http://", "");
            }
            auto urisplit = temp.split("/");
            if (urisplit.at(0).regexMatch(":")) {
                auto basesplit = urisplit.at(0).split(":");
                _basepath = basesplit.at(0);
                _port = basesplit.at(1).asInt();
            } else {
                _basepath = urisplit.at(0);
            }
        }
        ~URI() = default;
        EZTools::EZString path() { return _URILocation; }
        EZTools::EZString basepath() { return _basepath; }
        bool canConnect() {
            int portno     = _port;
            int sockfd;
            struct sockaddr_in serv_addr;
            struct hostent *server;

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                return false;
            }
            server = gethostbyname(_basepath.c_str());
            if (server == nullptr) {
                return false;
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr,
                  (char *)&serv_addr.sin_addr.s_addr,
                  server->h_length);

            serv_addr.sin_port = htons(portno);
            EZDateTime::Timer et;
            et.start();
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
                et.stop();
                _time = et.asMillisenconds();
                close(sockfd);
                return false;
            } else {
                et.stop();
                _time = et.asMillisenconds();
                close(sockfd);
                return true;
            }
        }
        float connectTime() { return _time; }
        bool isThere() {
            thread_local HTTPResponse response;
            thread_local std::stringstream result;
            thread_local EZTools::EZString buffer;
            thread_local EZTools::EZString headerbuffer;
            thread_local CURL *conn = nullptr;
            thread_local char errorBuffer[CURL_ERROR_SIZE];
            thread_local long response_code;

            conn = curl_easy_init();
            curl_easy_reset(conn);
            if(conn == nullptr) {
                response.message("Failed to create CURL connection");
                response.wasSuccessful(false);
                return false;
            }

            curl_easy_setopt(conn, CURLOPT_TIMEOUT, _timeout);

            if (_URILocation.regexMatch("^https://")) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, _hasRealCert));
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, _hasRealCert));
            }

            if (!_username.empty()) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_USERNAME, _username.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    return false;
                }
                response.responseCode(curl_easy_setopt(conn, CURLOPT_PASSWORD, _password.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    return false;
                }
            }

            response.responseCode(curl_easy_setopt(conn, CURLOPT_NOBODY, 1L));
            if (response.responseCode() != CURLE_OK) {
                return false;
            }

            struct curl_slist *chunk = nullptr;
            for (auto & _header : _headers.as_stringList()) {
                chunk = curl_slist_append(chunk, _header.c_str());
                response.responseCode(curl_easy_setopt(conn, CURLOPT_HTTPHEADER, chunk));
                if (response.responseCode() != CURLE_OK) {
                    return false;
                }
            }

            response.responseCode(curl_easy_setopt(conn, CURLOPT_USERAGENT, _ua.c_str()));
            if (response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_VERBOSE, 0L));
            if (response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer));
            if(response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_URL, _URILocation.c_str()));
            if(response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L));
            if (response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer));
            if(response.responseCode() != CURLE_OK) {
                return false;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer));
            if(response.responseCode() != CURLE_OK) {
                return false;
            }
            curl_easy_setopt(conn, CURLOPT_HEADER, 1);

            EZDateTime::Timer et;
            et.start();
            response.responseCode(curl_easy_perform(conn));
            et.stop();
            response.responseTimeInMillis(et.asMillisenconds());

            curl_easy_cleanup(conn);

            return response.responseCode() == CURLE_OK;
        }
        int timeout() { return _timeout; }
        bool hasRealCert() { return _hasRealCert; }
        EZTools::EZString username() { return _username; }
        EZTools::EZString password() { return _password; }

    private:
        HTTPHeaders _headers;
        HTTPHeaders _reqHeader;
        EZTools::EZString _ua = "EZHTTPURI by Michael Romans";
        float _time;
        int _timeout = 15;
        bool _hasRealCert;
        EZTools::EZString _username;
        EZTools::EZString _password;
        URI_T _type;
        int _port;
        EZTools::EZString _URILocation;
        EZTools::EZString _basepath;

        EZTools::EZString pathType(URI_T type) {
            switch (type) {
                case URI_HTTP:
                    return "http";
                case URI_HTTPS:
                    return "https";
                default:
                    return "Unknown";
            }
        }

        static size_t writer(char *data, size_t size, size_t nmemb, EZTools::EZString *writerData) {
            if (writerData == nullptr)
                return 0;
            writerData->append(data, size * nmemb);
            return size * nmemb;
        }
    };

    class HTTP {
    public:
        HTTP() = default;
        HTTP(EZTools::EZString userAgent) { _ua = userAgent; }
        HTTP(EZTools::EZString userAgent, int timeout, bool follow = true) {
            _ua = userAgent;
            _timeout = timeout;
            _follow = follow;
        }
        ~HTTP() = default;
        HTTPResponse get(URI uri, bool verbose = false) {
            _username = uri.username();
            _password = uri.password();
            _timeout = uri.timeout();
            thread_local HTTPResponse response;
            thread_local std::stringstream result;
            thread_local EZTools::EZString buffer;
            thread_local EZTools::EZString headerbuffer;
            thread_local CURL *conn = nullptr;
            thread_local char errorBuffer[CURL_ERROR_SIZE];
            thread_local long response_code;

            conn = curl_easy_init();
            curl_easy_reset(conn);
            if(conn == nullptr) {
                response.message("Failed to create CURL connection");
                response.wasSuccessful(false);
                return response;
            }

            curl_easy_setopt(conn, CURLOPT_TIMEOUT, _timeout);

            if (uri.path().regexMatch("^https://")) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, _hasRealCert));
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, _hasRealCert));
            }

            if (!_username.empty()) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_USERNAME, _username.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
                response.responseCode(curl_easy_setopt(conn, CURLOPT_PASSWORD, _password.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }

            struct curl_slist *chunk = nullptr;
            for (auto & _header : _headers.as_stringList()) {
                chunk = curl_slist_append(chunk, _header.c_str());
                response.responseCode(curl_easy_setopt(conn, CURLOPT_HTTPHEADER, chunk));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }

            response.responseCode(curl_easy_setopt(conn, CURLOPT_USERAGENT, _ua.c_str()));
            if (response.responseCode() != CURLE_OK) {response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_VERBOSE, 0L));
            if (response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_URL, uri.path().c_str()));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            if (_follow) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            curl_easy_setopt(conn, CURLOPT_HEADER, 1);

            EZDateTime::Timer et;
            et.start();
            response.responseCode(curl_easy_perform(conn));
            et.stop();
            response.responseTimeInMillis(et.asMillisenconds());

            curl_easy_cleanup(conn);

            if(response.responseCode() != CURLE_OK) {
                response.message(this->curlError(response.responseCode()));
                return response;
            }
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &response_code);
            std::vector<EZTools::EZString> resSplit = buffer.split("\n");
            int i = 0;
            std::stringstream hstring, bstring;
            for (auto& line : resSplit) {
                line.removeNonPrintable();
                if (line == "") {
                    i = 1;
                } else {
                    switch (i) {
                        case 0:
                            hstring << line << "\n";
                            break;
                        case 1:
                            bstring << line << "\n";
                            break;
                    }
                }
            }
            response.data(bstring.str());
            response.headerString(hstring.str());
            buffer = "";
            response.wasSuccessful(response.responseCode() == CURLE_OK);
            return response;
        }
        HTTPResponse head(URI uri, bool verbose = false) {
            _username = uri.username();
            _password = uri.password();
            _timeout = uri.timeout();
            thread_local HTTPResponse response;
            thread_local std::stringstream result;
            thread_local EZTools::EZString buffer;
            thread_local EZTools::EZString headerbuffer;
            thread_local CURL *conn = nullptr;
            thread_local char errorBuffer[CURL_ERROR_SIZE];
            thread_local long response_code;
            conn = curl_easy_init();
            curl_easy_reset(conn);
            if(conn == nullptr) {
                response.message("Failed to create CURL connection");
                response.wasSuccessful(false);
                return response;
            }

            curl_easy_setopt(conn, CURLOPT_TIMEOUT, _timeout);

            if (uri.path().regexMatch("^https://")) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, _hasRealCert));
                response.responseCode(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, _hasRealCert));
            }

            if (!_username.empty()) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_USERNAME, _username.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
                response.responseCode(curl_easy_setopt(conn, CURLOPT_PASSWORD, _password.c_str()));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }

            response.responseCode(curl_easy_setopt(conn, CURLOPT_NOBODY, 1L));
            if (response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }

            struct curl_slist *chunk = nullptr;
            for (auto & _header : _headers.as_stringList()) {
                chunk = curl_slist_append(chunk, _header.c_str());
                response.responseCode(curl_easy_setopt(conn, CURLOPT_HTTPHEADER, chunk));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }

            response.responseCode(curl_easy_setopt(conn, CURLOPT_USERAGENT, _ua.c_str()));
            if (response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_VERBOSE, 0L));
            if (response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_URL, uri.path().c_str()));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            if (_follow) {
                response.responseCode(curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L));
                if (response.responseCode() != CURLE_OK) {
                    response.message(curlError(response.responseCode()));
                    return response;
                }
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                response.wasSuccessful(false);
                return response;
            }
            response.responseCode(curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer));
            if(response.responseCode() != CURLE_OK) {
                response.message(curlError(response.responseCode()));
                return response;
            }
            curl_easy_setopt(conn, CURLOPT_HEADER, 1);

            EZDateTime::Timer et;
            et.start();
            response.responseCode(curl_easy_perform(conn));
            et.stop();
            response.responseTimeInMillis(et.asMillisenconds());

            curl_easy_cleanup(conn);

            if(response.responseCode() != CURLE_OK) {
                response.message(this->curlError(response.responseCode()));
                return response;
            }
            curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &response_code);
            std::vector<EZTools::EZString> resSplit = buffer.split("\n");
            int i = 0;
            std::stringstream hstring, bstring;
            for (auto& line : resSplit) {
                line.removeNonPrintable();
                if (line == "") {
                    i = 1;
                } else {
                    switch (i) {
                        case 0:
                            hstring << line << "\n";
                            break;
                        case 1:
                            bstring << line << "\n";
                            break;
                    }
                }
            }
            response.headerString(hstring.str());
            buffer = "";
            response.wasSuccessful(response.responseCode() == CURLE_OK);
            return response;
        }

        void follow(bool follow) { _follow = follow; }
        bool follow() { return _follow; }
        HTTPHeaders requestHeader() { return _reqHeader; }
        void requestHeader(HTTPHeaders reqHeader) { _reqHeader = reqHeader; }
        void setAuth(EZTools::EZString username, EZTools::EZString password) {
            _username = username;
            _password = password;
        }
        void userAgent(EZTools::EZString ua) { _ua = ua; }
        EZTools::EZString userAgent() { return _ua; }

    private:
        HTTPHeaders _headers;
        HTTPHeaders _reqHeader;
        bool _hasRealCert = true;
        EZTools::EZString _ua = "Block me because I'm lazy";
        int _timeout = 15;
        bool _follow = true;
        EZTools::EZString _username;
        EZTools::EZString _password;
        bool _isGood;
        int _port;
        float _time;

        static size_t writer(char *data, size_t size, size_t nmemb, EZTools::EZString *writerData) {
            if (writerData == nullptr)
                return 0;
            writerData->append(data, size * nmemb);
            return size * nmemb;
        }

        EZTools::EZString curlError(CURLcode code) {
            std::stringstream ss;
            ss << code << ": ";
            switch (code) {
                case CURLE_UNSUPPORTED_PROTOCOL:
                    ss << "Protocol not supported";
                    break;
                case CURLE_FAILED_INIT:
                    ss << "Very early initialization code failed";
                    break;
                case CURLE_URL_MALFORMAT:
                    ss << "The URL was not properly formatted";
                    break;
                case CURLE_COULDNT_RESOLVE_HOST:
                    ss << "Couldn't resolve host";
                    break;
                case CURLE_COULDNT_CONNECT:
                    ss << "Failed to connect() to host or proxy";
                    break;
                case CURLE_REMOTE_ACCESS_DENIED:
                    ss << "Denied access to the resource given in the URL";
                    break;
//            case CURLE_HTTP2:
//                return "A problem was detected in the HTTP2 framing layer";
                case CURLE_OUT_OF_MEMORY:
                    ss << "A memory allocation request failed";
                    break;
                case CURLE_OPERATION_TIMEDOUT:
                    ss << "Operation timeout";
                    break;
                case CURLE_SSL_CONNECT_ERROR:
                    ss << "A problem occurred somewhere in the SSL/TLS handshake";
                    break;
                case CURLE_FUNCTION_NOT_FOUND:
                    ss <<  "Function not found. A required zlib function was not found.";
                    break;
                case CURLE_BAD_FUNCTION_ARGUMENT:
                    ss << "Internal error. A function was called with a bad parameter.";
                    break;
                case CURLE_INTERFACE_FAILED:
                    ss << "Interface error";
                    break;
                case CURLE_TOO_MANY_REDIRECTS:
                    ss << "Too many redirects";
                    break;
                case CURLE_GOT_NOTHING:
                    ss << "Got nothing from the server";
                    break;
//            case CURLE_HTTP2_STREAM:
//                return "Stream error in the HTTP/2 framing layer.";
                default:
                    ss << "Other curl error, see response.code";
                    break;
            }
            return ss.str();
        }
    };

};


#endif //EZHTTP2_EZHTTP2_H