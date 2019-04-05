//
// Created by mromans on 12/7/18.
//

#include "../include/EZHTTP.h"
#include <bits/stdc++.h>

EZHTTP::EZHTTP(EZString userAgent) {
    _userAgent = userAgent;
}

void EZHTTP::setUserAgent(EZString userAgent) {
    _userAgent = userAgent;
}

void EZHTTP::setUsernamePassword(EZString username, EZString password) {
    _username = username;
    _password = password;
}

EZHTTPResponse EZHTTP::get(EZString URL) {
    thread_local EZString buffer;
    thread_local CURL *conn = nullptr;
    thread_local char errorBuffer[CURL_ERROR_SIZE];
    thread_local EZHTTPResponse response;

    curl_global_init(CURL_GLOBAL_DEFAULT);

    conn = curl_easy_init();
    curl_easy_reset(conn);

    if(conn == nullptr) {
        response.errorMessage("Failed to create CURL connection");
        response.isSuccess(false);
        return response;
    }

    if (URL.beginsWith("https://")) {
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

    struct curl_slist *chunk = nullptr;
    chunk = curl_slist_append(chunk, "Accept: application/geo+json;version=1");
    response.code(curl_easy_setopt(conn, CURLOPT_HTTPHEADER, chunk));
    if (response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
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

    auto start = high_resolution_clock::now();
    response.code(curl_easy_perform(conn));
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    response.responseTimeInMillis(duration.count());

    curl_easy_cleanup(conn);
    if(response.code() != CURLE_OK) {
        response.errorMessage(curlError(response.code()));
        response.isSuccess(false);
        return response;
    }
    response.data.value(buffer);
    buffer = "";
    response.isSuccess(true);
    return response;
}

void EZHTTP::setHasRealCert(bool badWebsite) {
    _realCert = badWebsite;
}
