/*
Copyright (c) 2017-2021, Michael Romans of Romans Audio
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

#ifndef EZT_EZHTTP_H
#define EZT_EZHTTP_H

#include <cstdio>
#include <netdb.h>
#include <ifaddrs.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sstream>
#include "EZTools.h"
#include "EZDateTime.h"
#include "cxxurl/cxxurl_all.h"
#include "json.h"
using namespace CXXUrl;

namespace EZHTTP {

    class HTTP;

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
    private:
        std::map<EZTools::EZString, EZTools::EZString> _headers;
    };

    struct HTTPResponseMetaData {
        bool wasSuccessful = false;
        long httpResponseCode = 0;
        long curlCode = 0;
        EZTools::EZString message;
        int headerSize = 0;
        float timeInMillis;
        EZTools::EZString location;
    };

    class HTTPResponse {
    public:
        HTTPResponse() = default;
        ~HTTPResponse() = default;
        void data(std::string data) { _data = data; }
        EZTools::EZString data() { return _data; }
        void responseCode(long code) { metaData.curlCode = code; }
        long responseCode() { return metaData.curlCode; }
        void httpCode(long code) {metaData.httpResponseCode = code; }
        long httpCode() { return metaData.httpResponseCode; }
        void wasSuccessful(bool success) { metaData.wasSuccessful = success; }
        bool wasSuccessful() { return metaData.wasSuccessful; }
        void message(std::string text) { metaData.message = text; }
        EZTools::EZString message() { return metaData.message; }
        void headerSize(long size) { metaData.headerSize = size; }
        long headerSize() { return metaData.headerSize; }
        void headerString(EZTools::EZString headerstring) { _headers.init(headerstring); }
        void responseTimeInMillis(float t) { metaData.timeInMillis = t; }
        float responseTimeInMillis() { return metaData.timeInMillis; }
        HTTPHeaders headers() { return _headers; }
        nlohmann::json asJSON() {
            return nlohmann::json::parse(_data);
        }
        HTTPResponseMetaData metaData;
    private:
        EZTools::EZString _data;
        HTTPHeaders _headers;
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
            std::ostringstream contentOutput;
            std::ostringstream headers;
            if (_username.empty()) {
                Request request = RequestBuilder()
                        .url(_URILocation)
                        .curlOptionString(CURLOPT_TIMEOUT, EZTools::EZString(_timeout)+"L")
                        .verifySSL(_hasRealCert)
                        .contentOutput(&contentOutput)
                        .headerOutput(&headers)
                        .userAgent(_ua)
                        .noBody(true)
                        .build();
                auto const res = request.get();
                return res.getCode() == CURLE_OK;
            } else {
                Request request = RequestBuilder()
                        .url(_URILocation)
                        .curlOptionString(CURLOPT_TIMEOUT, EZTools::EZString(_timeout)+"L")
                        .curlOptionString(CURLOPT_USERNAME, _username)
                        .curlOptionString(CURLOPT_PASSWORD, _password)
                        .verifySSL(_hasRealCert)
                        .contentOutput(&contentOutput)
                        .headerOutput(&headers)
                        .userAgent(_ua)
                        .noBody(true)
                        .build();
                auto const res = request.get();
                return res.getCode() == CURLE_OK;
            }
        }
        int timeout() { return _timeout; }
        bool hasRealCert() { return _hasRealCert; }
        EZTools::EZString username() { return _username; }
        EZTools::EZString password() { return _password; }
        void auth(EZTools::EZString username, EZTools::EZString password) {
            _username = username;
            _password = password;
        }
        void timeout(int Timeout) { _timeout = Timeout; }
        EZTools::EZString userAgent() { return _ua; }
        void userAgent(EZTools::EZString useragent) { _ua = useragent; }
        void requestHeaders(HTTPHeaders headers) { _requestHeaders = headers; }
        HTTPHeaders requestHeaders() { return _requestHeaders; }

    private:
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
        HTTPHeaders _requestHeaders;

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
        HTTPResponse get(URI uri, bool headerOnly = false) {
            _uri = uri;
            _username = uri.username();
            _password = uri.password();
            _timeout = uri.timeout();
            EZDateTime::Timer t;
            t.start();
            std::ostringstream contentOutput;
            std::ostringstream headers;
            CXXUrl::RequestHeader RH;
            if (!_uri.requestHeaders().as_map().empty()) {
                for (auto& rh : _uri.requestHeaders().as_map()) {
                    std::stringstream ss;
                    ss << rh.first << ": " << rh.second;
                    RH.add(ss.str());
                }
            }
            if (_uri.username().empty()) {
                Request request = RequestBuilder()
                        .url(_uri.path())
                        .curlOptionString(CURLOPT_TIMEOUT, EZTools::EZString(_timeout))
                        .followLocation(_follow)
                        .verifySSL(_uri.hasRealCert())
                        .contentOutput(&contentOutput)
                        .headerOutput(&headers)
                        .userAgent(_uri.userAgent())
                        .noBody(headerOnly)
                        .requestHeader(&RH)
                        .build();
                auto const res = request.get();
                EZTools::EZString mes;
                bool ws = true;
                if (res.getCode() != CURLE_OK) {
                    mes = curlError(res.getCode());
                    ws = false;
                }
                t.stop();
                HTTPResponse hres;
                hres.data(contentOutput.str());
                hres.headerString(headers.str());
                hres.httpCode(res.getHTTPCode());
                hres.responseCode(res.getCode());
                hres.wasSuccessful(res.getCode() == CURLE_OK);
                hres.responseTimeInMillis(t.asMillisenconds());
                hres.message(curlError(res.getCode()));
                return hres;
            } else {
                Request request = RequestBuilder()
                        .url(_uri.path())
                        .curlOptionString(CURLOPT_TIMEOUT, EZTools::EZString(_timeout))
                        .curlOptionString(CURLOPT_USERNAME, _uri.username())
                        .curlOptionString(CURLOPT_PASSWORD, _uri.password())
                        .followLocation(_follow)
                        .verifySSL(_uri.hasRealCert())
                        .contentOutput(&contentOutput)
                        .headerOutput(&headers)
                        .userAgent(_uri.userAgent())
                        .noBody(headerOnly)
                        .requestHeader(&RH)
                        .build();
                auto const res = request.get();
                EZTools::EZString mes;
                bool ws = true;
                if (res.getCode() != CURLE_OK) {
                    mes = curlError(res.getCode());
                    ws = false;
                }
                t.stop();
                HTTPResponse hres;
                hres.data(contentOutput.str());
                hres.headerString(headers.str());
                hres.httpCode(res.getHTTPCode());
                hres.responseCode(res.getCode());
                hres.wasSuccessful(res.getCode() == CURLE_OK);
                hres.responseTimeInMillis(t.asMillisenconds());
                hres.message(curlError(res.getCode()));
                return hres;
            }
        }
        void follow(bool Follow) { _follow = Follow; }
        void hasGoodCert(bool goodCert) { _verifySSL = goodCert; }
        void requestHeader(HTTPHeaders reqHeader) { _reqHeader = reqHeader; }
        void setAuth(EZTools::EZString username, EZTools::EZString password) {
            _username = username;
            _password = password;
        }
        void userAgent(EZTools::EZString ua) { _ua = ua; }
        EZTools::EZString userAgent() { return _ua; }

    private:
        URI _uri;
        bool _follow = false;
        bool _verifySSL = true;
        int _timeout = 15;
        EZTools::EZString _ua;
        EZTools::EZString _username;
        EZTools::EZString _password;
        HTTPHeaders _reqHeader;
    };

}

#endif //EZT_EZHTTP_H
