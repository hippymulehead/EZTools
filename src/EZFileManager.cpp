//
// Created by mromans on 6/24/19.
//

#include "EZFileManager.h"

EZURI::EZURI(EZString uri) {
    _uri = uri;
    if (uri.regexMatch("^/")) {
        EZFileStat stat(uri);
        _isThere = stat.isThere();
        _isReadable = stat.isReadable();
        _isWriteable = stat.isWriteable();
        _locType = 1;
    } else if (uri.regexMatch("^http")) {
        EZHTTP http;
        http.setUserAgent("EZFileManager v0.0.1 email:mromans@romansaudio.com");
        _isThere = http.there(uri);
        _isReadable = _isThere;
        _isWriteable = false;
        _locType = 0;
    }
}

EZError EZFileManager::webCopy(EZURI websource, Json::Value &jsonRoot) {
    _resp = EZHTTPResponse();
    EZString data;
    if (websource.locationType() == 0) {
        if (!websource.isThere() || !websource.isReadable()) {
            EZError retType("Source is not readable");
            return retType;
        }
        EZHTTP http("EZFileManager v0.0.1 email:mromans@romansaudio.com");
        _resp = http.get(websource.uri());
        if (_resp.isSuccess()) {
            jsonRoot = _resp.data.asJSONCPP();
        } else {
            EZError retType("JSON Conversion failed");
            return retType;
        }
        return EZError();
    } else {
        EZError retType("Source is not a web based file");
        return retType;
    }
}

EZError EZFileManager::webCopy(EZURI websource, EZURI filedest, bool overwrite) {
    _resp = EZHTTPResponse();
    EZString data;
    if (websource.locationType() == 0) {
        if (!websource.isThere() || !websource.isReadable()) {
            EZError ret(websource.uri()+" not found");
            return ret;
        }
        if (!overwrite && filedest.isThere()) {
            EZError ret("Can't overwrite "+filedest.uri());
            return ret;
        }
        if (overwrite && !filedest.isWriteable()) {
            EZError ret(filedest.uri()+" not writeable");
            return ret;
        }
        EZHTTP http("EZFileManager v0.0.1 email:mromans@romansaudio.com");
        _resp = http.get(websource.uri());
        if (_resp.isSuccess()) {
            data = _resp.data.value();
        } else {
            EZError ret("Could not get "+websource.uri()+ " error: "+_resp.errorMessage());
            return ret;
        }
        ofstream outfile(filedest.uri());
        outfile << data;
        outfile.close();
        return EZError();
    } else {
        EZError ret(websource.uri()+" is not a web based file");
        return ret;
    }
}

EZError EZFileManager::fileCopy(EZURI filesource, EZURI filedest, bool overwrite) {
    EZString data;
    if (filesource.locationType() == 1) {
        if (!filesource.isThere() || !filesource.isReadable()) {
            EZError ret(filesource.uri()+" not found or not readable");
            return ret;
        }
        if (!overwrite && filedest.isThere()) {
            EZError ret(filedest.uri()+" is there but not allowed to overwrite");
            return ret;
        }
        if (overwrite && !filedest.isWriteable()) {
            EZError ret(filedest.uri()+" is not writeable");
            return ret;
        }

        ofstream outfile(filedest.uri());
        outfile << data;
        outfile.close();
        return EZError();
    } else {
        EZError ret(filesource.uri()+" is not a filesystem based file");
        return ret;
    }
}

EZError EZFileManager::fileMove(EZURI filesource, EZURI filedest, bool overwrite) {
    if (!filesource.isWriteable()) {
        EZError ret(filesource.uri()+" is not deletable");
        return ret;
    }
    EZError ret = fileCopy(filesource, filedest, overwrite);
    if (ret.isSuccess()) {
        EZError dret = deleteFile(filesource.uri());
        if (!dret.isSuccess()) {
            EZError rett(filedest.uri()+" was overwritten but "+filesource.uri()+" failed to delete");
            return rett;
        }
        return EZError();
    } else {
        return ret;
    }
}
