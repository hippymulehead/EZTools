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

#ifndef CLUSTERCHECK_EZMYSQL_H
#define CLUSTERCHECK_EZMYSQL_H

#pragma once

#define MYSQLPP_MYSQL_HEADERS_BURIED
#include <mysql++/mysql++.h>
#include "EZTools.h"
#include "EZScreen.h"
#include <EZDateTime.h>

namespace EZMySQL {

    class DBStatusMap;

    enum db_types {
        BIGINT, INT, FLOAT, DOUBLE, VARCHAR, DATETIME, UNKNOWN
    };

    inline EZTools::EZString dbTypesAsString(db_types t) {
        switch (t) {
            case BIGINT:
                return "BIGINT";
            case INT:
                return "INT";
            case FLOAT:
                return "FLOAT";
            case DOUBLE:
                return "DOUBLE";
            case VARCHAR:
                return "VARCHAR";
            case DATETIME:
                return "DATETIME";
            default:
                return "UNKNOWN";
        }
    }

    enum MySQLTimeVaules_t {
        SECONDS,
        MINUTE,
        HOUR,
        DAY
    };

    EZTools::EZString mySQLTimeVaulesAsString(MySQLTimeVaules_t t) {
        switch(t) {
            case SECONDS:
                return "Seconds";
            case MINUTE:
                return "Minute";
            case HOUR:
                return "Hour";
            case DAY:
                return "Day";
        }
        return "Unknown";
    }

    class MySQLConnetionInfo {
    public:
        MySQLConnetionInfo() = default;
        MySQLConnetionInfo(EZTools::EZString server, EZTools::EZString database, EZTools::EZString user,
                           EZTools::EZString passowrd, unsigned int port = 3306) {
            _user = user;
            _password = passowrd;
            _database = database;
            _server = server;
            _port = port;
        }
        ~MySQLConnetionInfo() = default;
        void init(EZTools::EZString server, EZTools::EZString database, EZTools::EZString user,
                  EZTools::EZString passowrd, unsigned int port = 3306) {
            _user = user;
            _password = passowrd;
            _database = database;
            _server = server;
            _port = port;
        }
        const char * user() { return _user.c_str(); }
        const char * password() { return _password.c_str(); }
        const char * server() { return _server.c_str(); }
        const char * database() { return _database.c_str(); }
        unsigned int port() const { return _port; }
    private:
        EZTools::EZString _user;
        EZTools::EZString _password;
        EZTools::EZString _server;
        EZTools::EZString _database;
        unsigned int _port = 3306;
    };

    inline db_types stringToDBType(EZTools::EZString t) {
        if (t == "int") {
            return INT;
        } else if (t == "varchar") {
            return VARCHAR;
        } else if (t == "datetime") {
            return DATETIME;
        } else {
            return UNKNOWN;
        }
    }

    inline EZTools::EZString DBTypeToString(db_types t) {
        switch (t) {
            case INT:
                return "int";
            case VARCHAR:
                return "varchar";
            case DATETIME:
                return "datetime";
            default:
                return "unknown";
        }
    }

    struct SecondaryKey_t {
        EZTools::EZString name;
        std::vector<EZTools::EZString> columns;
    };

    class SecondaryKeys {
    public:
        SecondaryKeys() = default;
        ~SecondaryKeys() = default;
        void addKey(EZTools::EZString key) {
            SecondaryKey_t k;
            k.name = key;
            _keys.emplace_back(k);
        }
        void addKeyValue(EZTools::EZString key, EZTools::EZString value) {
            bool worked = false;
            for (auto& k : _keys) {
                if (k.name == key) {
                    k.columns.emplace_back(value);
                    worked = true;
                }
            }
            if (!worked) {
                std::cout << "Adding value to key " << key << " failed as key not found" << endl;
                exit(666);
            }
        }
        bool hasKey(std::string key) {
            for (auto& k : _keys) {
                if (k.name == key) {
                    return true;
                }
            }
            return false;
        }
        std::vector<EZTools::EZString> getKeyList() {
            std::vector<EZTools::EZString> keys;
            for (auto& k : _keys) {
                keys.emplace_back(k.name);
            }
            return keys;
        }
        EZTools::EZString getKeyValueList(EZTools::EZString key) {
            EZTools::EZString list;
            for (auto& k : _keys) {
                if (k.name == key) {
                    std::stringstream kl;
                    for (auto& v : k.columns) {
                        kl << "`" << v << "`,";
                    }
                    list = kl.str();
                    list.regexReplace(",$", "");
                    return list;
                }
            }
            return list;
        }
    private:
        std::vector<SecondaryKey_t> _keys;
    };

    struct columnData_t {
        EZTools::EZString colTitle;
        std::vector<EZTools::EZString> colData;
    };

    struct wrapData_t {
        EZTools::EZString str;
        unsigned count = 0;
    };

    struct bar_t {
        EZScreen::RGB foreground = EZScreen::WHITE;
        EZScreen::RGB background = EZScreen::BLACK;
    };

    struct tableColrs_t {
        EZScreen::RGB box = EZScreen::BRIGHT_CYAN;
        EZScreen::RGB title = EZScreen::BRIGHT_CYAN;
        std::vector<bar_t> bar;
    };

    inline tableColrs_t cyanTableColors() {
        std::vector<EZMySQL::bar_t> m;
        EZMySQL::bar_t n;
        n.foreground = EZScreen::BLACK;
        n.background = EZScreen::CYAN;
        m.emplace_back(n);
        EZMySQL::bar_t nn;
        nn.foreground = EZScreen::BLACK;
        nn.background = EZScreen::WHITE;
        m.emplace_back(nn);
        tableColrs_t t;
        t.bar = m;
        t.box = EZScreen::BRIGHT_CYAN;
        t.title = EZScreen::BRIGHT_CYAN;
        return t;
    }

    inline tableColrs_t greenTableColors() {
        std::vector<EZMySQL::bar_t> m;
        EZMySQL::bar_t n;
        n.foreground = EZScreen::BLACK;
        n.background = EZScreen::GREEN;
        m.emplace_back(n);
        EZMySQL::bar_t nn;
        nn.foreground = EZScreen::BLACK;
        nn.background = EZScreen::WHITE;
        m.emplace_back(nn);
        tableColrs_t t;
        t.bar = m;
        t.box = EZScreen::BRIGHT_GREEN;
        t.title = EZScreen::BRIGHT_GREEN;
        return t;
    }

    inline tableColrs_t blackAndWhiteTableColors() {
        std::vector<EZMySQL::bar_t> m;
        EZMySQL::bar_t n;
        n.foreground = EZScreen::WHITE;
        n.background = EZScreen::BLACK;
        m.emplace_back(n);
        EZMySQL::bar_t nn;
        nn.foreground = EZScreen::BLACK;
        nn.background = EZScreen::WHITE;
        m.emplace_back(nn);
        tableColrs_t t;
        t.bar = m;
        t.box = EZScreen::BRIGHT_WHITE;
        t.title = EZScreen::BRIGHT_WHITE;
        return t;
    }

    inline tableColrs_t mixedTableColors() {
        std::vector<EZMySQL::bar_t> m;
        EZMySQL::bar_t n;
        n.foreground = EZScreen::BLACK;
        n.background = EZScreen::GREEN;
        m.emplace_back(n);
        EZMySQL::bar_t nn;
        nn.foreground = EZScreen::BLACK;
        nn.background = EZScreen::WHITE;
        m.emplace_back(nn);
        tableColrs_t t;
        t.bar = m;
        t.box = EZScreen::BRIGHT_WHITE;
        t.title = EZScreen::BRIGHT_YELLOW;
        return t;
    }

    struct type_t {
        db_types type = UNKNOWN;
        int length = 0;
    };

    struct table_t {
        EZTools::EZString field;
        type_t type;
        bool nullValue = false;
        EZTools::EZString fieldDefault;
        EZTools::EZString extra;
        EZTools::EZString key;
    };

    struct dataReturn_t {
        mysqlpp::StoreQueryResult result;
        std::vector<EZTools::EZString> fields;
    };

    struct wrapDataType_t {
        std::vector<EZTools::EZString> fields;
        std::vector<std::vector<EZTools::EZString>> data;
        std::vector<unsigned long> maxLength;
        unsigned rows;
    };

    inline wrapDataType_t createWrapData(mysqlpp::StoreQueryResult& newData, std::vector<EZTools::EZString> fields) {
        wrapDataType_t res;
        res.fields = fields;
        for (size_t i = 0; i < newData.num_rows(); i++) {
            std::vector<EZTools::EZString> d;
            for (auto& f : res.fields) {
                d.emplace_back(newData[i][f.c_str()].data());
            }
            res.data.emplace_back(d);
        }
        for (size_t x = 0; x < res.fields.size(); x++) {
            res.maxLength.emplace_back(0);
        }
        for (size_t i = 0; i < newData.num_rows(); i++) {
            int pos = 0;
            int fo = 0;
            for (auto& f : res.fields) {
                EZTools::EZString st = newData[i][f.c_str()].data();
                auto si = st.length();
                if (si < f.length()) {
                    si = f.length();
                }
                si = si + 2;
                if (res.maxLength.at(pos) < si) {
                    res.maxLength.at(pos) = si;
                }
                pos++;
                fo++;
            }
        }
        res.rows = newData.num_rows();
        return res;
    }

    inline wrapData_t wrapTable(wrapDataType_t& data, tableColrs_t colors = cyanTableColors()) {
        wrapData_t res;
        std::stringstream ss;
        int currentLoc = 0;
        // Top Line
        for (auto& column : data.fields) {
//            ss << EZScreen::background(EZScreen::BLACK);
            if (currentLoc == 0) {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_UPPER_LEFT);
            }
            for (unsigned y = 0; y < data.maxLength.at(currentLoc) + 2; y++) {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_HORIZONTAL_LINE);
            }
            if (currentLoc < data.maxLength.size() - 1) {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_TOP_T);
            } else {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_UPPER_RIGHT) << CON_RESET << std::endl;
            }
            currentLoc++;
        }
        // Titles
        for (size_t i = 0; i < data.fields.size(); i++) {
            ss << EZScreen::foreground(colors.box);
            ss << EZScreen::putChar(EZScreen::SC_VERICAL_LINE);
            ss << " ";
            ss << EZScreen::foreground(colors.title);
            ss << data.fields.at(i);
            for (int q = 1; q < data.maxLength.at(i) - data.fields.at(i).length() + 1; q++) {
                ss << " ";
            }
            ss << " ";
        }
        ss << EZScreen::foreground(colors.box);
        ss << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << CON_RESET << std::endl;
        // Lower Line
        for (size_t i = 0; i < data.fields.size(); i++) {
            if (i == 0) {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_LEFT_T);
            } else {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_CROSS);
            }
            ss << EZScreen::foreground(colors.box);
            ss << EZScreen::putChar(EZScreen::SC_HORIZONTAL_LINE);
            for (int q = 1; q < data.maxLength.at(i) + 2; q++) {
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::putChar(EZScreen::SC_HORIZONTAL_LINE);
            }
        }
        ss << EZScreen::putChar(EZScreen::SC_RIGHT_T) << CON_RESET << std::endl;
        // Data
        int c = 0;
        for (size_t i = 0; i < data.data.size(); i++) {
            ss << EZScreen::foreground(colors.box);
            ss << EZScreen::putChar(EZScreen::SC_VERICAL_LINE);
            int b = c % 2;
            for (size_t f = 0; f < data.fields.size(); f++) {
                ss << EZScreen::foreground(colors.bar[b].foreground);
                ss << EZScreen::background(colors.bar[b].background);
                ss << " ";
                ss << data.data.at(i).at(f);
                for (int q = 1; q < data.maxLength.at(f) - (data.data.at(i).at(f).length() - 1); q++) {
                    ss << " ";
                }
                ss << " ";
                ss << EZScreen::foreground(colors.box);
                ss << EZScreen::background(EZScreen::BLACK);
                ss << EZScreen::putChar(EZScreen::SC_VERICAL_LINE);
            }
            ss << CON_RESET << std::endl;
            c++;
        }
        // Bottom Line
        currentLoc = 0;
        for (auto& column : data.fields) {
//            ss << EZScreen::background(EZScreen::BLACK);
            ss << EZScreen::foreground(colors.box);
            if (currentLoc == 0) {
                ss << EZScreen::putChar(EZScreen::SC_LOWER_LEFT);
            }
            for (unsigned y = 0; y < data.maxLength.at(currentLoc) + 2; y++) {
                ss << EZScreen::putChar(EZScreen::SC_HORIZONTAL_LINE);
            }
            if (currentLoc < data.maxLength.size() - 1) {
                ss << EZScreen::putChar(EZScreen::SC_BOTTOM_T);
            } else {
                ss << EZScreen::putChar(EZScreen::SC_LOWER_RIGHT);
            }
            currentLoc++;
        }
        ss << CON_RESET;
        res.str = ss.str();
        res.count = data.rows;
        return res;
    }

    inline EZTools::EZString typeAsString(EZTools::EZString type) {
        if (type == "i") {
            return "int";
        }
        if (type == "l") {
            return "long int";
        }
        if (type == "f") {
            return "float";
        }
        if (type == "d") {
            return "double";
        }
        return "unknown";
    }

    inline nlohmann::json storeQueryResultAsJSON(mysqlpp::StoreQueryResult &ezreturn) {
        nlohmann::json root;
        if (ezreturn.num_rows() == 0) {
            return root;
        }
        auto fields = ezreturn[0].field_list().list->size();
        std::vector<EZTools::EZString> fieldList;
        for (size_t i = 0; i < fields; i++) {
            fieldList.emplace_back(ezreturn[0].field_list().list->at(i));
        }
        int counter = 0;
        for (auto& l : ezreturn) {
            nlohmann::json t;
            t["EZRESULT_ID"] = counter;
            for (auto& name : fieldList) {
                EZTools::EZString ty = l[name.str().c_str()].type().name();
                ty.regexReplace("N7mysqlpp4NullI|NS_10NullIsNullEEE","");
                if (ty.regexMatch("^d$") || ty.regexMatch("^i$") || ty.regexMatch("^l$") || ty.regexMatch("^f$")) {
                    t[name.str()]["type"] = typeAsString(ty);
                    EZTools::EZString v = l[name.str().c_str()].data();
                    if ((ty == "i") || (ty == "l")) {
                        t[name.str()]["value"] = v.asInt();
                    } else if (ty == "f") {
                        t[name.str()]["value"] = v.asFloat();
                    } else if (ty == "d") {
                        t[name.str()]["value"] = v.asDouble();
                    }
//                    t[name.str()]["value"] = v.asDouble();
                } else {
                    t[name.str()] = l[name.str().c_str()];
                }
            }
            root["EZRESULTS"].emplace_back(t);
            counter++;
        }
        return root;
    }

    class EZReturnStoreQueryResultAsJSON {
    public:
        EZReturnStoreQueryResultAsJSON() = default;
        EZReturnStoreQueryResultAsJSON(int exitCode, EZTools::EZString message) {
            _exitCode = exitCode;
            _message = message;
        }
        ~EZReturnStoreQueryResultAsJSON() = default;
        bool wasSuccessful() const { return _wasSuccessful; }
        void wasSuccessful(bool success) { _wasSuccessful = success; }
        int exitCode() const { return _exitCode; }
        void exitCode(int value) {_exitCode = value; }
        EZTools::EZString message() const { return _message; }
        void message(EZTools::EZString value) { _message = value; }
        void location(EZTools::EZString Location) { _location = Location; }
        EZTools::EZString location() const { return _location; }
        explicit operator bool() const {
            return _wasSuccessful;
        }
        void last(EZTools::EZString LastSQL) {
            _last = LastSQL;
        }
        nlohmann::json data;
        void setData(nlohmann::json Data, unsigned int counter) {
            if (!Data.empty()) {
                _data = Data;
            }
            _data["EZRESULT_METADATA"]["EZTools"] = VERSION;
            _data["EZRESULT_METADATA"]["resultCount"] = counter;
            _data["EZRESULT_METADATA"]["wasSuccessful"] = _wasSuccessful;
            _data["EZRESULT_METADATA"]["message"] = _message;
            _data["EZRESULT_METADATA"]["sql"] = _last;
            data = _data;
        }
        EZTools::EZReturnMetaData metaData() {
            EZTools::EZReturnMetaData md;
            md.wasSuccessful = _wasSuccessful;
            if (!_message.empty()) {
                md.message = _message;
            }
            return md;
        }
    private:
        nlohmann::json _data;
        int _exitCode;
        EZTools::EZString _message;
        EZTools::EZString _location;
        bool _wasSuccessful = false;
        EZTools::EZString _last;
    };

    class Database {
    public:
        // Create a new database object
        Database() = default;

        // Disconnect & destroy the database object
        ~Database() {
            if (_conn.connected()) {
                _conn.disconnect();
            }
        }

        // Connect to a database
        EZTools::EZReturn<bool> connect(EZTools::EZString server, EZTools::EZString database, EZTools::EZString user,
                                        EZTools::EZString passowrd, unsigned int port = 3306) {
            EZTools::EZReturn<bool> res;
            _user = user;
            _password = passowrd;
            _database = database;
            _server = server;
            _port = port;
            mysqlpp::Connection conn(false);
            conn.connect(_database.c_str(), _server.c_str(), _user.c_str(), _password.c_str(),
                         _port);
            _conn = conn;
            res.wasSuccessful(_conn.connected());
            if (!res.wasSuccessful()) {
                res.message(_conn.error());
            }
            return res;
        }

        // Disconnect from a database
        void disconnect() {
            _conn.disconnect();
            _database = "";
            _last = "";
        }

        // Show wheather you're connected to a database or not
        bool connected() {
            return _conn.connected();
        }

        // Shows the currently connected database name
        EZTools::EZString currentDatabase() { return _database; }

        // Run a standard query on the database (can do any query)
        EZTools::EZReturn<mysqlpp::StoreQueryResult> query(EZTools::EZString Query) {
            EZTools::EZReturn<mysqlpp::StoreQueryResult> ret;
            _last = Query;
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            mysqlpp::Query query = _conn.query(Query);
            mysqlpp::StoreQueryResult res = query.store();
            ret.data = res;
            ret.wasSuccessful(strlen(query.error()) == 0);
            ret.message(query.error());
            return ret;
        }

        EZReturnStoreQueryResultAsJSON queryAsJSON(EZTools::EZString Query) {
            EZReturnStoreQueryResultAsJSON ret;
            _last = Query;
            ret.last(_last);
            mysqlpp::Query query = _conn.query(Query);
            mysqlpp::StoreQueryResult res = query.store();
            if (res.num_rows() > 0) {
                ret.wasSuccessful(true);
            } else {
                ret.message("No data returned");
            }
            ret.setData(storeQueryResultAsJSON(res),res.num_rows());
            ret.wasSuccessful(strlen(query.error()) == 0);
            ret.message(query.error());
            return ret;
        }

        // Run a select
        EZTools::EZReturn<mysqlpp::StoreQueryResult> select(EZTools::EZString table, EZTools::EZString what,
                                                            EZTools::EZString where = "",
                                                            EZTools::EZString groupBy = "",
                                                            EZTools::EZString orderBy = "",
                                                            int limit = 0) {
            std::stringstream ss;
            ss << "SELECT " << what << " FROM " << table;
            if (!where.empty()) {
                ss << " where " << where;
            }
            if (!groupBy.empty()) {
                ss << " group by " << groupBy;
            }
            if (!orderBy.empty()) {
                ss << " order by " << orderBy;
            }
            if (limit > 0) {
                ss << " limit " << limit;
            }
            ss << ";";
            _last = ss.str();
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            r.data = res;
            r.wasSuccessful(strlen(query.error()) == 0);
            r.message(query.error());
            return r;
        }

        EZReturnStoreQueryResultAsJSON selectAsJSON(EZTools::EZString table, EZTools::EZString what,
                                                            EZTools::EZString where = "",
                                                            EZTools::EZString groupBy = "",
                                                            EZTools::EZString orderBy = "",
                                                            int limit = 0) {
            std::stringstream ss;
            ss << "SELECT " << what << " FROM " << table;
            if (!where.empty()) {
                ss << " where " << where;
            }
            if (!groupBy.empty()) {
                ss << " group by " << groupBy;
            }
            if (!orderBy.empty()) {
                ss << " order by " << orderBy;
            }
            if (limit > 0) {
                ss << " limit " << limit;
            }
            ss << ";";
            _last = ss.str();
            EZReturnStoreQueryResultAsJSON ret;
            ret.last(_last);
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            if (res.num_rows() > 0) {
                ret.wasSuccessful(true);
            } else {
                ret.message("No data returned");
            }
            ret.setData(storeQueryResultAsJSON(res),res.num_rows());
            ret.wasSuccessful(strlen(query.error()) == 0);
            ret.message(query.error());
            return ret;
        }

        // Run an insert
        EZTools::EZReturn<mysqlpp::StoreQueryResult> insert(EZTools::EZString table, EZTools::EZString fields,
                                                            EZTools::EZString values) {

            std::stringstream ss;
            ss << "INSERT INTO " << table << "(" << fields << ") values(" << values << ");";
            _last = ss.str();
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            r.data = res;
            EZTools::EZString derp = query.error();
            if (derp.regexMatch("^Query was empty")) {
                derp = "";
            }
            r.wasSuccessful(derp.empty());
            r.message(derp);
            return r;
        }

        // Run a delete where
        EZTools::EZReturn<mysqlpp::StoreQueryResult> Delete(EZTools::EZString table, EZTools::EZString where) {
            std::stringstream ss;
            ss << "delete from " << table << " where " << where;
            _last = ss.str();
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            r.data = res;
            EZTools::EZString derp = query.error();
            if (derp.regexMatch("^Query was empty")) {
                derp = "";
            }
            r.wasSuccessful(derp.empty());
            r.message(derp);
            return r;
        }

        // Run an insert or update on a duplicate key being found
        EZTools::EZReturn<mysqlpp::StoreQueryResult> insertOnDuplicateKeyUpdate(EZTools::EZString table,
                                                                                EZTools::EZString fields,
                                                                                EZTools::EZString values,
                                                                                EZTools::EZString update) {
            std::stringstream ss;
            ss << "INSERT INTO " << table << "(" << fields << ") values(" << values << ") ON DUPLICATE KEY UPDATE "
               << update << ";";
            _last = ss.str();
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            query.execute();
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            r.data = res;
            EZTools::EZString derp = query.error();
            if (derp.regexMatch("^Query was empty")) {
                derp = "";
            }
            r.wasSuccessful(derp.empty());
            r.message(derp);
            return r;
        }

        // UPdate a row
        EZTools::EZReturn<mysqlpp::StoreQueryResult> update(EZTools::EZString table, EZTools::EZString set,
                                                            EZTools::EZString where = "") {
            std::stringstream ss;
            ss << "UPDATE " << table << " SET " << set;
            if (!where.empty()) {
                ss << " WHERE " << where;
            }
            ss << ";";
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            query.execute();
            EZTools::EZReturn<mysqlpp::StoreQueryResult> r;
            r.data = res;
            EZTools::EZString derp = query.error();
            if (derp.regexMatch("^Query was empty")) {
                derp = "";
            }
            r.wasSuccessful(derp.empty());
            r.message(derp);
            return r;
        }

        // Show all tables for the database
        EZTools::EZReturn<std::vector<EZTools::EZString>> showTables() {
            EZTools::EZReturn<std::vector<EZTools::EZString>> ret;
            _last = "show tables;";
            mysqlpp::Query query = _conn.query("show tables");
            if (strlen(query.error()) == 0) {
                ret.wasSuccessful(true);
                mysqlpp::StoreQueryResult res = query.store();
                mysqlpp::StoreQueryResult::iterator rit;
                for (rit = res.begin(); rit != res.end(); ++rit) {
                    std::string tbl((*rit)[0]);
                    ret.data.emplace_back(tbl);
                }
                return ret;
            } else {
                ret.wasSuccessful(false);
                ret.message(query.error());
                return ret;
            }
        }

        EZTools::EZReturn<std::vector<EZTools::EZString>> showDatabases() {
            EZTools::EZReturn<std::vector<EZTools::EZString>> ret;
            _last = "show databases;";
            mysqlpp::Query query = _conn.query("show databases");
            if (strlen(query.error()) == 0) {
                ret.wasSuccessful(true);
                mysqlpp::StoreQueryResult res = query.store();
                mysqlpp::StoreQueryResult::iterator rit;
                for (rit = res.begin(); rit != res.end(); ++rit) {
                    std::string tbl((*rit)[0]);
                    ret.data.emplace_back(tbl);
                }
                return ret;
            } else {
                ret.wasSuccessful(false);
                ret.message(query.error());
                return ret;
            }
        }

        struct queryStore_t {
            mysqlpp::StoreQueryResult store;
            EZTools::EZString error;
        };

        queryStore_t getIndexes(EZTools::EZString &table) {
            queryStore_t q;
            stringstream ss;
            ss << "SHOW INDEXES FROM " << table << ";";
            mysqlpp::Query query = _conn.query(ss.str());
            q.error = query.error();
            q.store = query.store();
            return q;
        }

        EZTools::EZString getVersion() {
            mysqlpp::Query query = _conn.query("select @@version;");
            mysqlpp::StoreQueryResult store = query.store();
            EZTools::EZString ver = store[0]["@@version"].data();
            return ver;
        }

        EZTools::EZString SQLHeader(EZTools::EZString host, EZTools::EZString database) {
            std::stringstream ss;
            ss << "-- EZMySQL::Dump: " << VERSION << endl;
            ss << "-- Host: " << host << "\tDatabase: " << database << endl;
            ss << "-- ------------------------------------------------" << endl;
            ss << "-- Server Version: " << getVersion() << endl;
            return ss.str();
        }

        EZTools::EZString SQLFooter() {
            std::stringstream ss;
            EZDateTime::DateTime dt;
            ss << "-- EZMySQL::Dump: Completed on " << dt.ymdt() << endl;
            return ss.str();
        }

        EZTools::EZString getEngine(EZTools::EZString table, EZTools::EZString database) {
            EZTools::EZString engine;
            std::stringstream ss;
            ss << "SELECT TABLE_NAME, ENGINE FROM information_schema.TABLES WHERE  TABLE_SCHEMA = '" << database << "';";
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult store = query.store();
            for (auto& t : store) {
                EZTools::EZString tab = t["TABLE_NAME"].data();
                if (tab == table) {
                    return t["ENGINE"].data();
                }
            }
            return "";
        }

        EZTools::EZString getAutoIncrement(EZTools::EZString table) {
            std::stringstream ss;
            ss << "select AUTO_INCREMENT from INFORMATION_SCHEMA.TABLES WHERE TABLE_SCHEMA = '"
                << _database << "' and TABLE_NAME = '" << table << "';";
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult store = query.store();
            std::stringstream ai;
            ai << "AUTO_INCREMENT=" << store[0]["AUTO_INCREMENT"].data();
            return ai.str();
        }

        EZTools::EZString getCharSet(EZTools::EZString table) {
            EZTools::EZString cs;
            std::stringstream ss;
            ss << "SHOW CREATE TABLE " << table << ";";
            mysqlpp::Query query = _conn.query(ss.str());
            EZTools::EZString q = query.str();
            cout << q << endl;
            exit(999);
            mysqlpp::StoreQueryResult store = query.store();

            return cs;
        }

        EZTools::EZReturn<EZTools::EZString> describeTableSQL(EZTools::EZString table, EZTools::EZString database) {
            EZTools::EZReturn<EZTools::EZString> r;
            std::stringstream ss;
            ss << "describe " << table << ";";
            _last = ss.str();
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult store = query.store();
            if (strlen(query.error()) != 0) {
                r.wasSuccessful(false);
                r.message(query.error());
                return r;
            }
            stringstream sql;
            bool autoInc = false;
            sql << "--" << endl;
            sql << "-- Table structure for table `" << table << "`" << endl;
            sql << "--" << endl;
            sql << "DROP TABLE IF EXISTS `" << table << "`;" << endl;
            sql << "CREATE TABLE `" << table << "` (" << endl;
            for (auto& m : store) {
                EZTools::EZString Field = m["Field"].data();
                EZTools::EZString Type = m["Type"].data();
                EZTools::EZString tmp = m["Null"].data();
                EZTools::EZString isNullable;
                isNullable = tmp == "YES" ? "DEFAULT NULL" : "NOT NULL";
                tmp = m["Default"].data();
                if (tmp == "NULL") {
                    tmp = "";
                }
                EZTools::EZString Default = tmp;
                EZTools::EZString Extra = m["Extra"].data();
                if (!autoInc) {
                    autoInc = Extra.regexMatch("auto_increment");
                }
                sql << "  `" << Field << "` " << Type << " " << isNullable.upper();
                if (!Default.empty()) {
                    sql << " DEFAULT " << Default;
                }
                if (!Extra.empty()) {
                    sql << " " << Extra;
                }
                sql << "," << endl;
            }
            auto index = getIndexes(table);
            if (!index.error.empty()) {
                r.message(index.error);
                return r;
            }
            EZTools::EZString PKEY;
            SecondaryKeys SKEYS;
            for (auto& pk : index.store) {
                EZTools::EZString k = pk["Key_name"].data();
                if (k == "PRIMARY") {
                    std::stringstream kk;
                    kk << "  PRIMARY KEY (`" << pk["Column_name"].data() << "`),";
                    PKEY = kk.str();
                } else {
                    if (SKEYS.hasKey(k)) {
                        SKEYS.addKeyValue(pk["Key_name"].data(),pk["Column_name"].data());
                    } else {
                        SKEYS.addKey(pk["Key_name"].data());
                        SKEYS.addKeyValue(pk["Key_name"].data(),pk["Column_name"].data());
                    }
                }
            }
            sql << PKEY << endl;
            auto kl = SKEYS.getKeyList();
            for (auto& k : kl) {
                sql << "  KEY `" << k << "` (" << SKEYS.getKeyValueList(k) << ")," << endl;
            }
            EZTools::EZString t = sql.str();
            auto sp = t.splitAtLast(",");
            std::stringstream se;
            se << sp.at(0) << endl;
            se << ") ";
            auto en = getEngine(table, database);
            se << "ENGINE=" << en;
            if (autoInc) {
                se << " " << getAutoIncrement(table);
            }
            auto cs = getCharSet(table);
            if (!cs.empty()) {
                se << " DEFAULT CHARSET=" << cs;
            }
            se << ";" << endl;
            r.wasSuccessful(true);
            r.data = se.str();
            return r;
        }

        // Describe a table
        EZTools::EZReturn<std::vector<table_t>> describeTable(EZTools::EZString table) {
            std::stringstream ss;
            ss << "DESCRIBE " << table << ";";
            _last = ss.str();
            EZTools::EZReturn<std::vector<table_t>> r;
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult res = query.store();
            if (strlen(query.error()) != 0) {
                r.wasSuccessful(false);
                r.message(query.error());
                return r;
            }
            for (auto& m : res) {
                table_t t;
                t.field = m["Field"].data();
                EZTools::EZString derp = m["Type"].data();
                derp.regexReplace("[(]|[)]", "-");
                auto s = derp.split("-");
                t.type.type = stringToDBType(s.at(0));
                if (s.size() > 1) {
                    t.type.length = s.at(1).asInt();
                }
                t.fieldDefault = m["Default"].data();
                t.extra = m["Extra"].data();
                t.key = m["Key"].data();
                r.data.emplace_back(t);
            }
            return r;
        }

        // Get the field list for a table
        EZTools::EZReturn<std::vector<EZTools::EZString>> fieldList(EZTools::EZString table) {
            EZTools::EZReturn<std::vector<EZTools::EZString>> res;
            std::stringstream ss;
            ss << "DESCRIBE " << table << ";";
            _last = ss.str();
            mysqlpp::Query query = _conn.query(ss.str());
            mysqlpp::StoreQueryResult store = query.store();
            if (strlen(query.error()) != 0) {
                res.wasSuccessful(false);
                res.message(query.error());
                return res;
            }
            for (auto& m : store) {
                res.data.emplace_back(m["Field"].data());
            }
            res.wasSuccessful(true);
            return res;
        }

        EZTools::EZReturnNoData isMaster() {
            EZTools::EZReturnNoData im;
            auto mc = query("SHOW SLAVE STATUS");
            if (!mc.data.empty()) {
                im.message("Not a master");
                im.wasSuccessful(false);
            } else {
                im.wasSuccessful(true);
            }
            return im;
        }
        EZTools::EZReturnNoData isSlave() {
            EZTools::EZReturnNoData im;
            auto mc = query("SHOW SLAVE STATUS");
            if (!mc.data.empty()) {
                im.wasSuccessful(true);
            } else {
                im.message("Not a slave");
                im.wasSuccessful(false);
            }
            return im;
        }
        // Show the last used command (clears with the database closing)
        EZTools::EZString lastCommand() { return _last; }
        void statusMapUpdate() {
            _statusTable.clear();
            auto st = query("SHOW STATUS");
            for (auto& l : st.data) {
                _statusTable[l.at(0).c_str()] = l.at(1).c_str();
            }
        }
        std::map<EZTools::EZString, EZTools::EZString> statusMap() {
            return _statusTable;
        }
        std::vector<EZTools::EZString> statusMapListKeys() {
            std::vector<EZTools::EZString> t;
            auto it = _statusTable.begin();
            while (it != _statusTable.end()) {
                t.push_back(it->first);
                ++it;
            }
            return t;
        }
        EZTools::EZString statusMapGetValue(EZTools::EZString key) {
            EZTools::EZString value;
            if (_statusTable.find(key) != _statusTable.end()) {
                value = _statusTable[key];
            }
            return value;
        }
        float statusMapUptime(MySQLTimeVaules_t t) {
            auto u = statusMapGetValue("Uptime");
            switch (t) {
                case SECONDS:
                    return u.asFloat();
                case MINUTE:
                    return u.asFloat() / 60;
                case HOUR:
                    return u.asFloat() / 60 / 60;
                case DAY:
                    return u.asFloat() / 60 / 60 / 24;
            }
            return 0.0;
        }
        float statusMapQueriesPer(MySQLTimeVaules_t t) {
            float rv = 0.0;
            auto q = statusMapGetValue("Queries");
            auto u = statusMapGetValue("Uptime");
            if (q.empty() || u.empty()) {
                return 0.0;
            }
            float h = 0.0;
            switch (t) {
                case SECONDS:
                    h = u.asFloat();
                    break;
                case MINUTE:
                    h = u.asFloat() / 60;
                    break;
                case HOUR:
                    h = u.asFloat() / 60 / 60;
                    break;
                case DAY:
                    h = u.asFloat() / 60 / 60 / 24;
                    break;
            }
            rv = q.asFloat() / h;
            return rv;
        }
        float statusMapMaxConnections() {
            return statusMapGetValue("Max_used_connections").asFloat();
        }
        float statusMapConnections() {
            return statusMapGetValue("Connections").asFloat();
        }
        float statusMapSlavesConnected() {
            return statusMapGetValue("Slaves_connected").asFloat();
        }
        bool statusMapSlaveRunning() {
            auto sr =  statusMapGetValue("Slave_running");
            return sr == "OFF" ? false : true;
        }
        EZTools::EZString statusMapReplicationStatus() {
            return statusMapGetValue("Rpl_status");
        }

    private:
        EZTools::EZString _last;
        mysqlpp::Connection _conn;
        EZTools::EZString _user;
        EZTools::EZString _password;
        EZTools::EZString _server;
        EZTools::EZString _database;
        std::vector<std::pair<EZTools::EZString, std::vector<EZTools::EZString>>> _tables;
        unsigned int _port = 3306;
        std::map<EZTools::EZString, EZTools::EZString> _statusTable;
    };

}

#endif //CLUSTERCHECK_EZMYSQL_H
