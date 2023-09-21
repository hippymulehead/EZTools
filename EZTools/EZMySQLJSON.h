/*
Copyright (c) 2017-2023, Michael Romans of Romans Audio
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
#ifndef EZTOOLS_EZMYSQLJSON_H
#define EZTOOLS_EZMYSQLJSON_H

#include "EZMySQL.h"
#include "nlohmann/json.hpp"

namespace EZMySQLJSON {

    enum STATUS {
        ERROR,
        CONNECTED
    };

    struct connectionObject_t {
        EZTools::EZString host;
        EZTools::EZString databaseName;
        EZTools::EZString userName;
        EZTools::EZString password;
        int port = 3306;
    };

    inline nlohmann::json connectionObject(EZTools::EZString Host, EZTools::EZString DatabaseName,
                                           EZTools::EZString UserName, EZTools::EZString Password, int Port = 3306) {
        nlohmann::json conObject;
        conObject["host"] = Host;
        conObject["databaseName"] = DatabaseName;
        conObject["userName"] = UserName;
        conObject["password"] = Password;
        conObject["port"] = Port;
        return conObject;
    }

    inline nlohmann::json statusMessage(STATUS status, EZTools::EZString message = "") {
        nlohmann::json root;
        switch (status) {
            case ERROR:
                root["status"] = "Error";
                break;
            case CONNECTED:
                root["status"] = "Connected";
                break;
        }
        if (!message.empty()) {
            root["message"] = message;
        }
        return root;
    }

    class JSONDatabaseInterface {
    public:
        JSONDatabaseInterface() = default;
        ~JSONDatabaseInterface() = default;
        nlohmann::json connect(nlohmann::json &connectObject) {
            if (connectObject["host"].is_string()) {
                _conOb.host = connectObject["host"].get<EZTools::EZString>();
            }
            if (connectObject["databaseName"].is_string()) {
                _conOb.databaseName = connectObject["databaseName"].get<EZTools::EZString>();
            }
            if (connectObject["userName"].is_string()) {
                _conOb.userName = connectObject["userName"].get<EZTools::EZString>();
            }
            if (connectObject["password"].is_string()) {
                _conOb.password = connectObject["password"].get<EZTools::EZString>();
            }
            if (connectObject["port"].is_number_integer()) {
                _conOb.port = connectObject["port"].get<int>();
            }
            if (_conOb.host.empty() || _conOb.userName.empty() || _conOb.password.empty()) {
                return statusMessage(ERROR, "Missing connection info");
            }
            _db.connect(_conOb.host,_conOb.databaseName, _conOb.userName, _conOb.password,_conOb.port);
            if (_db.connected()) {
                _connected = true;
                return statusMessage(CONNECTED);
            } else {
                return statusMessage(ERROR, "Connection Failed");
            }
        }
        void disconnect() {
            if (_connected) {
                _db.disconnect();
            }
        }
    private:
        EZMySQL::Database _db;
        connectionObject_t _conOb;
        bool _connected = false;
    };

}

#endif //EZTOOLS_EZMYSQLJSON_H
