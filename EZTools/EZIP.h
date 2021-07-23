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

#ifndef EZT_EZIP_H
#define EZT_EZIP_H

#include <cstdio>
#include <netdb.h>
#include <ifaddrs.h>
#include <cstdlib>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/nameser_compat.h>
#include <resolv.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/if.h>

#include "EZTools.h"
#include "EZLinux.h"
#include "EZDateTime.h"
#include "EZDNS.h"
#include "EZHTTP.h"

namespace EZIP {

    struct IPAddressData {
        EZTools::EZString interface;
        EZTools::EZString v4Address;
    };

    inline bool isIP(EZTools::EZString ipAddress) {
        return ipAddress.regexMatch(EZTools::IPv4Regex);
    }

    inline std::vector<int> getOctets(EZTools::EZString address) {
        auto octs = address.regexExtract(EZTools::IPv4Regex);
        std::vector<int> res;
        if (octs.size() != 4) {
            return res;
        }
        for (auto& oct : octs) {
            res.push_back(oct.asInt());
        }
        return res;
    }

    class IPAddresses {
    public:
        IPAddresses() = default;
        ~IPAddresses() = default;
        void addPair(EZTools::EZString interface, EZTools::EZString v4Address) {
            IPAddressData d;
            d.interface = interface;
            d.v4Address = v4Address;
            _ips.emplace_back(d);
        }
        EZTools::EZString wifiAddress() {
            for (auto& ip : _ips) {
                if (ip.interface.regexMatch("^w")) {
                    return ip.v4Address;
                }
            }
            return "";
        }
        EZTools::EZString ethernetAddress() {
            for (auto& ip : _ips) {
                if (ip.interface.regexMatch("^e")) {
                    return ip.v4Address;
                }
            }
            return "";
        }
        EZTools::EZString localAddress() {
            for (auto& ip : _ips) {
                if (ip.interface.regexMatch("^l")) {
                    return ip.v4Address;
                }
            }
            return "";
        }
        EZTools::EZString returnBest() {
            if (!ethernetAddress().empty()) { return ethernetAddress(); } else
            if (!wifiAddress().empty()) { return wifiAddress(); } else
                return localAddress();
        }
    private:
        std::vector<IPAddressData> _ips;
    };

    inline bool isInterfaceUp(std::string interface) {
        struct ifreq ifr;
        int sock = socket(PF_INET6, SOCK_DGRAM, IPPROTO_IP);
        memset(&ifr, 0, sizeof(ifr));
        strcpy(ifr.ifr_name, interface.c_str());
        if (ioctl(sock, SIOCGIFFLAGS, &ifr) < 0) {
            perror("SIOCGIFFLAGS");
        }
        close(sock);
        return (ifr.ifr_flags & IFF_UP) != 0;
    }

    inline IPAddresses localIPAddresses() {
        IPAddresses addresses;
        struct ifaddrs *interfaces = nullptr;
        struct ifaddrs *temp_addr = nullptr;
        int success = 0;
        // retrieve the current interfaces - returns 0 on success
        success = getifaddrs(&interfaces);
        if (success == 0) {
            temp_addr = interfaces;
            while(temp_addr != nullptr) {
                if (isInterfaceUp(temp_addr->ifa_name)) {
                    if (temp_addr->ifa_addr->sa_family == AF_INET) {
                        addresses.addPair(temp_addr->ifa_name,
                                          inet_ntoa(((struct sockaddr_in *) temp_addr->ifa_addr)->sin_addr));
                    }
                }
                temp_addr = temp_addr->ifa_next;
            }
        }
        // Free memory
        freeifaddrs(interfaces);
        return addresses;
    }

    class IPAddress {
    public:
        IPAddress() = default;
        explicit IPAddress(EZTools::EZString address) {
            EZTools::EZString ip;
            if (isIP(address)) {
                ip = address;
            } else {
                return;
            }
            _octs = getOctets(ip);
            if (_octs.empty()) {
                _isGood = false;
            }
        }
        ~IPAddress() = default;
        void init(EZTools::EZString address) {
            EZTools::EZString ip;
            if (isIP(address)) {
                ip = address;
            } else {
                return;
            }
            _octs = getOctets(ip);
            if (_octs.empty()) {
                _isGood = false;
            }
        }
        bool isGood() { return _isGood; }
        EZTools::EZString asString() {
            std::stringstream ss;
            if (_isGood) {
                ss << _octs[0] << "." << _octs[1] << "." << _octs[2] << "." << _octs[3];
            } else {
                ss << "";
            }
            return ss.str();
        }
        void port(int portNumber) { _port = portNumber; }
        int port() { return _port; }
        bool canConnect() {
            int portno     = _port;
            int sockfd;
            struct sockaddr_in serv_addr;
            struct hostent *server;

            sockfd = socket(AF_INET, SOCK_STREAM, 0);
            if (sockfd < 0) {
                return false;
            }
            server = gethostbyname(asString().c_str());
            if (server == nullptr) {
                return false;
            }
            bzero((char *) &serv_addr, sizeof(serv_addr));
            serv_addr.sin_family = AF_INET;
            bcopy((char *)server->h_addr,
                  (char *)&serv_addr.sin_addr.s_addr,
                  server->h_length);

            serv_addr.sin_port = htons(portno);
            EZDateTime::Timer et;
            et.start();
            if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
                et.stop();
                _time = et.asMillisenconds();
                close(sockfd);
                return false;
            } else {
                et.stop();
                _time = et.asMillisenconds();
                close(sockfd);
                return true;
            }
        }
        float connectTime() { return _time; }
    private:
        bool _isGood;
        std::vector<int> _octs;
        int _port;
        float _time;
    };

    class LocalIPAddress {
    public:
        LocalIPAddress() {
            auto ips = localIPAddresses();
            auto temp = EZLinux::exec("/sbin/route | grep '^default'");
            if (!temp.data.empty()) {
                temp.data = temp.data.regexReplace("\n", "");
                while (temp.data.regexMatch("  ")) {
                    temp.data = temp.data.regexReplace("  ", " ");
                }
                std::vector<EZTools::EZString> data = temp.data.split(" ");
                _internetConnected = true;
                _defaultInterface = data.at(data.size() - 1);
            } else {
                _internetConnected = false;
            }
            if (_defaultInterface.regexMatch("^w")) {
                _defaultIP = ips.wifiAddress();
                _interfaceType = EZTools::INTERFACETYPE::WIFI;
            } else if (_defaultInterface.regexMatch("^e")) {
                _defaultIP = ips.ethernetAddress();
                _interfaceType = EZTools::INTERFACETYPE::ETHERNET;
            } else {
                _defaultIP = ips.localAddress();
                _interfaceType = EZTools::INTERFACETYPE::LOCALHOST;
            }
            std::vector<EZTools::EZString> octs = _defaultIP.regexExtract(EZTools::IPv4Regex);
            if (octs.empty()) {
                _isGood = false;
            } else {
                for (auto i = 0; i < 4; i++) {
                    _octs.push_back(octs.at(i + 1).asInt());
                    _isGood = true;
                }
            }
        }
        ~LocalIPAddress() = default;
        void retest() {
            auto ips = localIPAddresses();
            auto temp = EZLinux::exec("/sbin/route | grep '^default'");
            if (!temp.data.empty()) {
                temp.data = temp.data.regexReplace("\n", "");
                while (temp.data.regexMatch("  ")) {
                    temp.data = temp.data.regexReplace("  ", " ");
                }
                std::vector<EZTools::EZString> data = temp.data.split(" ");
                _internetConnected = true;
                _defaultInterface = data.at(data.size() - 1);
            } else {
                _internetConnected = false;
            }
            if (_defaultInterface.regexMatch("^w")) {
                _defaultIP = ips.wifiAddress();
                _interfaceType = EZTools::INTERFACETYPE::WIFI;
            } else if (_defaultInterface.regexMatch("^e")) {
                _defaultIP = ips.ethernetAddress();
                _interfaceType = EZTools::INTERFACETYPE::ETHERNET;
            } else {
                _defaultIP = ips.localAddress();
                _interfaceType = EZTools::INTERFACETYPE::LOCALHOST;
            }
            std::vector<EZTools::EZString> octs = _defaultIP.regexExtract(EZTools::IPv4Regex);
            if (octs.empty()) {
                _isGood = false;
            } else {
                for (auto i = 0; i < 4; i++) {
                    _octs.push_back(octs.at(i + 1).asInt());
                    _isGood = true;
                }
            }
        }
        bool testStack() {
            retest();
            if (!hasInternet()) {
                return false;
            }
            if (!canRoute()) {
                return false;
            }
            if (!isRoutable()) {
                EZTools::EZString nat = NATAddress();
                if (nat.empty()) {
                    return false;
                }
            }
            return true;
        }
        bool isGoodIP() { return _isGood; }
        EZTools::EZString asString() {
            std::stringstream ss;
            if (_isGood) {
                return _defaultIP;
            } else {
                ss << "";
            }
            return ss.str();
        }
        bool isRoutable() {
            if (_isGood) {
                return !(_octs[0] == 10
                         || _octs[0] == 127
                         || (_octs[0] == 169 && _octs[1] == 254)
                         || (_octs[0] == 172 && _octs[1] == 16)
                         || (_octs[0] == 192 && _octs[1] == 0 && _octs[2] == 0)
                         || (_octs[0] == 192 && _octs[1] == 168));
            }
            return false;
        }
        bool canRoute() {
            if (_isGood) {
                if (_octs[0] == 127) return false;
                if (_octs[0] == 169) {
                    if (_octs[1] == 254) return false;
                }
            }
            return true;
        }
        EZTools::EZString NATAddress() {
            EZTools::EZString externalIP;
            if (canRoute()) {
                if (!isRoutable()) {
                    EZHTTP::URI ipify("\"https://api.ipify.org/?format=json");
                    ipify.userAgent("EZTools v4 ip module");
                    EZHTTP::HTTP http;
                    auto response = http.get(ipify);
                    if (response.wasSuccessful()) {
                        externalIP = response.data();
                        externalIP.regexReplace("^{\"ip\":\"|\"}", "");
                        return externalIP;
                    }
                } else {
                    return asString();
                }
            } else {
                return "";
            }
            return "";
        }
        EZTools::EZString interface() { return _defaultInterface; }
        bool hasInternet() { return _internetConnected; }
        EZTools::INTERFACETYPE interfaceType() { return _interfaceType; }

    private:
        std::vector<int> _octs;
        bool _isGood = false;
        EZTools::EZString _defaultInterface;
        EZTools::EZString _defaultIP;
        bool _internetConnected;
        EZTools::INTERFACETYPE _interfaceType;
    };

    inline EZHTTP::URI ezipToURI(EZTools::CONNECTIONTYPE type, EZIP::IPAddress &ezip, EZTools::EZString path,
            int timeout = 15, EZTools::EZString username = "", EZTools::EZString password = "",
            bool hasRealCert = true) {
        std::stringstream ss;
        switch (type) {
            case EZTools::CONNECTIONTYPE::HTTP:
                ss << "http://";
                break;
            case EZTools::CONNECTIONTYPE::HTTPS:
                ss << "https://";
                break;
        }
        ss << ezip.asString();
        if (ezip.port() != 0) {
            ss << ":" << ezip.port();
        }
        if (!path.regexMatch("^/")) {
            ss << "/";
        }
        ss << path;
        EZHTTP::URI uri(ss.str(), timeout, username, password, hasRealCert);
        return uri;
    }

};

#endif //EZT_EZIP_H
