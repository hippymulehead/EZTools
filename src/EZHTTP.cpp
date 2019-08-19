//
// Created by mromans on 12/7/18.
//

#include "EZHTTP.h"

EZHTTPFunctions::EZHTTP::EZHTTP(EZString userAgent) {
    _userAgent = userAgent;
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

void EZHTTPFunctions::EZHTTP::setUserAgent(EZString userAgent) {
    _userAgent = userAgent;
}

void EZHTTPFunctions::EZHTTP::setUsernamePassword(EZString username, EZString password) {
    _username = username;
    _password = password;
}

EZHTTPFunctions::EZHTTPResponse EZHTTPFunctions::EZHTTP::get(EZString URL) {
    thread_local EZString buffer;
    thread_local EZString headerbuffer;
    thread_local CURL *conn = nullptr;
    thread_local char errorBuffer[CURL_ERROR_SIZE];
    thread_local EZHTTPResponse response;
    thread_local long response_code;

    conn = curl_easy_init();
    curl_easy_reset(conn);

    if(conn == nullptr) {
        response.errorMessage("Failed to create CURL connection");
        response.isSuccess(false);
        return response;
    }

    if (URL.regexMatch("^https://")) {
        response.code(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYHOST, _realCert));
        response.code(curl_easy_setopt(conn, CURLOPT_SSL_VERIFYPEER, _realCert));
    }

    if (!_username.empty()) {
        response.code(curl_easy_setopt(conn, CURLOPT_USERNAME, _username.c_str()));
        if (response.code() != CURLE_OK) {
            response.errorMessage(curlError(response.code()));
            response.isSuccess(false);
            return response;
        }
        response.code(curl_easy_setopt(conn, CURLOPT_PASSWORD, _password.c_str()));
        if (response.code() != CURLE_OK) {
            response.errorMessage(curlError(response.code()));
            response.isSuccess(false);
            return response;
        }
    }

    if (_isThereFlag) {
        response.code(curl_easy_setopt(conn, CURLOPT_NOBODY, 1L));
        if (response.code() != CURLE_OK) {
            response.errorMessage(curlError(response.code()));
            response.isSuccess(false);
            return response;
        }
    }

    struct curl_slist *chunk = nullptr;
    for (auto & _header : _headers) {
        chunk = curl_slist_append(chunk, _header.c_str());
        response.code(curl_easy_setopt(conn, CURLOPT_HTTPHEADER, chunk));
        if (response.code() != CURLE_OK) {
            response.errorMessage(curlError(response.code()));
            response.isSuccess(false);
            return response;
        }
    }

    response.code(curl_easy_setopt(conn, CURLOPT_USERAGENT, _userAgent.c_str()));
    if (response.code() != CURLE_OK) {response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_VERBOSE, 0L));
    if (response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_ERRORBUFFER, errorBuffer));
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_URL, URL.c_str()));
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_FOLLOWLOCATION, 1L));
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_WRITEFUNCTION, writer));
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.code(curl_easy_setopt(conn, CURLOPT_WRITEDATA, &buffer));
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }

    EZDateTimeFunctions::EZTimer et;
    et.start();
    response.code(curl_easy_perform(conn));
    et.stop();
    response.responseTimeInMillis(et.asMillisenconds());

    curl_easy_cleanup(conn);
    if(response.code() != CURLE_OK) {
        response.errorMessage(this->curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    curl_easy_getinfo(conn, CURLINFO_RESPONSE_CODE, &response_code);
    response.data.value(buffer);
    response.data.httpResponseCode(response_code);
    buffer = "";
    response.isSuccess(response_code <= 399);
    return response;
}

void EZHTTPFunctions::EZHTTP::setHasRealCert(bool badWebsite) {
    _realCert = badWebsite;
}

void EZHTTPFunctions::EZHTTP::addHeaderLine(EZString line) {
    _headers.push_back(line);
}

bool EZHTTPFunctions::EZHTTP::there(EZString uri) {
    _isThereFlag = true;
    EZHTTPResponse response = this->get(uri);
    return response.isSuccess();
}

Json::Value EZHTTPFunctions::EZHTTPDataType::asJSONCPP() const {
    thread_local Json::Value root;
    stringstream d;
    d << _data;
    d >> root;
    return root;
}

//stringstream EZHTTPFunctions::EZHTTPDataType::asStringstream() const {
//    stringstream cc;
//    cc << _data;
//    return cc;
//}