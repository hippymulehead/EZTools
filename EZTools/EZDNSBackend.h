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

//FIXME: This sucks but we have to hit many kernel targets and the 5.x kernels are changing how all this is done.
//Once that calms down I will rewrite this to no longer suck.
//Once that has been done the old EZDNS will become this rather than just using this.  This will do more and be more
//simple to use.  This also include v6 IP space.

#ifndef EZT_EZDNSBACKEND_H
#define EZT_EZDNSBACKEND_H

#pragma once

#include "EZTools.h"
#include "EZLinux.h"

namespace EZDNSBackend {

    struct mxRecord_t {
        EZTools::EZString type;
        EZTools::EZString weight;
        EZTools::EZString name;
    };
    struct nsRecord_t {
        EZTools::EZString type;
        EZTools::EZString name;
    };
    struct soaRecord_t {
        EZTools::EZString type;
        EZTools::EZString zoneChange;
        EZTools::EZString refresh;
        EZTools::EZString retry;
        EZTools::EZString nxAuthoritative;
        EZTools::EZString nxTTL;
        EZTools::EZString name;
    };

    inline EZTools::EZReturn<std::vector<mxRecord_t>> DigMX(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<mxRecord_t>> answers;
        std::stringstream command;
        command << "dig " << server << " MX";
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                ll.at(5).regexReplace(".$", "");
                mxRecord_t t;
                t.type = "MX";
                t.weight = ll.at(4);
                t.name = ll.at(5);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

    inline EZTools::EZReturn<std::vector<nsRecord_t>> DigA(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<nsRecord_t>> answers;
        std::stringstream command;
        command << "dig " << server << " A";
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                answers.wasSuccessful(false);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                nsRecord_t t;
                t.type = "A";
                t.name = ll.at(4);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

    inline EZTools::EZReturn<std::vector<nsRecord_t>> DigAAAA(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<nsRecord_t>> answers;
        std::stringstream command;
        command << "dig " << server << " AAAA";
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                nsRecord_t t;
                t.type = "AAAA";
                t.name = ll.at(4);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

    inline EZTools::EZReturn<std::vector<nsRecord_t>> DigNS(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<nsRecord_t>> answers;
        std::stringstream command;
        command << "dig " << server << " NS";
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                ll.at(4).regexReplace(".$", "");
                nsRecord_t t;
                t.type = "NS";
                t.name = ll.at(4);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

    inline EZTools::EZReturn<std::vector<soaRecord_t>> DigSOA(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<soaRecord_t>> answers;
        std::stringstream command;
        command << "dig " << server << " SOA";
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                ll.at(4).regexReplace(".$", "");
                soaRecord_t t;
                t.type = "SOA";
                t.name = ll.at(4);
                t.zoneChange = ll.at(6);
                t.refresh = ll.at(7);
                t.retry = ll.at(8);
                t.nxAuthoritative = ll.at(9);
                t.nxTTL = ll.at(10);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

    inline EZTools::EZReturn<std::vector<nsRecord_t>> DigPTR(EZTools::EZString server) {
        EZTools::EZReturn<std::vector<nsRecord_t>> answers;
        std::stringstream command;
        command << "dig -x " << server;
        auto output = EZLinux::exec(command.str());
        auto sp = output.data.split("\n");
        bool a = false;
        for (auto& l : sp) {
            if (l.regexMatch("NXDOMAIN")) {
                l.regexReplace(";; ->>HEADER<<- ", "");
                answers.message(l);
                return answers;
            }
            if (a && l.empty()) {
                a = false;
            }
            if (a) {
                l.regexReplace("\t", " ");
                l.removeExtraSpaces();
                auto ll = l.split(" ");
                ll.at(4).regexReplace(".$", "");
                nsRecord_t t;
                t.type = "PTR";
                t.name = ll.at(4);
                answers.data.push_back(t);
            }
            if (l.regexMatch("^;; ANSWER SECTION:")) {
                a = true;
            }
        }
        answers.wasSuccessful(true);
        return answers;
    }

}

#endif //EZT_EZDNSBACKEND_H
