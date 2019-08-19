//
// Created by mromans on 8/8/19.
//

#include "EZGetOpt.h"


void EZGetOpt::EZOptPairMap::insert(char key, EZString value) {
    _map.insert(pair<char, EZString>(key, value));
}

EZString EZGetOpt::EZOptPairMap::valueForKey(char key) {
    return _map[key];
}

EZGetOpt::GetOpt::GetOpt(int &argc, char* argv[], EZString programName, EZString version) {
    _programName = programName;
    _version = version;
    std::vector<EZString> arguments(argv + 1, argv + argc);
    _args = arguments;
}

void EZGetOpt::GetOpt::addCommandLineOption(EZString option, EZString description) {
    _options.insert(option, description);
}

void EZGetOpt::GetOpt::addCopyright(EZString copyright) {
    _copyright = copyright;
}

void EZGetOpt::GetOpt::addExtraMessage(EZString extraMessage) {
    _extraMessage = extraMessage;
}

void EZGetOpt::GetOpt::showHelp() {
    cout << endl << _programName << " " << _version << " " << _copyright << endl;
    if (!_extraMessage.empty()) {
        cout << _extraMessage << endl;
    }
    cout << "Help: " << _programName << " <";
    for (auto &option : _options.data) {
        cout << option.opt();
    }
    cout << "> ";
    if (_fileOption) {cout << "<Filename ...>"; }
    cout << endl;

    // Show Options
    for (auto &option : _options.data) {
        cout << "  " << option.opt() << "\t: " << option.description() << endl;
    }
    cout << endl;
}

void EZGetOpt::GetOpt::showVersion() {
    cout << endl << _programName << " " << _version << endl << endl;
}

void EZGetOpt::GetOptData::insert(EZString opt, EZString description) {
    GODat g;
    g.opt(opt);
    g.description(description);
    data.emplace_back(g);
}

EZGetOpt::EZOpts EZGetOpt::GetOpt::processedOptions() {
    if (_opts.empty()) {
        vector<EZString> newlist;
        for (auto &arg : _args) {
            if (arg.regexMatch("^-")) {
                EZString m = arg;
                m.regexReplace("^-", "");
                if (m.length() > 1) {
                    vector<char> as(arg.begin(), arg.end());
                    for (auto &a : as) {
                        stringstream s;
                        s << a;
                        newlist.emplace_back(s.str());
                    }
                } else {
                    newlist.emplace_back(arg);
                }
            } else {
                newlist.emplace_back(arg);
            }
        }
        for (auto i = 0; i < newlist.size(); i++) {
            bool added = false;
            for (unsigned long ii = 0; ii < _options.data.size(); ii++) {
                bool hasArg = false;
                EZString o = _options.data.at(ii).opt();
                if (o.regexMatch(":")) {
                    hasArg = true;
                    o.regexReplace(":", "");
                }
                EZString nodash = newlist.at(i);
                nodash.regexReplace("^-", "");
                if (nodash == o) {
                    added = true;
                    EZString V;
                    char O = nodash[0];
                    if (hasArg && ((i + 1) < newlist.size())) {
                        i++;
                        if (!newlist.at(i).regexMatch("^-")) {
                            V = newlist.at(i);
                        }
                    } else if (hasArg) {
                        showHelp();
                        exit(EXIT_FAILURE);
                    }
                    _opts.insert(pair<char, EZString>(O,V));
                }
            }
            if (!added) {
                showHelp();
                exit(EXIT_FAILURE);
            }
        }
    }
    return _opts;
}
