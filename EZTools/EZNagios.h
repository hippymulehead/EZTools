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

#ifndef EZT_EZNAGIOS_H
#define EZT_EZNAGIOS_H

#pragma once

#include "EZTools.h"

namespace EZNagios {

    class Nagios {
    public:
        Nagios() = default;
        ~Nagios() = default;
        EZTools::EZString exit(EZTools::NAGIOS_EXIT_LEVELS exitLevel, bool testing = false) {
            std::stringstream ss;
            if (_title.empty()) {
                ss << NAGEXITLEVELSAsEZSting(exitLevel) << _message.str();
            } else {
                ss << NAGEXITLEVELSAsEZSting(exitLevel) << _title.str();
            }
            bool first = true;
            if (!_dataPoints.empty()) {
                for (auto& m : _dataPoints) {
                    if (first) {
                        ss << "|";
                        first = false;
                    } else {
                        ss << ";";
                    }
                    ss << m.first << "=" << m.second;
                }
            }
            if (testing) {
                return ss.str();
            }
            std::cout << ss.str() << std::endl;
            std::exit(exitLevel);
        }
        void addDataPoint(EZTools::EZString name, EZTools::EZString data) {
            _dataPoints.insert(std::pair<EZTools::EZString, EZTools::EZString>(name, data));
        }
        void title(EZTools::EZString title) {
            _title = title;
        }

    private:
        std::stringstream _message;
        std::map<EZTools::EZString, EZTools::EZString> _dataPoints;
        EZTools::EZString _title;
        template<typename T>
        friend std::ostream &operator<<(Nagios &log, T op) {
            log._message << op;
            return log._message;
        }
        static EZTools::EZString NAGEXITLEVELSAsEZSting(EZTools::NAGIOS_EXIT_LEVELS exitLevel) {
            switch (exitLevel) {
                case EZTools::NAGIOS_EXIT_LEVELS::NAG_OK:
                    return "OK: ";
                case EZTools::NAGIOS_EXIT_LEVELS::NAG_WARN:
                    return "WARNING: ";
                case EZTools::NAGIOS_EXIT_LEVELS::NAG_CRIT:
                    return "CRITICAL: ";
                default:
                    return "UNKNOWN: ";
            }
        }
    };

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZNagios");
        EZNagios::Nagios nag;
        nag << "TEST MESSAGE";
        auto re = nag.exit(EZTools::NAG_OK, true);
        if (re == "OK: TEST MESSAGE") {
            res.wasSuccessful(true);
            res.output(re);
            return res;
        } else {
            res.message("Message didn't match: "+re);
            return res;
        }
    }

}


#endif //EZT_EZNAGIOS_H
