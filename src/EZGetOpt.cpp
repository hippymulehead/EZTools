//
// Created by mromans on 8/8/19.
//

#include "EZGetOpt.h"

EZGetOpt::GetOpt::GetOpt(int argc, char* argv[], EZString programName, EZString version) {
    _programName = programName;
    _version = version;
    std::vector<EZString> arguments(argv + 1, argv + argc);
    _args = arguments;
}

void EZGetOpt::GetOpt::addCommandLineOption(EZString option, EZString description) {
    _options.insert(pair<EZString, EZString>(option, description));
}

void EZGetOpt::GetOpt::addCopyright(EZString copyright) {
    _copyright = copyright;
}

void EZGetOpt::GetOpt::addExtraMessage(EZString extraMessage) {
    _extraMessage = extraMessage;
}

void EZGetOpt::GetOpt::showHelp() {
    cout << endl << _programName << " v" << _version << " " << _copyright << endl;
    if (!_extraMessage.empty()) {
        cout << _extraMessage << endl;
    }
    cout << "Help: " << _programName << " <";
    for (auto& op : _options) {
        cout << op.first;
    }
    cout << ">" << endl;
    for (auto& op : _options) {
        cout << "  " << op.first << " \t" << op.second << endl;
    }
    cout << endl;
}

void EZGetOpt::GetOpt::showVersion() {
    cout << endl << _programName << " " << _version << endl << endl;
}

void EZGetOpt::GetOpt::parseOptions() {
    if (_opts.empty()) {
        for (unsigned long i = 0; i < _args.size(); i++) {
            if (_args.at(i).regexMatch("^-")) {
                EZString p = _args.at(i);
                p.regexReplace("^-", "");
                for (unsigned long ii = 0; ii < p.size(); ii++) {
                    stringstream ss;
                    ss << p.at(ii);
                    if (_options.find(ss.str() + ":") != _options.end()) {
                        if (_args.at(i + 1).regexMatch("^-")) {
                            cout << "Error: option " << ss.str() << " requires an argument" << endl;
                            exit(EXIT_FAILURE);
                        }
                        //cout << ss.str() << " " << _args.at(i + 1) << endl;
                        _parsedOptions.insert(pair<char, EZString>(ss.str().at(0), _args.at(i + 1)));
                        i++;
                    } else {
                        if (_options.find(ss.str()) != _options.end()) {
                            _parsedOptions.insert(pair<char, EZString>(ss.str().at(0), ""));
                            //cout << ss.str() << endl;
                        } else {
                            cout << "Error: use of unknown option " << ss.str() << endl;
                            exit(EXIT_FAILURE);
                        }
                    }
                }
            } else {
                cout << "Error: unmatched argument " << _args.at(i) << endl;
                exit(EXIT_FAILURE);
            }
        }
    }
}

bool EZGetOpt::GetOpt::option(EZString opt) {
    return _parsedOptions.find(opt.at(0)) != _parsedOptions.end();
}

EZString EZGetOpt::GetOpt::argForOption(EZString opt) {
    if (_parsedOptions.find(opt.at(0)) != _parsedOptions.end()) {
        return _parsedOptions.find(opt.at(0))->second;
    } else {
        return "";
    }
}
