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

#ifndef EZT_EZDICE_H
#define EZT_EZDICE_H

#pragma once

#include "EZRandom.h"
#include "EZTools.h"
#include "nlohmann/json.hpp"

namespace EZDice {

    enum Dice_t {
        COIN = 2,
        D4 = 4,
        D6 = 6,
        D8 = 8,
        D10 = 10,
        D12 = 12,
        D20 = 20,
        PERCENT = 100
    };

    enum RND_t {
        UNIFORM,
        NONUNIFORM
    };

    inline RND_t RND_t_asEZString (EZTools::EZString t ) {
        if (t.upper() == "UNIFORM") {
            return UNIFORM;
        } else {
            return RND_t::NONUNIFORM;
        }
    }

    struct Roll_t {
        Dice_t sides = COIN;
        unsigned int total = 0;
    };

    class Dice {
    public:
        explicit Dice(RND_t rollType) {
            _rollType = rollType;
            EZRandom::init();
        }
        ~Dice() = default;
        unsigned int roll(std::vector<Dice_t> dice, int times = 1) {
            unsigned int total = 0;
            for (int i = 0; i < times; i++) {
                for (auto &d: dice) {
                    switch (_rollType) {
                        case UNIFORM:
                            _last.total = _randomSource.IntUniform(d);
                            break;
                        case NONUNIFORM:
                            _last.total = _randomSource.IntNotUniform(d) + 1;
                            break;
                    }
                    _last.sides = d;
                    _history.emplace_back(_last);
                    total = total + _last.total;
                }
            }
            return total;
        }
        EZTools::EZString lastAsCoin() {
            if (_last.sides != COIN) {
                return "Not a coin toss";
            }
            if (_last.total == 1) {
                return "heads";
            } else {
                return "tails";
            }
        }
        std::vector<Roll_t> history() { return _history; }
        nlohmann::json historyAsJSON() {
            nlohmann::json root = nlohmann::json::array();
            int c = 1;
            for (auto& h : _history) {
                nlohmann::json p;
                p["roll"] = c;
                p["sides"] = h.sides;
                p["val"] = h.total;
                root.push_back(p);
                c++;
            }
            return root;
        }
        // historyAsJSON
        Roll_t last() { return _last; }

    private:
        EZRandom::EZRand _randomSource;
        std::vector<Roll_t> _history;
        Roll_t _last;
        RND_t _rollType;
    };

//    inline diceRoll rollDice(Dice_t sides, unsigned int times) {
//        diceRoll dr;
//        dr.sides = sides;
//        dr.times = times;
//        int total = 0;
//        for (int i = 0; i < times; i++) {
//            int t = randomSource.Int(sides);
//            dr.roll.push_back(t);
//            total += t;
//        }
//        dr.total = total;
//    }
//
//    class Dice_Object {
//    public:
//        Dice_Object(EZTools::EZString historyName) {
//            EZRandom::init();
//            _historyName = historyName;
//        }
//        ~Dice_Object() = default;
//        std::vector<diceRoll> history;
//        nlohmann::json historyAsJSON() {
//            nlohmann::json res;
//            res[_historyName] = nlohmann::json::array();
//            int counter = 1;
//            for (auto& d : history) {
//                nlohmann::json dd;
//                dd["id"] = counter;
//                dd["sides"] = d.sides;
//                dd["times"] = d.times;
//                dd["total"] = d.total;
//                dd["roll"] = d.roll;
//                res[_historyName].push_back(dd);
//                counter++;
//            }
//            return res;
//        }
//        diceRoll lastRoll() {
//            return history[history.size() - 1];
//        }
//
//    protected:
//        EZRandom::EZRand randomSource;
//        EZTools::EZString _historyName;
//        int roll(int sides, int times) {
//            diceRoll dr;
//            dr.sides = sides;
//            dr.times = times;
//            int total = 0;
//            for (int i = 0; i < times; i++) {
//                int t = randomSource.Int(sides);
//                dr.roll.push_back(t);
//                total += t;
//            }
//            dr.total = total;
//            history.push_back(dr);
//            return total;
//        }
//    };
//
//    class D20Dice : public Dice_Object {
//    public:
//        D20Dice(EZTools::EZString historyName): Dice_Object(historyName) {}
//        int coin() {
//            int total = randomSource.Int(2);
//            diceRoll dr;
//            dr.total = total;
//            dr.sides = 2;
//            dr.times = 1;
//            dr.roll.push_back(total);
//            history.push_back(dr);
//            return total;
//        }
//        EZTools::EZString coinAsString() {
//            int total = randomSource.Int(2);
//            diceRoll dr;
//            dr.total = total;
//            dr.sides = 2;
//            dr.times = 1;
//            dr.roll.push_back(total);
//            history.push_back(dr);
//            return _coinAsString(total);
//        }
//        int d4(int times = 1) { return roll(4, times); }
//        int d6(int times = 1) { return roll(6, times); }
//        int d8(int times = 1) { return roll(8, times); }
//        int d10(int times = 1) { return roll(10, times); }
//        int dPercentage() {
//            int total = randombytes_uniform(101);
//            diceRoll dr;
//            dr.sides = 100;
//            dr.times = 100;
//            dr.roll.push_back(total);
//            return total;
//        }
//        int d12(int times = 1) { return roll(12, times); }
//        int d20(int times = 1) { return roll(20, times); }
//
//    protected:
//        static EZTools::EZString _coinAsString(int value) {
//            if (value == 1) {
//                return "Heads";
//            } else {
//                return "Tails";
//            }
//        }
//    };
//
//    class D6Dice : public Dice_Object {
//    public:
//        D6Dice(EZTools::EZString historyName): Dice_Object(historyName) {}
//        int rollDice(int times = 1) { return roll(6, times); }
//    };
//
//    class CoinFlip : public Dice_Object {
//    public:
//        CoinFlip(EZTools::EZString historyName): Dice_Object(historyName) {}
//        int asInt() {
//            int total = randomSource.Int(2);
//            diceRoll dr;
//            dr.sides = 2;
//            dr.times = 1;
//            dr.total = total;
//            dr.roll.push_back(total);
//            history.push_back(dr);
//            return total;
//        }
//        EZTools::EZString asString() {
//            int total = randomSource.Int(2);
//            diceRoll dr;
//            dr.total = total;
//            dr.sides = 2;
//            dr.times = 1;
//            dr.roll.push_back(total);
//            history.push_back(dr);
//            return _coinAsString(total);
//        }
//    private:
//        static EZTools::EZString _coinAsString(int value) {
//            if (value == 1) {
//                return "Heads";
//            } else {
//                return "Tails";
//            }
//        }
//    };

    inline EZTools::TEST_RETURN TEST() {
        EZTools::TEST_RETURN res("EZDice");
        std::stringstream ss;
        Dice dice(NONUNIFORM);
        ss << "\tdice.roll({COIN}): " << dice.roll({COIN}) << std::endl;
        ss << "\tdice.lastAsCoin(): " << dice.lastAsCoin() << std::endl;
        ss << "\tdice.roll({D4}): " << dice.roll({D4}) << std::endl;
        ss << "\tdice.roll({D6}): " << dice.roll({D6}) << std::endl;
        ss << "\tdice.roll({D8}): " << dice.roll({D8}) << std::endl;
        ss << "\tdice.roll({D10}): " << dice.roll({D10}) << std::endl;
        ss << "\tdice.roll({D12}): " << dice.roll({D12}) << std::endl;
        ss << "\tdice.roll({D20}): " << dice.roll({D20}) << std::endl;
        ss << "\tdice.roll({PERCENT}): " << dice.roll({PERCENT}) << std::endl;
//        ss << "\tD20Dice.historyAsJSON(): " << std::endl;
//        ss << d.historyAsJSON() << std::endl;
//        D6Dice dd("d6");
//        ss << "\tD6Dice.rollDice(): " << dd.rollDice() << std::endl;
//        ss << "\tD6Dice.historyAsJSON()" << dd.historyAsJSON() << std::endl;
//        CoinFlip f("coin");
//        ss << "\tCoinFlip.asInt(): " << f.asInt() << std::endl;
//        ss << "\tCoinFlip.asString(): " << f.asString() << std::endl;
//        ss << "\tCoinFlip.historyAsJSON(): " << f.historyAsJSON() << std::endl;
        res.output(ss.str());
        res.wasSuccessful(true);
        return res;
    }

}

#endif //EZT_EZDICE_H
