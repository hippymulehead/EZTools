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

#ifndef EZT_EZPROC_H
#define EZT_EZPROC_H

#include <fstream>
#include "EZTools.h"
#include "EZLinux.h"

namespace EZProc {

    struct EZProcess {
        EZTools::EZString UID;
        int PID;
        int PPID;
        int C;
        EZTools::EZString STIME;
        EZTools::EZString TTY;
        EZTools::EZString TIME;
        EZTools::EZString CMD;
        EZTools::EZString BASECMD;

        EZProcess(EZTools::EZString uid, int pid, int ppid, int c, EZTools::EZString stime, EZTools::EZString tty,
                  EZTools::EZString ptime, EZTools::EZString cmd, EZTools::EZString basecmd)
                : UID(move(uid)), PID(pid), PPID(ppid), C(c), STIME(move(stime)), TTY(move(tty)),TIME(move(ptime)),
                  CMD(move(cmd)), BASECMD(move(basecmd)) {};

        EZProcess() :
                UID(""), PID(0), PPID(0), C(0), STIME(""), TTY(""), TIME(""), CMD(""), BASECMD("") {};
    };

    class EZProcessList {
    public:
        EZProcessList() {
            EZTools::EZReturn<EZTools::EZString> ps = EZLinux::exec("ps -wfe");
            std::vector<EZTools::EZString> pss = ps.data.split("\n");
            for (unsigned long line = 1; line < pss.size(); line++) {
                while (pss.at(line).regexMatch("  ")) {
                    pss.at(line).regexReplace("  ", " ");
                }
                if (!pss.at(line).empty()) {
                    std::vector<EZTools::EZString> data = pss.at(line).split(" ");
                    EZTools::EZString uid = data.at(0);
                    int pid = data.at(1).asInt();
                    int ppid = data.at(2).asInt();
                    int c = data.at(3).asInt();
                    EZTools::EZString stime = data.at(4);
                    EZTools::EZString tty = data.at(5);
                    EZTools::EZString ptime = data.at(6);
                    std::stringstream ss;
                    for (auto i = 7; i < data.size(); i++) {
                        ss << data.at(i) << " ";
                    }
                    EZTools::EZString cmd = ss.str();
                    if (!cmd.regexMatch("sh -c ps -wfa")) {
                        cmd.regexReplace(" $", "");
                        EZTools::EZString basecmd;
                        if (cmd.regexMatch(" |[/]")) {
                            std::vector<EZTools::EZString> bc = cmd.split(" ");
                            std::vector<EZTools::EZString> sd;
                            if (bc.at(0).regexMatch("^[/]")) {
                                bc.at(0).regexReplace("^[/]", "");
                                sd = bc.at(0).split("/");
                                basecmd = sd.at(sd.size() - 1);
                            } else {
                                basecmd = bc.at(0);
                            }
                        } else {
                            basecmd = cmd;
                        }
                        EZProcess p(uid, pid, ppid, c, stime, tty, ptime, cmd, basecmd);
                        _processes.push_back(p);
                    }
                }
            }
        }
        ~EZProcessList() = default;
        void updateProcessList() {
            EZTools::EZReturn<EZTools::EZString> ps = EZLinux::exec("ps -wfa");
            std::vector<EZTools::EZString> pss = ps.data.split("\n");
            for (auto line = 1; line < pss.size(); line++) {
                while (pss.at(line).regexMatch("  ")) {
                    pss.at(line).regexReplace("  ", " ");
                }
                if (!pss.at(line).empty()) {
                    std::vector<EZTools::EZString> data = pss.at(line).split(" ");
                    EZTools::EZString uid = data.at(0);
                    int pid = data.at(1).asInt();
                    int ppid = data.at(2).asInt();
                    int c = data.at(3).asInt();
                    EZTools::EZString stime = data.at(4);
                    EZTools::EZString tty = data.at(5);
                    EZTools::EZString ptime = data.at(6);
                    std::stringstream ss;
                    for (auto i = 7; i < data.size(); i++) {
                        ss << data.at(i) << " ";
                    }
                    EZTools::EZString cmd = ss.str();
                    if (!cmd.regexMatch("sh -c ps -wfa")) {
                        cmd.regexReplace(" $", "");
                        EZTools::EZString basecmd;
                        if (cmd.regexMatch(" |[/]")) {
                            std::vector<EZTools::EZString> bc = cmd.split(" ");
                            std::vector<EZTools::EZString> sd;
                            if (bc.at(0).regexMatch("^[/]")) {
                                bc.at(0).regexReplace("^[/]", "");
                                sd = bc.at(0).split("/");
                                basecmd = sd.at(sd.size() - 1);
                            } else {
                                basecmd = bc.at(0);
                            }
                        } else {
                            basecmd = cmd;
                        }
                        EZProcess p(uid, pid, ppid, c, stime, tty, ptime, cmd, basecmd);
                        _processes.push_back(p);
                    }
                }
            }
        }
        std::vector<EZProcess> procs() { return _processes; }
        std::vector<EZProcess> uid(EZTools::EZString uid) {
            std::vector<EZProcess> p;
            for (auto & proc : _processes) {
                if (proc.UID == uid) {
                    p.push_back(proc);
                }
            }
            return p;
        }
        EZProcess pid(int pid) {
            EZProcess p;
            for (auto & proc : _processes) {
                if (proc.PID == pid) {
                    return proc;
                }
            }
            return p;
        }
        std::vector<EZProcess> ppid(int ppid) {
            std::vector<EZProcess> p;
            for (auto & proc : _processes) {
                if (proc.PPID == ppid) {
                    p.push_back(proc);
                }
            }
            return p;
        }
        std::vector<EZProcess> baseCmd(EZTools::EZString baseCmd) {
            std::vector<EZProcess> p;
            for (auto & proc : _processes) {
                if (proc.BASECMD == baseCmd) {
                    p.push_back(proc);
                }
            }
            return p;
        }
        std::vector<EZProcess> baseCmdRegexMatch(EZTools::EZString baseCmd) {
            std::vector<EZProcess> p;
            for (auto & proc : _processes) {
                if (proc.BASECMD.regexMatch(baseCmd)) {
                    p.push_back(proc);
                }
            }
            return p;
        }

    private:
        std::vector<EZProcess> _processes;
    };

    //TODO: Add the Process List code
    struct SysLoad {
        float oneMin;
        float fiveMin;
        float fifteenMin;

        SysLoad() : oneMin(0.0), fiveMin(0.0), fifteenMin(0.0) {}
    };

    struct CPUs {
        int processor;
        EZTools::EZString vender_id;
        int cpu_family;
        int model;
        EZTools::EZString model_name;
        int stepping;
        EZTools::EZString microcode;
        float cpu_mhz;
        EZTools::EZString cache_size;
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
        EZTools::EZString flags;
        EZTools::EZString bugs;
        float bogomips;
        int clflush_size;
        int cache_alignment;
        EZTools::EZString address_sizes;
        EZTools::EZString power_management;
        bool hyperthreading;

        CPUs() : processor(0), vender_id(""), cpu_family(0), model(0), model_name(""), stepping(0), microcode(""),
                 cpu_mhz(0.0), cache_size(""), physical_id(0), siblings(0), core_id(0), cpu_cores(0), apicid(0),
                 initial_apicid(0), fpu(false), fpu_exception(false), cpuid_level(0), wp(false), flags(""),
                 bugs(""), bogomips(0.0), clflush_size(0), address_sizes(""), power_management(""),
                 hyperthreading(false), cache_alignment(0) {

        }
    };

    class RAMSize {
    public:
        RAMSize() = default;
        virtual ~RAMSize() = default;
        RAMSize &operator=(int s) {
            _rs = s;
            return *this;
        }
        float asMegs() { return static_cast< float >(_rs) / 1024; }
        float asGigs() { return static_cast< float >(_rs) / 1024 / 1024; }

    private:
        int _rs = 0;
    };

    class MemInfo {
    public:
        MemInfo() {
            std::ifstream inFile;
            inFile.open("/proc/meminfo");
            std::stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZTools::EZString line1 = cpufile.str();
            inFile.close();
            std::vector<EZTools::EZString> lines = line1.split("\n");
            for (auto & line : lines) {
                line = line.regexReplace(" ","");
                line = line.regexReplace("kB", "");
                if (line.regexMatch("^MemTotal:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _total = sp.at(1).asInt();
                } else if (line.regexMatch("^MemFree:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _free = sp.at(1).asInt();
                } else if (line.regexMatch("^MemAvailable:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _available = sp.at(1).asInt();
                } else if (line.regexMatch("^SwapTotal:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _totalSwap = sp.at(1).asInt();
                } else if (line.regexMatch("^SwapFree:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _freeSwap = sp.at(1).asInt();
                }
            }
        }
        virtual ~MemInfo() = default;
        void update() {
            std::ifstream inFile;
            inFile.open("/proc/meminfo");
            std::stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZTools::EZString line1 = cpufile.str();
            inFile.close();
            std::vector<EZTools::EZString> lines = line1.split("\n");
            for (auto & line : lines) {
                line = line.regexReplace(" ", "");
                line = line.regexReplace("kB", "");
                if (line.regexMatch("^MemTotal:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _total = sp.at(1).asInt();
                } else if (line.regexMatch("^MemFree:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _free = sp.at(1).asInt();
                } else if (line.regexMatch("^MemAvailable:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _available = sp.at(1).asInt();
                } else if (line.regexMatch("^SwapTotal:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
                    _totalSwap = sp.at(1).asInt();
                } else if (line.regexMatch("^SwapFree:")) {
                    std::vector<EZTools::EZString> sp = line.split(":");
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

    inline EZTools::EZString kernelVersion() {
        std::ifstream inFile;
        inFile.open("/proc/version");
        std::stringstream versig;
        versig << inFile.rdbuf();
        EZTools::EZString line = versig.str();
        std::vector<EZTools::EZString> sp = line.split(" ");
        return sp.at(2);
    }

    class CPUInfo {
    public:
        CPUInfo() {
            CPUs cpu;
            int counter = 0;
            std::ifstream inFile;
            inFile.open("/proc/cpuinfo");
            std::stringstream cpufile;
            cpufile << inFile.rdbuf();
            EZTools::EZString line1 = cpufile.str();
            inFile.close();
            std::vector<EZTools::EZString> lines = line1.split("\n");
            for (auto &line : lines) {
                if (!line.empty()) {
                    line = line.regexReplace("\t", "");
                    std::vector<EZTools::EZString> sp;
                    if (line.regexMatch(": ")) {
                        sp = line.split(": ");
                    } else {
                        sp = line.split(":");
                    }
                    sp.at(0) = sp.at(0).regexReplace(" ", "_");
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
        std::vector<CPUs> cpu() { return _cpus; }

    private:
        std::vector<CPUs> _cpus;
    };

    inline SysLoad loadavg() {
        SysLoad loadA;
        std::ifstream file("/proc/loadavg");
        if (file.is_open()) {
            EZTools::EZString line;
            while (getline(file, line)) {
                std::vector<EZTools::EZString> la = line.split(" ");
                loadA.oneMin = la.at(0).asFloat();
                loadA.fiveMin = la.at(1).asFloat();
                loadA.fifteenMin = la.at(2).asFloat();
            }
            file.close();
        }
        return loadA;
    }

    inline SysLoad RealLoad() {
        std::ifstream inFile;
        inFile.open("/proc/cpuinfo");
        std::stringstream cpufile;
        cpufile << inFile.rdbuf();
        EZTools::EZString line1 = cpufile.str();
        inFile.close();
        int c = 0;
        std::vector<EZTools::EZString> lines = line1.split("\n");
        for (auto& line : lines) {
            line.regexReplace("\t", "");
            if (line.regexMatch("^processor: ")) {
                c++;
            }
        }
        SysLoad loadA;
        std::ifstream file("/proc/loadavg");
        if (file.is_open()) {
            EZTools::EZString line;
            while (getline(file, line)) {
                std::vector<EZTools::EZString> la = line.split(" ");
                loadA.oneMin = la.at(0).asFloat() / c;
                loadA.fiveMin = la.at(1).asFloat() / c;
                loadA.fifteenMin = la.at(2).asFloat() / c;
            }
            file.close();
        }
        return loadA;
    }
}

#endif //EZT_EZPROC_H
