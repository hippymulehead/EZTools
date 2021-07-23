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

#ifndef EZT_EZDNS_H
#define EZT_EZDNS_H

#include <cstring> //memset
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "EZFiles.h"
#include "EZHTTP.h"
#include "json.h"
#include <iostream>
#include <string>
#include <vector>
#include <arpa/nameser.h>
#include <resolv.h>

namespace EZDNS {

    typedef std::vector<EZTools::EZString> ip_t_ipv4;
    typedef std::vector<EZTools::EZString> ip_t_ipv6;
    typedef std::vector<EZTools::EZString> resolver_t;
    typedef std::vector<EZTools::EZString> ns_t;
    typedef EZTools::EZString ptr_t;

    struct ip_t {
        ip_t_ipv4 A;
    };

    struct mx_t_rec {
        int weight = 0;
        EZTools::EZString name;
        bool operator<(const mx_t_rec& a) const {
            if (weight == a.weight) {
                return name < a.name;
            } else {
                return weight < a.weight;
            }
        }
    };

    typedef std::vector<mx_t_rec> mx_t;

    struct EZDNSHostEnt {
        EZTools::EZString hostname;
        ip_t ips;
        resolver_t localResolver;
        ns_t ns;
        ptr_t ptr;
        mx_t mx;
        std::vector<EZTools::EZString> problems;
    };

    inline bool isIP(EZTools::EZString ipAddress) {
        return ipAddress.regexMatch(EZTools::IPv4Regex);
    }

    inline EZTools::EZReturn<ip_t> whatIsMyExternalIP() {
        EZTools::EZReturn<ip_t> ezReturn;
        ezReturn.metaData.location = "EZDNS::whatIsMyExternalIP";
        EZHTTP::URI ipify("https://api.ipify.org?format=json");
        EZHTTP::HTTP http;
        auto response = http.get(ipify);
        if (!response.wasSuccessful()) {
            ezReturn.message(response.message());
            return ezReturn;
        }
        if (response.data().empty()) {
            ezReturn.message("No value returned");
            return ezReturn;
        }
        nlohmann::json root = nlohmann::json::parse(response.data());
        ezReturn.data.A.push_back(root["ip"].get<EZTools::EZString>());
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<resolver_t> getSystemResolvers() {
        EZTools::EZReturn<resolver_t> res;
        EZFiles::URI resolvConf("/etc/resolv.conf");
        auto r = EZFiles::copyFileToVectorOfEZString(resolvConf);
        if (!r.wasSuccessful()) {
            res.message(r.message());
            return res;
        }
        bool hasData = false;
        for (auto& line : r.data) {
            if (line.regexMatch("^nameserver ")) {
                line.regexReplace("^nameserver ", "");
                res.data.push_back(line);
                hasData = true;
            }
        }
        if (!hasData) {
            res.message("RESOLV ERROR: No name servers found");
            return res;
        }
        res.wasSuccessful(true);
        return res;
    }

    inline EZTools::EZReturn<ip_t> getAddrByHost(const EZTools::EZString &hostname) {
        EZTools::EZReturn<ip_t> ezReturn;
        ezReturn.metaData.location = "EZDNS::getAddrByHost";
        struct addrinfo hints{}, *res, *p;
        int status, ai_family;
        char ip_address[INET6_ADDRSTRLEN];
        ai_family = AF_UNSPEC;
        memset(&hints, 0, sizeof hints);
        hints.ai_family = ai_family;
        hints.ai_socktype = SOCK_STREAM;
        if ((status = getaddrinfo(hostname.c_str(), nullptr, &hints, &res)) != 0) {
            std::stringstream ss;
            ss << "getaddrinfo: "<< gai_strerror(status);
            ezReturn.message(ss.str());
            return ezReturn;
        }
        for(p = res;p != nullptr; p = p->ai_next) {
            void *addr;
            bool v4 = true;
            if (p->ai_family == AF_INET) {
                auto *ipv4 = (struct sockaddr_in *)p->ai_addr;
                addr = &(ipv4->sin_addr);
            } else {
                auto *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
                addr = &(ipv6->sin6_addr);
                v4 = false;
            }
            inet_ntop(p->ai_family, addr, ip_address, sizeof ip_address);
            EZTools::EZString ip = ip_address;
            ip.removeNonPrintable();
            ip.removeOpeningSpaces();
            ezReturn.data.A.push_back(ip);
        }
        freeaddrinfo(res);
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<ptr_t> getPTRByAddr(EZTools::EZString ipAddress) {
        EZTools::EZReturn<ptr_t> ezReturn;
        ezReturn.metaData.location = "EZDNS::getPTRByAddr";
        struct sockaddr_in sa{};
        char node[NI_MAXHOST];
        memset(&sa, 0, sizeof sa);
        sa.sin_family = AF_INET;
        inet_pton(AF_INET, ipAddress.c_str(), &sa.sin_addr);
        int res = getnameinfo((struct sockaddr*)&sa, sizeof(sa), node, sizeof(node),
                nullptr, 0, NI_NAMEREQD);
        if (res) {
            std::stringstream ss;
            ss << res << " " << gai_strerror(res);
            ezReturn.message(ss.str());
            return ezReturn;
        } else {
            ezReturn.data = node;
        }
        if (ezReturn.data.empty()) {
            ezReturn.message("Can't get ptr");
            return ezReturn;
        }
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<mx_t> getMXByHost(EZTools::EZString hostname) {
        EZTools::EZString hn = hostname;
        EZTools::EZReturn<mx_t> ezReturn;
        ezReturn.metaData.location = "EZDNS::getMXByHost";
        auto tmp = hostname.split(".");
        if (tmp.size() > 2) {
            std::stringstream ss;
            ss << tmp.at(tmp.size() - 2) << "." << tmp.at(tmp.size() - 1);
            hn = ss.str();
        }
        u_char nsbuf[4096];
        char dispbuf[4096];
        ns_msg msg;
        ns_rr rr;
        int i, l;
        l = res_query(hn.c_str(), ns_c_any, ns_t_mx, nsbuf, sizeof(nsbuf));
        if (l < 0) {
            ezReturn.message("No value returned");
            return ezReturn;
        } else {
            ns_initparse(nsbuf, l, &msg);
            l = ns_msg_count(msg, ns_s_an);
            for (i = 0; i < l; i++) {
                ns_parserr(&msg, ns_s_an, i, &rr);
                ns_sprintrr(&msg, &rr, nullptr, nullptr, dispbuf, sizeof(dispbuf));
                mx_t_rec t;
                EZTools::EZString s = dispbuf;
                auto sp1 = s.split("IN MX");
                sp1.at(1).removeNonPrintable();
                sp1.at(1).regexReplace(".$", "");
                auto sp2 = sp1.at(1).split(" ");
                t.weight = sp2.at(0).asInt();
                t.name = sp2.at(1);
                ezReturn.data.push_back(t);
            }
        }
        if (ezReturn.data.empty()) {
            ezReturn.message("No value returned");
            return ezReturn;
        }
        std::sort(ezReturn.data.begin(), ezReturn.data.end());
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<ns_t> getNSByHost(EZTools::EZString hostname) {
        EZTools::EZReturn<ns_t> ezReturn;
        ezReturn.metaData.location = "EZDNS::getNSByHost";
        u_char nsbuf[4096];
        char dispbuf[4096];
        ns_msg msg;
        ns_rr rr;
        int i, l;
        l = res_query(hostname.c_str(), ns_c_any, ns_t_ns, nsbuf, sizeof(nsbuf));
        if (l < 0) {
            ezReturn.message("No value returned");
            return ezReturn;
        } else {
            ns_initparse(nsbuf, l, &msg);
            l = ns_msg_count(msg, ns_s_an);
            for (i = 0; i < l; i++) {
                ns_parserr(&msg, ns_s_an, i, &rr);
                ns_sprintrr(&msg, &rr, nullptr, nullptr, dispbuf, sizeof(dispbuf));
                EZTools::EZString s = dispbuf;
                auto sp1 = s.split("IN NS");
                sp1.at(1).removeOpeningSpaces();
                sp1.at(1).removeNonPrintable();
                sp1.at(1).regexReplace(".$", "");
                ezReturn.data.push_back(sp1.at(1));
            }
        }
        std::sort(ezReturn.data.begin(), ezReturn.data.end());
        if (ezReturn.data.empty()) {
            ezReturn.message("No value returned");
            return ezReturn;
        }
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<EZDNSHostEnt> getHostEnt(EZTools::EZString whatToLookUp) {
        EZTools::EZReturn<EZDNSHostEnt> hostEnt;
        hostEnt.metaData.location = "EZDNS::getHostEnt";
        auto re = getSystemResolvers();
        if (!re.wasSuccessful()) {
            hostEnt.data.problems.emplace_back("Your system does not have a proper resolv.conf");
            hostEnt.message(re.message());
            return hostEnt;
        }
        hostEnt.data.localResolver = re.data;
        if (isIP(whatToLookUp)) {
            auto ptr = getPTRByAddr(whatToLookUp);
            if (!ptr.wasSuccessful()) {
                hostEnt.data.problems.emplace_back(whatToLookUp+" has no PTR record");
                hostEnt.message(ptr.message());
                return hostEnt;
            }
            hostEnt.data.ptr = ptr.data;
            hostEnt.data.hostname = ptr.data;
            auto ips = getAddrByHost(hostEnt.data.hostname);
            if (!ips.wasSuccessful()) {
                hostEnt.data.problems.emplace_back(whatToLookUp+" fails lookup");
                hostEnt.message(ips.message());
                return hostEnt;
            }
            hostEnt.data.ips = ips.data;
        } else {
            hostEnt.data.hostname = whatToLookUp;
            auto ips = getAddrByHost(hostEnt.data.hostname);
            if (!ips.wasSuccessful()) {
                hostEnt.data.problems.emplace_back(whatToLookUp+" fails lookup");
                hostEnt.message(ips.message());
                return hostEnt;
            }
            hostEnt.data.ips = ips.data;
            auto ptr = getPTRByAddr(hostEnt.data.ips.A.at(0));
            if (ptr.wasSuccessful()) {
                hostEnt.data.ptr = ptr.data;
            } else {
                hostEnt.data.problems.emplace_back(whatToLookUp+" has no PTR record");
                hostEnt.data.ptr = ptr.message();
            }
        }
        auto mx = getMXByHost(hostEnt.data.hostname);
        if (mx.wasSuccessful()) {
            if (mx.data.size() < 2) {
                hostEnt.data.problems.emplace_back(whatToLookUp+" only has a single MX record");
            }
            hostEnt.data.mx = mx.data;
        }
        auto ns = getNSByHost(hostEnt.data.hostname);
        if (ns.wasSuccessful()) {
            if (ns.data.size() < 2) {
                hostEnt.data.problems.emplace_back(whatToLookUp+" only has a single NS record");
            }
            hostEnt.data.ns = ns.data;
        }
        hostEnt.wasSuccessful(true);
        return hostEnt;
    }

//    inline EZTools::TEST_RETURN TEST() {
//        EZTools::TEST_RETURN res("EZDNS", false);
//        auto ip = whatIsMyExternalIP();
//        if (!ip.wasSuccessful()) {
//            res.functionTest("whatIsMyExternalIP()");
//            res.message(ip.message());
//            return res;
//        }
//        res.output << "whatIsMyExternalIP() - " << ip.data.A.at(0) << std::endl;
//
//        auto ptr = getPTRByAddr(ip.data.A.at(0));
//        if (!ptr.wasSuccessful()) {
//            res.functionTest("getPTRByAddr(ip.data.A.at(0)");
//            res.message(ptr.message());
//            return res;
//        }
//        res.output << "getPTRByAddr(ip.data.A.at(0) - " << ptr.data << std::endl;
//
//        auto mx = EZDNS::getMXByHost("google.com");
//        if (!mx.wasSuccessful()) {
//            res.functionTest("getMXByHost(\"google.com\")");
//            res.message(mx.message());
//            return res;
//        }
//        res.output << "getMXByHost(\"google.com\")" << std::endl;
//        for (auto& m : mx.data) {
//            res.output << "\t" << m.weight << " " << m.name << std::endl;
//        }
//
//        auto ns = getNSByHost("google.com");
//        if (!ns.wasSuccessful()) {
//            res.functionTest("getNSByHost(\"google.com\")");
//            res.message(ns.message());
//            return res;
//        }
//        res.output << "getNSByHost(\"google.com\")" << std::endl;
//        for (auto& n : ns.data) {
//            res.output << "\t" << n << std::endl;
//        }
//
//        auto ent = getHostEnt("google.com");
//        if (!ent.wasSuccessful()) {
//            res.functionTest("getHostEnt(\"google.com\")");
//            res.message(ent.message());
//            return res;
//        }
//        res.output << "getHostEnt(\"google.com\")" << std::endl;
//        res.output << "\tHostname: " << ent.data.hostname << std::endl;
//        for (auto& ipp: ent.data.ips.A) {
//            res.output << "\tIP: " << ipp << std::endl;
//        }
//        for (auto& ipp: ent.data.localResolver) {
//            res.output << "\tResolver: " << ipp << std::endl;
//        }
//        for (auto& ipp: ent.data.ns) {
//            res.output << "\tNS: " << ipp << std::endl;
//        }
//        res.output << "\tPTR: " << ent.data.ptr << std::endl;
//        for (auto& ipp: ent.data.mx) {
//            res.output << "\tMX: " << ipp.weight << " " << ipp.name << std::endl;
//        }
//        for (auto& p : ent.data.problems) {
//            res.output << p << std::endl;
//        }
//        res.wasSuccessful(true);
//        return res;
//    }

}

#endif //EZT_EZDNS_H
