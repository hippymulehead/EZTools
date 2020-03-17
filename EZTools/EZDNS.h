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
either expressed or implied, of the EZTools project.
*/

#ifndef EZT_EZDNS_H
#define EZT_EZDNS_H

#include "EZTools.h"
#include <resolv.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <netdb.h>

namespace EZDNS {

    /*
     * EZDNS::RECORD_T
     * A enum type for future code
     */
    enum RECORD_T {A, PTR, MX, NS};

    /*
     * EZDNS::dndLookup
     * Right now a simple A record lookup.
     */
    inline EZTools::EZString dnsLookup(EZTools::EZString ip, RECORD_T type = A) {
        struct hostent *server;
        switch (type) {
            case A:
                server = gethostbyname(ip.c_str());
                break;
        }

        if (server == nullptr) {
            return "";
        }
        int i = 0;
        EZTools::EZString addr;
        while(server->h_addr_list[i] != nullptr) {
            addr = inet_ntoa( (struct in_addr) *((struct in_addr *) server->h_addr_list[i]));
            i++;
        }
        return addr;
    }

};

#endif //EZT_EZDNS_H
