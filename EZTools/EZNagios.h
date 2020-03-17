/*
Copyright (c) 2018-2019, Michael Romans of Romans Audio
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

#include "EZTools.h"

namespace EZNagios {

    enum _NAGIOS_EXIT_LEVELS_T {
        NAG_OK = 0,
        NAG_WARN = 1,
        NAG_CRIT = 2,
        NAG_UNKNOWN = 3
    };

    class Nagios {
    public:
        Nagios() = default;
        ~Nagios() = default;
        void exit(_NAGIOS_EXIT_LEVELS_T exitLevel) {
            std::cout << NAGEXITLEVELSAsEZSting(exitLevel) << _message.str() << std::endl;
            std::exit(exitLevel);
        }

    private:
        std::stringstream _message;
        template<typename T>
        friend std::ostream &operator<<(Nagios &log, T op) {
            log._message << op;
            return log._message;
        }
        static EZTools::EZString NAGEXITLEVELSAsEZSting(_NAGIOS_EXIT_LEVELS_T exitLevel) {
            switch (exitLevel) {
                case NAG_OK:
                    return "OK: ";
                case NAG_WARN:
                    return "WARNING: ";
                case NAG_CRIT:
                    return "CRITICAL: ";
                default:
                    return "UNKNOWN: ";
            }
        }
    };

}


#endif //EZT_EZNAGIOS_H
