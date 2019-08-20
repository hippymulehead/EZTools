//
// Created by mromans on 6/21/19.
//

#include "EZProc.h"

EZProc::EZProcessList::EZProcessList() {
    EZString ps = exec("ps -wfa");
    vector<EZString> pss = ps.split("\n");
    for (auto line = 1; line < pss.size(); line++) {
        while (pss.at(line).regexMatch("  ")) {
            pss.at(line).regexReplace("  ", " ");
        }
        if (!pss.at(line).empty()) {
            vector<EZString> data = pss.at(line).split(" ");
            EZString uid = data.at(0);
            int pid = data.at(1).asInt();
            int ppid = data.at(2).asInt();
            int c = data.at(3).asInt();
            EZString stime = data.at(4);
            EZString tty = data.at(5);
            EZString ptime = data.at(6);
            stringstream ss;
            for (auto i = 7; i < data.size(); i++) {
                ss << data.at(i) << " ";
            }
            EZString cmd = ss.str();
            if (!cmd.regexMatch("sh -c ps -wfa")) {
                cmd.regexReplace(" $", "");
                EZString basecmd;
                if (cmd.regexMatch(" |[/]")) {
                    vector<EZString> bc = cmd.split(" ");
                    vector<EZString> sd;
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

void EZProc::EZProcessList::updateProcessList() {
    EZString ps = exec("ps -wfa");
    vector<EZString> pss = ps.split("\n");
    for (auto line = 1; line < pss.size(); line++) {
        while (pss.at(line).regexMatch("  ")) {
            pss.at(line).regexReplace("  ", " ");
        }
        if (!pss.at(line).empty()) {
            vector<EZString> data = pss.at(line).split(" ");
            EZString uid = data.at(0);
            int pid = data.at(1).asInt();
            int ppid = data.at(2).asInt();
            int c = data.at(3).asInt();
            EZString stime = data.at(4);
            EZString tty = data.at(5);
            EZString ptime = data.at(6);
            stringstream ss;
            for (auto i = 7; i < data.size(); i++) {
                ss << data.at(i) << " ";
            }
            EZString cmd = ss.str();
            if (!cmd.regexMatch("sh -c ps -wfa")) {
                cmd.regexReplace(" $", "");
                EZString basecmd;
                if (cmd.regexMatch(" |[/]")) {
                    vector<EZString> bc = cmd.split(" ");
                    vector<EZString> sd;
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

vector<EZProc::EZProcess> EZProc::EZProcessList::uid(EZString uid) {
    vector<EZProcess> p;
    for (auto & proc : _processes) {
        if (proc.UID == uid) {
            p.push_back(proc);
        }
    }
    return p;
}

EZProc::EZProcess EZProc::EZProcessList::pid(int pid) {
    EZProcess p;
    for (auto & proc : _processes) {
        if (proc.PID == pid) {
            return proc;
        }
    }
    return p;
}

vector<EZProc::EZProcess> EZProc::EZProcessList::ppid(int ppid) {
    vector<EZProcess> p;
    for (auto & proc : _processes) {
        if (proc.PPID == ppid) {
            p.push_back(proc);
        }
    }
    return p;
}

vector<EZProc::EZProcess> EZProc::EZProcessList::baseCmd(EZString baseCmd) {
    vector<EZProcess> p;
    for (auto & proc : _processes) {
        if (proc.BASECMD == baseCmd) {
            p.push_back(proc);
        }
    }
    return p;
}

vector<EZProc::EZProcess> EZProc::EZProcessList::baseCmdRegexMatch(EZString baseCmd) {
    vector<EZProcess> p;
    for (auto & proc : _processes) {
        if (proc.BASECMD.regexMatch(baseCmd)) {
            p.push_back(proc);
        }
    }
    return p;
}