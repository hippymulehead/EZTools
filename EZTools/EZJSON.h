//
// Created by mromans on 2/28/20.
//

#ifndef EZT_EZJSON_H
#define EZT_EZJSON_H

#include "EZTools.h"
#include "EZCSV.h"
#include "json.h"

class EZJSON {
public:
    nlohmann::json root;
    EZJSON() = default;
    EZJSON(EZTools::EZString jsonString) {
        root = nlohmann::json::parse(jsonString);
    }
    EZJSON(EZCSV::CSVFile csvFile) {
        std::stringstream ss;
        ss << "[";
        for (auto line : csvFile.csv()) {
            ss << "{";
            for (unsigned long int i = 0; i < csvFile.columns(); i++) {
                if (line.at(i).isDouble()) {
                    ss << "\"" << csvFile.headers()[i] << "\": " << line[i];
                } else {
                    ss << "\"" << csvFile.headers()[i] << "\": \"" << line[i] << "\"";
                }
                if (i != csvFile.columns() - 1) {
                    ss << ",";
                }
            }
            ss << "},";
        }
        EZTools::EZString sss = ss.str();
        sss.regexReplace(",$", "");
        std::stringstream s;
        s << sss;
        s << "]";
        root = nlohmann::json::parse(s);
    }
    ~EZJSON() = default;
private:
    bool debug = true;
};

#endif //EZT_EZJSON_H
