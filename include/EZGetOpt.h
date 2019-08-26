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

#ifndef HELPNEW_EZGETOPT_H
#define HELPNEW_EZGETOPT_H

#include "EZTools.h"

using namespace std;
using namespace EZTools;

namespace EZGetOpt {

    typedef map<char, EZString> EZOpts;

    class GetOpt {
    public:
        GetOpt(int argc, char* argv[], EZString programName, EZString version);
        ~GetOpt() = default;
        EZString programName() { return _programName; }
        void addCommandLineOption(EZString option, EZString description);
        void addCopyright(EZString copyright);
        void addExtraMessage(EZString extraMessage);
        EZOpts options() { return _parsedOptions; }
        bool option(EZString opt);
        EZString argForOption(EZString opt);
        void showHelp();
        void showVersion();
        void parseOptions();

    private:
        EZString _programName;
        map<EZString, EZString> _options;
        EZString _copyright;
        EZString _extraMessage;
        EZString _version;
        vector<EZString> _args;
        EZOpts _opts;
        EZOpts _parsedOptions;
    };

};

#endif //HELPNEW_EZGETOPT_H
