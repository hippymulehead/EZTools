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

#ifndef EZT_EZRANDOM_H
#define EZT_EZRANDOM_H

#include <sodium.h>
#include <fstream>
#include "EZTools.h"

namespace EZRandom {

    int init() {
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
        int Int() { return randombytes_random(); }
        unsigned long int UnsignedLong() { return randombytes_random(); }
        unsigned long long int UnsignedLongLong() { return randombytes_random(); }
        float Float() { return randombytes_random(); }
        double Double() { return randombytes_random(); }
        int Int(uint32_t upper) { return randombytes_uniform(upper) + 1; }
        char Char() { return char(randombytes_uniform(128)); }
        bool Bool() {
            int i = Int(100) + 1;
            return i > 50;
        }
    };

    class EZDice {
    public:
        EZDice() = default;
        ~EZDice() = default;

        int coin() { return _d.Int(2) + 1; }
        EZTools::EZString coinAsString() { return _coinAsString(_d.Int(2) + 1); }
        int d4(int times) { return _roll(4, times); }
        int d6(int times) { return _roll(6, times); }
        int d8(int times) { return _roll(8, times); }
        int d10(int times) { return _roll(10, times); }
        int d12(int times) { return _roll(12, times); }
        int d20(int times) { return _roll(20, times); }

    private:
        EZRand _d;

        int _roll(int sides, int times) {
            int total = 0;
            for (int i = 0; i < times; i++) {
                total += _d.Int(sides) + 1;
            }
            return total;
        }

        static EZTools::EZString _coinAsString(int value) {
            if (value == 1) {
                return "Heads";
            } else {
                return "Tails";
            }
        }
    };
};

#endif //EZT_EZRANDOM_H
