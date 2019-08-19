//
// Created by mromans on 5/25/19.
//

#include "EZConfig.h"

EZConfig::EZConfig(EZString filename) {
    _filename = filename;
    EZFileStat stat(_filename);
    if (stat.isThere() && stat.isReadable()) {
        _there = stat.isThere();
        std::ifstream t(_filename);
        std::stringstream buffer;
        buffer << t.rdbuf();
        _file = buffer.str();
        t.close();
        stringstream ss;
        ss << _file;
        ss >> jsonRoot;
    }
}

void EZConfig::write() {
    Json::StreamWriterBuilder builder;
    std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
    std::ofstream outputFileStream(_filename);
    writer -> write(jsonRoot, &outputFileStream);
}