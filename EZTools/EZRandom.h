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

#ifndef EZT_EZRANDOM_H
#define EZT_EZRANDOM_H

#pragma once

#include <sodium.h>
#include <fstream>
#include "EZTools.h"
#include "nlohmann/json.hpp"

namespace EZRandom {

    inline int init() {
        if (sodium_init() == -1) {
            return 0;
        }
        return 1;
    }

    template <class T>
    inline T urandom() {
        T random_value = 0;
        size_t size = sizeof(random_value);
        std::ifstream ur("/dev/urandom", std::ios::in|std::ios::binary);
        if(ur) {
            ur.read(reinterpret_cast<char*>(&random_value), size);
            ur.close();
        }
        return random_value;
    }

    class EZRand {
    public:
        EZRand() = default;
        ~EZRand() = default;
        int IntNotUniform() { return randombytes_random(); }
        int IntUniform() { return randombytes_uniform(4294967295); }
//        unsigned long int UnsignedLong() { return randombytes_random(); }
//        unsigned long long int UnsignedLongLong() { return randombytes_random(); }
        float Float() { return randombytes_random(); }
        double Double() { return randombytes_random(); }
        unsigned int IntUniform(uint32_t upper) { return randombytes_uniform(upper) + 1; }
        unsigned int IntNotUniform(uint32_t upper) {
            return randombytes_random() % upper;
        }
        char Char() { return char(IntNotUniform(128)); }
        bool Bool() {
            unsigned int i = IntNotUniform(100);
            return i > 50;
        }
    };

    inline EZTools::EZString mkpasswd(int length = 8) {
        std::vector<EZTools::EZString> alpha {"a","b","c","d","e","f","g","h","i","j","k","l","m","n","o","p","q","r",
                                              "s","t","u","v","w","x","y","z"};
        std::vector<EZTools::EZString> special {"!","@","#","$","%","^","&","*","(",")","-","_","=","+","[","]","{","}",
                                                "|","\\","/",",",".","<",">"};
        EZRandom::init();
        EZRandom::EZRand rnd;
        EZTools::EZString pass;
        bool notdone = true;
        while (notdone) {
            std::stringstream ss;
            for (int i = 0; i < length; i++) {
                auto t = rnd.IntUniform(4);
                EZTools::EZString l;
                switch (t) {
                    case 1:
                        l = alpha.at(rnd.IntUniform(alpha.size()) - 1);
                        break;
                    case 2:
                        l = alpha.at(rnd.IntUniform(alpha.size()) - 1);
                        l = l.upper();
                        break;
                    case 3:
                        l = rnd.IntUniform(10) - 1;
                        break;
                    case 4:
                        l = special.at(rnd.IntUniform(special.size()) - 1);
                        break;
                    default:
                        l = "__";
                        break;
                }
                ss << l;
            }
            pass = ss.str();
            if (pass.regexMatch("(?=.*[a-z])(?=.*[A-Z])(?=.*[$&+,:;=?@#|'<>.^*()%!-])")) {
                notdone = false;
            }
        }
        return pass;
    }
};

#endif //EZT_EZRANDOM_H
