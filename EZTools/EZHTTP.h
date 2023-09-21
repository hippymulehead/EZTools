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

#ifndef EZT_EZHTTP_H
#define EZT_EZHTTP_H

#define CPPHTTPLIB_OPENSSL_SUPPORT
#pragma once

#include "httplib.h"
#include "EZTools.h"
#include "EZDateTime.h"
#include "nlohmann/json.hpp"

namespace EZHTTP {

    struct urlParts_t {
        EZTools::EZString fullURL;
        EZTools::EZString type;
        EZTools::EZString domain;
        EZTools::EZString port;
        EZTools::EZString call;
        EZTools::EZString base;
        std::vector<EZTools::EZString> callVector;
        bool goodBreakDown = false;
    };

    inline urlParts_t breakDownURL(EZTools::EZString url) {
        urlParts_t t;
        if (url.regexMatch(R"(^(http:\/\/|https:\/\/)([a-z0-9+\-.]*)(:)([0-9]*)(.*)?)")) {
            auto u = url.regexExtract(R"(^(http:\/\/|https:\/\/)([a-z0-9+\-.]*)(:)([0-9]*)(.*)?)");
            t.type = u.at(1);
            t.domain = u.at(2);
            t.port = u.at(4);
            if (u.size() > 4) {
                t.call = u.at(5);
            }
            std::stringstream ss;
            ss << t.type << t.domain << ":" << t.port;
            t.base = ss.str();
            if (u.size() > 4) {
                EZTools::EZString cvs = t.call;
                cvs.regexReplace("^/", "");
                t.callVector = cvs.split("/");
                t.goodBreakDown = true;
            }
            t.fullURL = url;
            if (t.call.empty()) {
                t.call = "/";
            }
        } else if (url.regexMatch(R"(^(http:\/\/|https:\/\/)([a-z0-9+\-.]*)(.*)?)")) {
            auto u = url.regexExtract(R"(^(http:\/\/|https:\/\/)([a-z0-9+\-.]*)(.*)?)");
            t.type = u.at(1);
            t.domain = u.at(2);
            if (u.size() > 2) {
                t.call = u.at(3);
            }
            std::stringstream ss;
            ss << t.type << t.domain;
            t.base = ss.str();
            if (u.size() > 2) {
                EZTools::EZString cvs = t.call;
                cvs.regexReplace("^/", "");
                t.callVector = cvs.split("/");
                t.goodBreakDown = true;
            }
            t.fullURL = url;
            if (t.call.empty()) {
                t.call = "/";
            }
        }
        return t;
    }

    inline EZTools::EZString httpErrorAsString(httplib::Error error) {
        switch (error) {
            case httplib::Error::Success:
                return "Success";
            case httplib::Error::Unknown:
                return "Unknown";
            case httplib::Error::Connection:
                return "Connection";
            case httplib::Error::BindIPAddress:
                return "BindIPAddress";
            case httplib::Error::Read:
                return "Read";
            case httplib::Error::Write:
                return "Write";
            case httplib::Error::ExceedRedirectCount:
                return "ExceedRedirectCount";
            case httplib::Error::Canceled:
                return "Canceled";
            case httplib::Error::SSLConnection:
                return "SSLConnection";
            case httplib::Error::SSLLoadingCerts:
                return "SSLLoadingCerts";
            case httplib::Error::SSLServerVerification:
                return "SSLServerVerification";
            case httplib::Error::UnsupportedMultipartBoundaryChars:
                return "UnsupportedMultipartBoundaryChars";
            case httplib::Error::Compression:
                return "Compression";
        }
    }

    class Headers {
    public:
        Headers() = default;
        ~Headers() = default;
        void addPair(EZTools::EZString key, EZTools::EZString value) {
            _headers.insert({key,value});
        }
        httplib::Headers list() { return _headers; }
    private:
        httplib::Headers _headers;
    };

    class URI {
    public:
        URI() = default;
        explicit URI(EZTools::EZString url, int timeout = 15, bool hasGoodCert = true, bool follow = true) {
            _urlParts = breakDownURL(url);
            if (_urlParts.goodBreakDown) {
                _timeout = timeout;
                _follow = follow;
                _hasGoodCert = hasGoodCert;
                _isGood = true;
            } else {
                _isGood = false;
            }
        }
        ~URI() = default;
        EZTools::EZString path() { return _urlParts.fullURL; }
        bool canConnect() {
            std::stringstream ss;
            if (!_urlParts.port.empty()) {
                ss << _urlParts.type << _urlParts.base << ":" << _urlParts.port;
            } else {
                ss << _urlParts.type << _urlParts.base;
            }
            httplib::Client cli(ss.str());
            cli.set_read_timeout(_timeout);
            cli.set_connection_timeout(_timeout);
            cli.enable_server_certificate_verification(_hasGoodCert);
            _isGood = true;
            cli.set_default_headers(_headers);
            EZDateTime::Timer timer;
            timer.start();
            auto bok = cli.Get("/");
            timer.stop();
            _time = timer.asMillisenconds();
            if (bok.error() != httplib::Error::Success) {
                _message = httpErrorAsString(bok.error()) + " Error";
                _isGood = false;
            }
            return _isGood;
        }
        float connectTime() { return _time; }
        int timeout() { return _timeout; }
        void timeout(int Timeout) { _timeout = Timeout; }
        bool hasRealCert() { return _hasGoodCert; }
        void hasRealCert(bool HasRealCert) { _hasGoodCert = HasRealCert; }
        void basicAuth(EZTools::EZString username, EZTools::EZString password) {
            _basicAuth.first = username;
            _basicAuth.second = password;
        }
        std::pair<EZTools::EZString, EZTools::EZString> basicAuth() { return _basicAuth; }
        void requestHeaders(httplib::Headers headers) { _headers = headers; }
        [[nodiscard]] bool wasSuccessful() const { return _isGood; }
        EZTools::EZString message() { return _message; }
        bool follow() {return _follow; }
        EZTools::EZString type() { return _urlParts.type; }
        EZTools::EZString base() { return _urlParts.base; }
        EZTools::EZString port() {return _urlParts.port; }
        httplib::Headers requestHeaders() { return _headers; }
        urlParts_t urlParts() { return _urlParts; }
    private:
        std::pair<EZTools::EZString, EZTools::EZString> _basicAuth;
        httplib::Headers _headers;
        int _timeout;
        bool _isGood = false;
        bool _follow;
        bool _hasGoodCert;
        EZTools::EZString _message;
        float _time;
        urlParts_t _urlParts;
    };

    class EZReturnHTTP : public EZTools::EZReturnNoData {
    public:
        EZReturnHTTP() = default;
        ~EZReturnHTTP() = default;
        void location(EZTools::EZString Location) { _location = Location; }
        EZTools::EZString location() { return _location; }
        void httpCode(int code) {_httpResponseCode = code; }
        long httpCode() { return _httpResponseCode; }
        void headerSize(long size) { _headerSize = size; }
        long headerSize() { return _headerSize; }
//        void headerString(EZTools::EZString headerstring) { _headers.init(headerstring); }
        void responseTimeInMillis(float t) { _timeInMillis = t; }
        float responseTimeInMillis() { return _timeInMillis; }
//        HTTPHeaders headers() { return _headers; }
        nlohmann::json asJSON() {
            return nlohmann::json::parse(data);
        }
        EZTools::EZString data;
    private:
        int _httpResponseCode = 0;
        int _headerSize = 0;
        float _timeInMillis = 0;
        EZTools::EZString _location;
    };

    class HTTPClient {
    public:
        HTTPClient() = default;
        explicit HTTPClient(URI &uri) {
            _uri = uri;
        }
        ~HTTPClient() = default;
        void init(URI &uri) {
            _uri = uri;
        }
        bool follow() { return _uri.follow(); }
        EZReturnHTTP get() {
            EZReturnHTTP res;
            if (!_uri.wasSuccessful()) {
                res.message(_uri.message());
                return res;
            } else {
                res.location("EZHTTP::get");
                httplib::Client cli(_uri.urlParts().base);
                if (!_uri.basicAuth().first.empty()) {
                    cli.set_basic_auth(_uri.basicAuth().first.c_str(),
                                       _uri.basicAuth().second.c_str());
                }
                cli.enable_server_certificate_verification(_uri.hasRealCert());
                EZDateTime::Timer t;
                t.start();
                cli.set_follow_location(_uri.follow());
                cli.set_default_headers(_uri.requestHeaders());
                if (auto me = cli.Get(_uri.urlParts().call)) {
                    t.stop();
                    res.responseTimeInMillis(t.asMillisenconds());
                    res.httpCode(me->status);
                    if (me->status == 200) {
                        res.wasSuccessful(true);
                        res.data = me->body;
                    } else {
                        res.message("Error Code: " + EZTools::EZString(me->status));
                    }
                } else {
                    std::stringstream ss;
                    ss << "error code: " << me.error() << std::endl;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
                    auto result = cli.get_openssl_verify_result();
                    if (result) {
                        ss << "\nverify error: " << X509_verify_cert_error_string(result);
                    }
#endif
                    res.message(ss.str());
                }
                return res;
            }
        }
        EZReturnHTTP get(EZTools::EZString url) {
            EZReturnHTTP res;
            if (!_uri.wasSuccessful()) {
                res.message(_uri.message());
                return res;
            } else {
                res.location("EZHTTP::get(EZTools::EZString)");
                httplib::Client cli(_uri.urlParts().base);
                if (!_uri.basicAuth().first.empty()) {
                    cli.set_basic_auth(_uri.basicAuth().first.c_str(),
                                       _uri.basicAuth().second.c_str());
                }
                cli.enable_server_certificate_verification(_uri.hasRealCert());
                EZDateTime::Timer t;
                t.start();
                cli.set_follow_location(_uri.follow());
                cli.set_default_headers(_uri.requestHeaders());
                if (auto me = cli.Get(url)) {
                    t.stop();
                    res.responseTimeInMillis(t.asMillisenconds());
                    res.httpCode(me->status);
                    if (me->status == 200) {
                        res.wasSuccessful(true);
                        res.data = me->body;
                    } else {
                        res.message("Error Code: " + EZTools::EZString(me->status));
                    }
                } else {
                    std::stringstream ss;
                    ss << "error code: " << me.error() << std::endl;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
                    auto result = cli.get_openssl_verify_result();
                    if (result) {
                        ss << "\nverify error: " << X509_verify_cert_error_string(result);
                    }
#endif
                    res.message(ss.str());
                }
                return res;
            }
        }
        EZReturnHTTP head() {
            EZReturnHTTP res;
            if (!_uri.wasSuccessful()) {
                res.message(_uri.message());
                return res;
            } else {
                res.location("EZHTTP::get(EZTools::EZString)");
                httplib::Client cli(_uri.urlParts().base);
                if (!_uri.basicAuth().first.empty()) {
                    cli.set_basic_auth(_uri.basicAuth().first.c_str(),
                                       _uri.basicAuth().second.c_str());
                }
                cli.enable_server_certificate_verification(_uri.hasRealCert());
                EZDateTime::Timer t;
                t.start();
                cli.set_follow_location(_uri.follow());
                cli.set_default_headers(_uri.requestHeaders());
                if (auto me = cli.Head(_uri.urlParts().call)) {
                    t.stop();
                    res.responseTimeInMillis(t.asMillisenconds());
                    res.httpCode(me->status);
                    if (me->status == 200) {
                        res.wasSuccessful(true);
                        res.data = me->body;
                    } else {
                        res.message("Error Code: " + EZTools::EZString(me->status));
                    }
                } else {
                    std::stringstream ss;
                    ss << "error code: " << me.error() << std::endl;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
                    auto result = cli.get_openssl_verify_result();
                    if (result) {
                        ss << "\nverify error: " << X509_verify_cert_error_string(result);
                    }
#endif
                    res.message(ss.str());
                }
                return res;
            }
        }
        EZReturnHTTP head(EZTools::EZString url) {
            EZReturnHTTP res;
            if (!_uri.wasSuccessful()) {
                res.message(_uri.message());
                return res;
            } else {
                res.location("EZHTTP::get(EZTools::EZString)");
                httplib::Client cli(_uri.urlParts().base);
                if (!_uri.basicAuth().first.empty()) {
                    cli.set_basic_auth(_uri.basicAuth().first.c_str(),
                                       _uri.basicAuth().second.c_str());
                }
                cli.enable_server_certificate_verification(_uri.hasRealCert());
                EZDateTime::Timer t;
                t.start();
                cli.set_follow_location(_uri.follow());
                cli.set_default_headers(_uri.requestHeaders());
                if (auto me = cli.Head(url)) {
                    t.stop();
                    res.responseTimeInMillis(t.asMillisenconds());
                    res.httpCode(me->status);
                    if (me->status == 200) {
                        res.wasSuccessful(true);
                        res.data = me->body;
                    } else {
                        res.message("Error Code: " + EZTools::EZString(me->status));
                    }
                } else {
                    std::stringstream ss;
                    ss << "error code: " << me.error() << std::endl;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
                    auto result = cli.get_openssl_verify_result();
                    if (result) {
                        ss << "\nverify error: " << X509_verify_cert_error_string(result);
                    }
#endif
                    res.message(ss.str());
                }
                return res;
            }
        }
        void requestHeader(Headers headers) {
            _uri.requestHeaders(headers.list());
        }
        void userAgent(EZTools::EZString UserAgent) {
            _uri.requestHeaders().insert({"User-Agent", UserAgent});
        }
        void basicAuth(EZTools::EZString username, EZTools::EZString password) {
            _uri.basicAuth(username, password);
        }
    private:
        URI _uri;
    };

}

#endif //EZT_EZHTTP_H
