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
#ifndef EZTOOLS_EZNETWORKTESTS_H
#define EZTOOLS_EZNETWORKTESTS_H

#pragma once

#include <string>
#include <iostream>
#include <thread>
#include <vector>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "EZTools.h"
#include "EZDNS.h"
#include "EZIP.h"

namespace EZNetworkTests {

    using namespace std;

    struct portInfo_t {
        EZTools::EZString host;
        int port = 0;
        double connectTime = 0;
    };

    struct ports_t {
        EZTools::EZString host;
        int port = 0;
    };

    inline EZTools::EZReturn<portInfo_t> isPortOpen(EZTools::EZString host, int port) {
        EZTools::EZReturn<portInfo_t> res;
        while (!EZIP::isIP(host)) {
            auto ret = EZDNS::getAddrByHost(host);
            if (!ret) {
                res.data.host = host;
                res.data.port = port;
                res.message("DNS failure");
                res.wasSuccessful(false);
                return res;
            }
            host = ret.data.A.at(0);
        }
        res.data.host = host;
        res.data.port = port;
        struct sockaddr_in addr_s{};
        const char* addr = host.c_str();
        short int fd=-1;
        fd_set fdset;
        struct timeval tv{};
        int rc;
        int so_error;
        socklen_t len;
        struct timespec tstart={0,0}, tend={0,0};
        int seconds = 1;

        addr_s.sin_family = AF_INET;
        addr_s.sin_addr.s_addr = inet_addr(addr);
        addr_s.sin_port = htons(port);

        clock_gettime(CLOCK_MONOTONIC, &tstart);

        fd = socket(AF_INET, SOCK_STREAM, 0);
        fcntl(fd, F_SETFL, O_NONBLOCK); // setup non blocking socket

        // make the connection
        rc = connect(fd, (struct sockaddr *)&addr_s, sizeof(addr_s));
        if ((rc == -1) && (errno != EINPROGRESS)) {
            close(fd);
            res.wasSuccessful(false);
            res.message("not connected");
            return res;
        }
        if (rc == 0) {
            clock_gettime(CLOCK_MONOTONIC, &tend);
            close(fd);
            res.wasSuccessful(true);
            return res;
        }
        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);
        tv.tv_sec = seconds;
        tv.tv_usec = 0;

        rc = select(fd + 1, NULL, &fdset, NULL, &tv);
        switch(rc) {
            case 1: // data to read
                len = sizeof(so_error);
                getsockopt(fd, SOL_SOCKET, SO_ERROR, &so_error, &len);
                if (so_error == 0) {
                    clock_gettime(CLOCK_MONOTONIC, &tend);
                    std::stringstream ss;
                    ss << "Socket " << addr << ":" << port << " connected. It took " <<
                    (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec)) <<
                    " seconds.";
                    res.data.connectTime = (((double)tend.tv_sec + 1.0e-9*tend.tv_nsec) - ((double)tstart.tv_sec + 1.0e-9*tstart.tv_nsec));
                    res.message(ss.str());
                    close(fd);
                    res.wasSuccessful(true);
                    return res;
                } else {
                }
                break;
            case 0:
                break;
        }
        close(fd);
        res.wasSuccessful(false);
        res.message("not connected");
        return res;
    }

    class PortChecks {
    public:
        PortChecks() = default;
        ~PortChecks() = default;
        void addHost(EZTools::EZString host, int port) {
            ports_t p;
            p.host = host;
            p.port = port;
            _p.push_back(p);
        }
        std::vector<ports_t> hostsList() { return _p; }
    private:
        std::vector<ports_t> _p;
    };

    inline EZTools::EZReturn<std::vector<portInfo_t>> isListOpen(std::vector<ports_t> list) {
        EZTools::EZReturn<std::vector<portInfo_t>> res;
        for (auto& l : list) {
            auto r = isPortOpen(l.host, l.port);
            if (r) {
                portInfo_t pi;
                pi.host = r.data.host;
                pi.port = r.data.port;
                pi.connectTime = r.data.connectTime;
                res.data.emplace_back(pi);
            } else {
                res.message(r.data.host + ":" + EZTools::EZString(r.data.port) + " " + r.message());
                res.wasSuccessful(false);
                return res;
            }
        }
        double time = 0;
        for (auto& rs : res.data) {
            time = time + rs.connectTime;
        }
        res.message(EZTools::EZString(time));
        res.wasSuccessful(true);
        return res;
    }

}

#endif //EZTOOLS_EZNETWORKTESTS_H
