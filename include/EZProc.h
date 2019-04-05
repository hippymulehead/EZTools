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

#ifndef EZTOOLS_EZPROC_H
#define EZTOOLS_EZPROC_H

#include <vector>
#include <fstream>
#include <sstream>
#include "EZString.h"

namespace EZProc {

    struct SysLoad {
        float oneMin;
        float fiveMin;
        float fifteenMin;
    };

    inline SysLoad loadavg() {
        SysLoad loadA;
        ifstream file("/proc/loadavg");
        if (file.is_open()) {
            EZString line;
            while (getline(file, line)) {
                vector<EZString> la = line.splitBy(" ");
                loadA.oneMin = la.at(0).asFloat();
                loadA.fiveMin = la.at(1).asFloat();
                loadA.fifteenMin = la.at(2).asFloat();
            }
            file.close();
        }
        return loadA;
    }

    struct CPUs {
        int processor;
        EZString vender_id;
        int cpu_family;
        int model;
        EZString model_name;
        int stepping;
        EZString microcode;
        float cpu_mhz;
        EZString cache_size;
        int physical_id;
        int siblings;
        int core_id;
        int cpu_cores;
        int apicid;
        int initial_apicid;
        bool fpu;
        bool fpu_exception;
        int cpuid_level;
        bool wp;
        EZString flags;
        EZString bugs;
        float bogomips;
        int clflush_size;
        int cache_alignment;
        EZString address_sizes;
        EZString power_management;
        bool hyperthreading;
    };

    class RAMSize {
    public:
        RAMSize() = default;

        virtual ~RAMSize() = default;

        int operator=(int s) {
            _rs = s;
            return _rs;
        }

        float asMegs() {
            return static_cast< float >(_rs) / 1024;
        }

        float asGigs() {
            return static_cast< float >(_rs) / 1024 / 1024;
        }

    private:
        int _rs;
    };

    class MemInfo {
    public:
        MemInfo() {
            ifstream inFile;
            inFile.open("/proc/meminfo");
            stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZString line1 = cpufile.str();
            inFile.close();
            vector<EZString> lines = line1.splitBy("\n");
            for (auto i = 0; i < lines.size(); i++) {
                lines.at(i) = lines.at(i).replaceAll(" ", "");
                lines.at(i) = lines.at(i).replaceAll("kB", "");
                if (lines.at(i).beginsWith("MemTotal:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _total = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("MemFree:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _free = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("MemAvailable:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _available = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("SwapTotal:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _totalSwap = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("SwapFree:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _freeSwap = sp.at(1).asInt();
                }
            }
        }

        virtual ~MemInfo() = default;

        void update() {
            ifstream inFile;
            inFile.open("/proc/meminfo");
            stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZString line1 = cpufile.str();
            inFile.close();
            vector<EZString> lines = line1.splitBy("\n");
            for (auto i = 0; i < lines.size(); i++) {
                lines.at(i) = lines.at(i).replaceAll(" ", "");
                lines.at(i) = lines.at(i).replaceAll("kB", "");
                if (lines.at(i).beginsWith("MemTotal:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _total = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("MemFree:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _free = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("MemAvailable:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _available = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("SwapTotal:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _totalSwap = sp.at(1).asInt();
                } else if (lines.at(i).beginsWith("SwapFree:")) {
                    vector<EZString> sp = lines.at(i).splitBy(":");
                    _freeSwap = sp.at(1).asInt();
                }
            }
        }

        RAMSize total() { return _total; }

        RAMSize free() { return _free; }

        RAMSize available() { return _available; }

        RAMSize swapTotal() { return _totalSwap; }

        RAMSize swapFree() { return _freeSwap; }

    private:
        RAMSize _total;
        RAMSize _free;
        RAMSize _available;
        RAMSize _totalSwap;
        RAMSize _freeSwap;
    };

    inline EZString kernelVersion() {
        ifstream inFile;
        inFile.open("/proc/version_signature");
        stringstream versig;
        versig << inFile.rdbuf();
        EZString line = versig.str();
        line = line.replaceAll("\n", "");
        vector<EZString> sp = line.splitBy(" ");
        auto s = sp.size() - 1;
        return sp.at(s);
    }

    class CPUInfo {
    public:
        CPUInfo() {
            CPUs cpu;
            int counter = 0;
            ifstream inFile;
            inFile.open("/proc/cpuinfo");
            stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZString line = cpufile.str();
            inFile.close();
            vector<EZString> lines = line.splitBy("\n");
            for (auto &line : lines) {
                if (!line.empty()) {
                    line = line.regexReplace("\t", "");
                    vector<EZString> sp;
                    if (line.includes(": ")) {
                        sp = line.splitBy(": ");
                    } else {
                        sp = line.splitBy(":");
                    }
                    sp.at(0) = sp.at(0).replaceAll(" ", "_");
                    if (sp.at(0) == "power_management") {
                        cpu.power_management = sp.at(1);
                        if (cpu.processor == 0) {
                            counter++;
                            cpu.processor = counter;
                            cpu.hyperthreading = cpu.cpu_cores != cpu.siblings;
                            _cpus.push_back(cpu);
                            cpu = CPUs();
                        }
                    }
                    if (sp.at(0) == "processor") {
                        cpu.processor = sp.at(1).asInt();
                    }
                    if (cpu.processor == 0) {
                        if (sp.at(0) == "vendor_id") {
                            cpu.vender_id = sp.at(1);
                        } else if (sp.at(0) == "cpu_family") {
                            cpu.cpu_family = sp.at(1).asInt();
                        } else if (sp.at(0) == "model") {
                            cpu.model = sp.at(1).asInt();
                        } else if (sp.at(0) == "model_name") {
                            cpu.model_name = sp.at(1);
                        } else if (sp.at(0) == "stepping") {
                            cpu.stepping = sp.at(1).asInt();
                        } else if (sp.at(0) == "microcode") {
                            cpu.microcode = sp.at(1);
                        } else if (sp.at(0) == "cpu_MHz") {
                            cpu.cpu_mhz = sp.at(1).asFloat();
                        } else if (sp.at(0) == "cache_size") {
                            cpu.cache_size = sp.at(1);
                        } else if (sp.at(0) == "physical_id") {
                            cpu.physical_id = sp.at(1).asInt();
                        } else if (sp.at(0) == "siblings") {
                            cpu.siblings = sp.at(1).asInt();
                        } else if (sp.at(0) == "core_id") {
                            cpu.core_id = sp.at(1).asInt();
                        } else if (sp.at(0) == "cpu_cores") {
                            cpu.cpu_cores = sp.at(1).asInt();
                        } else if (sp.at(0) == "apicid") {
                            cpu.apicid = sp.at(1).asInt();
                        } else if (sp.at(0) == "initial_apicid") {
                            cpu.initial_apicid = sp.at(1).asInt();
                        } else if (sp.at(0) == "fpu") {
                            cpu.fpu = sp.at(1) == "yes";
                        } else if (sp.at(0) == "fpu_exception") {
                            cpu.fpu_exception = sp.at(1) == "yes";
                        } else if (sp.at(0) == "cpuid_level") {
                            cpu.cpuid_level = sp.at(1).asInt();
                        } else if (sp.at(0) == "wp") {
                            cpu.wp = sp.at(1) == "yes";
                        } else if (sp.at(0) == "flags") {
                            cpu.flags = sp.at(1);
                        } else if (sp.at(0) == "bugs") {
                            cpu.bugs = sp.at(1);
                        } else if (sp.at(0) == "bogomips") {
                            cpu.bogomips = sp.at(1).asFloat();
                        } else if (sp.at(0) == "clflush_size") {
                            cpu.clflush_size = sp.at(1).asInt();
                        } else if (sp.at(0) == "cache_alignment") {
                            cpu.cache_alignment = sp.at(1).asInt();
                        } else if (sp.at(0) == "address_sizes") {
                            cpu.address_sizes = sp.at(1);
                        }
                    }
                }
            }
        }

        virtual ~CPUInfo() = default;

        vector<CPUs> cpu() { return _cpus; }

    private:
        vector<CPUs> _cpus;
    };

}

#endif //EZTOOLS_EZPROC_H
