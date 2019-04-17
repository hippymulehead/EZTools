//
// Created by mromans on 4/16/19.
//

#ifndef EZTOOLS_EZSTATUS_H
#define EZTOOLS_EZSTATUS_H

#include "EZString.h"

enum EZSTATUSTYPE {
    SUCCESS = 0,
    DEST_NOT_WRITEABLE,
    SOURCE_NOT_READABLE,
    WOULD_OVERWRITE_BUT_NOT_ALLOWED,
    COULD_NOT_REMOVE_FILE,
    FILE_NOT_LOCAL,
    FILE_NOT_FOUND,
    NOT_A_DIRECTORY,
    USERNAME_ERROR,
    GROUP_ERROR,
    CHOWN_ERROR,
    PASSTHROUGH,
    CURRENTLY_NOT_IMPLEMENTED,
    UNKNOWN
};

class EZSTATUS {
public:
    EZSTATUS() = default;
    virtual ~EZSTATUS() = default;
    void type(EZSTATUSTYPE t) { _type = t; }
    EZSTATUSTYPE type() { return _type; }
    EZString emessage() { return _emessage; }
    void emessage(EZString em) { _emessage = em; }

    bool success() {
        return _type == SUCCESS;
    }
    EZString message() {
        switch (_type) {
            case SUCCESS:
                return "";
            case DEST_NOT_WRITEABLE:
                return "The desination is not writable";
            case SOURCE_NOT_READABLE:
                return "The source is not readable";
            case WOULD_OVERWRITE_BUT_NOT_ALLOWED:
                return "Would overwrite the destination but that is not allowed";
            case CURRENTLY_NOT_IMPLEMENTED:
                return "This is currently not implemented";
            case COULD_NOT_REMOVE_FILE:
                return "Could not remove file";
            case FILE_NOT_LOCAL:
                return "File not local";
            case FILE_NOT_FOUND:
                return "File not found";
            case NOT_A_DIRECTORY:
                return "Not a directory";
            case USERNAME_ERROR:
                return "Username error";
            case GROUP_ERROR:
                return "Group error";
            case CHOWN_ERROR:
                return "There was a problem in chown";
            case PASSTHROUGH:
                return _emessage;
            default:
                return "Error 42 - Unknown";
        }
    }

private:
    EZSTATUSTYPE _type;
    EZString _emessage;
};

#endif //EZTOOLS_EZSTATUS_H
