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

#ifndef EZT_EZDICE_H
#define EZT_EZDICE_H

#include "../EZTools/EZRandom.h"
#include "../EZTools/EZTools.h"
#include "../EZTools/json.h"

namespace EZDice {

    struct diceRoll {
        int sides;
        int times;
        std::vector<int> roll;
        int total;
    };

    class Dice_Object {
    public:
        Dice_Object(EZTools::EZString historyName) {
            EZRandom::init();
            _historyName = historyName;
        }
        ~Dice_Object() = default;
        std::vector<diceRoll> history;
        nlohmann::json historyAsJSON() {
            nlohmann::json res;
            res[_historyName] = nlohmann::json::array();
            int counter = 1;
            for (auto& d : history) {
                nlohmann::json dd;
                dd["id"] = counter;
                dd["sides"] = d.sides;
                dd["times"] = d.times;
                dd["total"] = d.total;
                dd["roll"] = d.roll;
                res[_historyName].push_back(dd);
                counter++;
            }
            return res;
        }
        diceRoll lastRoll() {
            return history[history.size() - 1];
        }

    protected:
        EZRandom::EZRand randomSource;
        EZTools::EZString _historyName;
        int roll(int sides, int times) {
            diceRoll dr;
            dr.sides = sides;
            dr.times = times;
            int total = 0;
            for (int i = 0; i < times; i++) {
                int t = randomSource.Int(sides);
                dr.roll.push_back(t);
                total += t;
            }
            dr.total = total;
            history.push_back(dr);
            return total;
        }
    };

    class D20Dice : public Dice_Object {
    public:
        D20Dice(EZTools::EZString historyName): Dice_Object(historyName) {}
        int coin() {
            int total = randomSource.Int(2);
            diceRoll dr;
            dr.total = total;
            dr.sides = 2;
            dr.times = 1;
            dr.roll.push_back(total);
            history.push_back(dr);
            return total;
        }
        EZTools::EZString coinAsString() {
            int total = randomSource.Int(2);
            diceRoll dr;
            dr.total = total;
            dr.sides = 2;
            dr.times = 1;
            dr.roll.push_back(total);
            history.push_back(dr);
            return _coinAsString(total);
        }
        int d4(int times = 1) { return roll(4, times); }
        int d6(int times = 1) { return roll(6, times); }
        int d8(int times = 1) { return roll(8, times); }
        int d10(int times = 1) { return roll(10, times); }
        int dPercentage() {
            int total = randombytes_uniform(101);
            diceRoll dr;
            dr.sides = 100;
            dr.times = 100;
            dr.roll.push_back(total);
            return total;
        }
        int d12(int times = 1) { return roll(12, times); }
        int d20(int times = 1) { return roll(20, times); }

    protected:
        static EZTools::EZString _coinAsString(int value) {
            if (value == 1) {
                return "Heads";
            } else {
                return "Tails";
            }
        }
    };

    class D6Dice : public Dice_Object {
    public:
        D6Dice(EZTools::EZString historyName): Dice_Object(historyName) {}
        int rollDice(int times = 1) { return roll(6, times); }
    };

    class CoinFlip : public Dice_Object {
    public:
        CoinFlip(EZTools::EZString historyName): Dice_Object(historyName) {}
        int asInt() {
            int total = randomSource.Int(2);
            diceRoll dr;
            dr.sides = 2;
            dr.times = 1;
            dr.total = total;
            dr.roll.push_back(total);
            history.push_back(dr);
            return total;
        }
        EZTools::EZString asString() {
            int total = randomSource.Int(2);
            diceRoll dr;
            dr.total = total;
            dr.sides = 2;
            dr.times = 1;
            dr.roll.push_back(total);
            history.push_back(dr);
            return _coinAsString(total);
        }
    private:
        static EZTools::EZString _coinAsString(int value) {
            if (value == 1) {
                return "Heads";
            } else {
                return "Tails";
            }
        }
    };

//    inline EZTools::TEST_RETURN TEST() {
//        EZTools::TEST_RETURN res("EZDice", false);
//        D20Dice d("d20");
//        res.output << "\tD20Dice.coin(): " << d.coin() << std::endl;
//        res.output << "\tD20Dice.coinAsString(): " << d.coinAsString() << std::endl;
//        res.output << "\tD20Dice.d4(): " << d.d4() << std::endl;
//        res.output << "\tD20Dice.d6(): " << d.d6() << std::endl;
//        res.output << "\tD20Dice.d8(): " << d.d8() << std::endl;
//        res.output << "\tD20Dice.d10(): " << d.d10() << std::endl;
//        res.output << "\tD20Dice.d12(): " << d.d12() << std::endl;
//        res.output << "\tD20Dice.d20(): " << d.d20() << std::endl;
//        res.output << "\tD20Dice.dPercentage(): " << d.dPercentage() << std::endl;
//        res.output << "\tD20Dice.historyAsJSON(): " << std::endl;
//        res.output << d.historyAsJSON() << std::endl;
//        D6Dice dd("d6");
//        res.output << "\tD6Dice.rollDice(): " << dd.rollDice() << std::endl;
//        res.output << "\tD6Dice.historyAsJSON()" << dd.historyAsJSON() << std::endl;
//        CoinFlip f("coin");
//        res.output << "\tCoinFlip.asInt(): " << f.asInt() << std::endl;
//        res.output << "\tCoinFlip.asString(): " << f.asString() << std::endl;
//        res.output << "\tCoinFlip.historyAsJSON(): " << f.historyAsJSON() << std::endl;
//        res.wasSuccessful(true);
//        return res;
//    }

}

#endif //EZT_EZDICE_H
