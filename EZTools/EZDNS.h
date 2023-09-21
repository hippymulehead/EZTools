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

#ifndef EZT_EZDNS_H
#define EZT_EZDNS_H

#pragma once

#include <cstring>
#include "EZFiles.h"
#include "EZHTTP.h"
#include "nlohmann/json.hpp"
#include <iostream>
#include <string>
#include <vector>
#include "EZLinux.h"
#include "EZDNSBackend.h"

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

    //FIXME::
    inline EZTools::EZReturn<ip_t> whatIsMyExternalIP() {
        EZTools::EZReturn<ip_t> ezReturn;
        ezReturn.location("EZDNS::whatIsMyExternalIP");
        EZHTTP::URI ipify("https://api.ipify.org");
        EZHTTP::HTTPClient http(ipify);
        auto response = http.get("/?format=json");
        if (!response.wasSuccessful()) {
            ezReturn.message(response.message());
            return ezReturn;
        }
        if (response.data.empty()) {
            ezReturn.message("No value returned");
            return ezReturn;
        }
        nlohmann::json root = nlohmann::json::parse(response.data);
        ezReturn.data.A.push_back(root["ip"].get<EZTools::EZString>());
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<resolver_t> getSystemResolvers() {
        EZTools::EZReturn<resolver_t> res;
        EZFiles::URI resolvConf("/etc/resolv.conf");
        auto r = EZFiles::copyFileToVectorOfEZString(&resolvConf);
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
        ezReturn.location("EZDNS::getAddrByHost");
        auto ret = EZDNSBackend::DigA(hostname);
        if (!ret.wasSuccessful()) {
            ezReturn.wasSuccessful(false);
            ezReturn.message(ret.message());
            return ezReturn;
        }
        for (auto& r : ret.data) {
            ezReturn.data.A.push_back(r.name);
        }
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<ptr_t> getPTRByAddr(EZTools::EZString ipAddress) {
        EZTools::EZReturn<ptr_t> ezReturn;
        ezReturn.location("EZDNS::getPTRByAddr");
        auto res = EZDNSBackend::DigPTR(ipAddress);
        if (!res.wasSuccessful()) {
            ezReturn.message(res.message());
            return ezReturn;
        }
        ezReturn.data = res.data.at(0).name;
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<mx_t> getMXByHost(EZTools::EZString hostname) {
        EZTools::EZString hn = hostname;
        EZTools::EZReturn<mx_t> ezReturn;
        ezReturn.location("EZDNS::getMXByHost");
        auto res = EZDNSBackend::DigMX(hostname);
        if (!res.wasSuccessful()) {
            ezReturn.message(res.message());
            return ezReturn;
        }
        for (auto& m : res.data) {
            mx_t_rec t;
            t.weight = m.weight.asInt();
            t.name = m.name;
            ezReturn.data.push_back(t);
        }
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<ns_t> getNSByHost(EZTools::EZString hostname) {
        EZTools::EZReturn<ns_t> ezReturn;
        ezReturn.location("EZDNS::getNSByHost");
        auto res = EZDNSBackend::DigNS(hostname);
        if (!res.wasSuccessful()) {
            ezReturn.message(res.message());
            return ezReturn;
        }
        for (auto& a : res.data) {
            ezReturn.data.push_back(a.name);
        }
        ezReturn.wasSuccessful(true);
        return ezReturn;
    }

    inline EZTools::EZReturn<EZDNSHostEnt> getHostEnt(EZTools::EZString whatToLookUp) {
        EZTools::EZReturn<EZDNSHostEnt> hostEnt;
        hostEnt.location("EZDNS::getHostEnt");
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

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZDNS");
        std::stringstream ss;
        auto ip = whatIsMyExternalIP();
        if (!ip.wasSuccessful()) {
            res.functionTest("whatIsMyExternalIP()");
            res.message(ip.message());
            return res;
        }
        ss << "whatIsMyExternalIP() - " << ip.data.A.at(0) << std::endl;

        auto ptr = getPTRByAddr(ip.data.A.at(0));
        if (!ptr.wasSuccessful()) {
            res.functionTest("getPTRByAddr(ip.data.A.at(0)");
            res.message(ptr.message());
            return res;
        }
        ss << "getPTRByAddr(ip.data.A.at(0) - " << ptr.data << std::endl;

        auto mx = EZDNS::getMXByHost("google.com");
        if (!mx.wasSuccessful()) {
            res.functionTest("getMXByHost(\"google.com\")");
            res.message(mx.message());
            return res;
        }
        ss << "getMXByHost(\"google.com\")" << std::endl;
        for (auto& m : mx.data) {
            ss << "\t" << m.weight << " " << m.name << std::endl;
        }

        auto ns = getNSByHost("google.com");
        if (!ns.wasSuccessful()) {
            res.functionTest("getNSByHost(\"google.com\")");
            res.message(ns.message());
            return res;
        }
        ss << "getNSByHost(\"google.com\")" << std::endl;
        for (auto& n : ns.data) {
            ss << "\t" << n << std::endl;
        }

        auto ent = getHostEnt("google.com");
        if (!ent.wasSuccessful()) {
            res.functionTest("getHostEnt(\"google.com\")");
            res.message(ent.message());
            return res;
        }
        ss << "getHostEnt(\"google.com\")" << std::endl;
        ss << "\tHostname: " << ent.data.hostname << std::endl;
        for (auto& ipp: ent.data.ips.A) {
            ss << "\tIP: " << ipp << std::endl;
        }
        for (auto& ipp: ent.data.localResolver) {
            ss << "\tResolver: " << ipp << std::endl;
        }
        for (auto& ipp: ent.data.ns) {
            ss << "\tNS: " << ipp << std::endl;
        }
        ss << "\tPTR: " << ent.data.ptr << std::endl;
        for (auto& ipp: ent.data.mx) {
            ss << "\tMX: " << ipp.weight << " " << ipp.name << std::endl;
        }
        for (auto& p : ent.data.problems) {
            ss << p << std::endl;
        }
        res.output(ss.str());
        res.wasSuccessful(true);
        return res;
    }

}

#endif //EZT_EZDNS_H