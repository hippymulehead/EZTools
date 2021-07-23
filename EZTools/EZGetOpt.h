/*
Copyright (c) 2017-2021, Michael Romans of Romans Audio
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

#ifndef EZT_EZGETOPT_H
#define EZT_EZGETOPT_H

#include <map>
#include "EZTools.h"
#include "EZScreen.h"

namespace EZGetOpt {

    /*
     * EZGetOpt::GetOpt
     * A fully C++ version of getopt std that creates your help screens for you and gives you added powers.
     */
    class GetOpt {
    public:
        GetOpt() = default;
        GetOpt(int argc, char* argv[], EZTools::EZString programName, EZTools::EZString version) {
            _programName = programName;
            _version = version;
            std::vector<EZTools::EZString> arguments(argv + 1, argv + argc);
            _args = arguments;
        }
        ~GetOpt() = default;
        void init(int argc, char* argv[], EZTools::EZString programName, EZTools::EZString version) {
            _programName = programName;
            _version = version;
            std::vector<EZTools::EZString> arguments(argv + 1, argv + argc);
            _args = arguments;
        }
        EZTools::EZString programName() { return _programName; }
        void addCommandLineOption(EZTools::EZString option, EZTools::EZString description) {
            _options.insert(std::pair<EZTools::EZString, EZTools::EZString>(option, description));
        }
        void addUnpairedOptions(EZTools::EZString options) {
            auto sp = options.split(":");
            _unpairedRequired = sp.size();
            for (auto& n : sp) {
                _unpairedNames.push_back(n);
            }
        }
        void addCopyright(EZTools::EZString copyright) { _copyright = copyright; }
        void addExtraMessage(EZTools::EZString extraMessage) { _extraMessage = extraMessage; }
        EZTools::EZOpts options() { return _parsedOptions; }
        bool option(EZTools::EZString opt) {
            return _parsedOptions.find(opt.at(0)) != _parsedOptions.end();
        }
        EZTools::EZString valueForOption(EZTools::EZString opt) {
            if (_parsedOptions.find(opt.at(0)) != _parsedOptions.end()) {
                return _parsedOptions.find(opt.at(0))->second;
            } else {
                return "";
            }
        }
        bool hasOption(EZTools::EZString opt) {
            if (_parsedOptions.find(opt.at(0)) != _parsedOptions.end()) {
                return true;
            } else {
                return false;
            }
        }
        void showHelp(bool showColor = true) {
            EZScreen::color = showColor;
            std::stringstream ss;
            ss << EZScreen::foreground(EZScreen::BRIGHT_GREEN);
            ss << std::endl << _programName;
            ss << EZScreen::foreground(EZScreen::BRIGHT_CYAN);
            ss << " v" << _version;
            ss << EZScreen::foreground(EZScreen::BRIGHT_YELLOW);
            ss << " " << _copyright << std::endl;
            if (!_extraMessage.empty()) {
                ss << EZScreen::foreground(EZScreen::BRIGHT_MAGENTA);
                ss << _extraMessage << std::endl;
            }
            ss << EZScreen::foreground(EZScreen::BRIGHT_YELLOW);
            ss << "Help: ";
            ss << EZScreen::foreground(EZScreen::BRIGHT_GREEN);
            ss << _programName << " <-";
            ss << EZScreen::foreground(EZScreen::BRIGHT_YELLOW);
            for (auto& op : _options) {
                ss << op.first;
            }
            ss << EZScreen::foreground(EZScreen::BRIGHT_GREEN);
            ss << ">";
            ss << EZScreen::foreground(EZScreen::BRIGHT_BLUE);
            if (_unpairedRequired > 0) {
                for (auto& oo : _unpairedNames) {
                    ss << " <" << oo << ">";
                }
            }
            ss << std::endl;
            for (auto& op : _options) {
                ss << EZScreen::foreground(EZScreen::BRIGHT_YELLOW);
                ss << "  " << op.first << " \t";
                ss << EZScreen::foreground(EZScreen::BRIGHT_GREEN);
                ss << op.second << std::endl;
            }
            ss << std::endl;
            ss << CON_RESET;
            std::cout << ss.str();
        }
        void showVersion(bool showColor = true) {
            std::stringstream ss;
            EZScreen::color = showColor;
            ss << std::endl << _programName << " " << _version << std::endl << std::endl;
            std::cout << ss.str();
        }
        EZTools::EZReturn<bool> parseOptions() {
            EZTools::EZReturn<bool> res;
            res.metaData.location = "EZGetOpt::parseOptions";
            std::vector<EZTools::EZString> noOpts;
            std::vector<EZTools::EZString> opts;
            int noOptCounter = 0;
            for (auto& arg : _args) {
                if (arg.regexMatch("^-[a-zA-Z]*")) {
                    arg.regexReplace("-", "");
                    for (auto& a : arg) {
                        std::stringstream ss;
                        ss << a;
                        if (ss.str() == "h" || ss.str() == "v") {
                            _helpOrVersion = true;
                        }
                        opts.push_back(ss.str());
                    }
                } else {
                    noOpts.push_back(arg);
                }
            }
            for (auto& o : opts) {
                if (_options.find(o) != _options.end()) {
                    _parsedOptions.insert(std::pair<char, EZTools::EZString>(o.at(0), ""));
                } else if (_options.find(o + ":") != _options.end()) {
                    if (noOptCounter < noOpts.size() && !noOpts.empty()) {
                        _parsedOptions.insert(std::pair<char, EZTools::EZString>(o.at(0), noOpts.at(noOptCounter)));
                        noOptCounter++;
                    } else {
                        std::stringstream ss;
                        ss << "Option " << o << " requires an argument";
                        res.message(ss.str());
                        res.exitCode(10);
                        res.wasSuccessful(false);
                        return res;
                    }
                } else {
                    std::stringstream ss;
                    ss << "Option " << o << " is unknown";
                    res.message(ss.str());
                    res.exitCode(11);
                    res.wasSuccessful(false);
                    return res;
                }
            }
            if (noOptCounter != noOpts.size() || noOpts.empty()) {
                for (int i = noOptCounter; i < noOpts.size(); i++) {
                    _unpairedArgs.push_back(noOpts.at(i));
                }
            }
            if (!_helpOrVersion) {
                if (_unpairedArgs.size() < _unpairedRequired) {
                    std::stringstream ss;
                    ss << "Not enough arguments, " << _unpairedArgs.size() << " found but " << _unpairedRequired
                        << " required";
                    res.message(ss.str());
                    res.exitCode(12);
                    res.wasSuccessful(false);
                    return res;
                }
            }
            res.wasSuccessful(true);
            return res;
        }
        std::vector<EZTools::EZString> unpairedArgs() { return _unpairedArgs; }
        int requiredUnpairedArgs() { return _unpairedRequired; }

    private:
        bool _helpOrVersion = false;
        EZTools::EZString _programName;
        std::map<EZTools::EZString, EZTools::EZString> _options;
        EZTools::EZString _copyright;
        EZTools::EZString _extraMessage;
        EZTools::EZString _version;
        std::vector<EZTools::EZString> _args;
        std::vector<EZTools::EZString> _unpairedArgs;
        EZTools::EZOpts _opts;
        EZTools::EZOpts _parsedOptions;
        unsigned long long _unpairedRequired = 0;
        std::vector<EZTools::EZString> _unpairedNames;
        std::map<EZTools::EZString, EZTools::EZString> _requiredArgs;
    };
}

#endif //EZT_EZGETOPT_H
