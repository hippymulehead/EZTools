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

#ifndef EZT_EZSPARK_H
#define EZT_EZSPARK_H

#pragma once

#include "EZTools.h"
#include "EZScreen.h"

namespace EZSpark {

    struct SparkGraphSettings {
        EZTools::EZString title;
        int width = 0;
        std::vector<EZTools::EZString> lables;
        EZScreen::RGB base = EZScreen::GREEN;
        EZScreen::RGB mid = EZScreen::YELLOW;
        EZScreen::RGB high = EZScreen::RED;
        int lowSplit = 0;
        int highSplit = 0;
        EZScreen::RGB borderColor = EZScreen::WHITE;
        EZScreen::RGB titleColor = EZScreen::BRIGHT_WHITE;
    };

    class Spark {
    public:
        Spark() = default;
        explicit Spark(std::vector<int> values) {
            for (auto& v : values) {
                if (v > _high) { _high = v; }
                if (v < _low) { _low = v; }
                _values.push_back(v);
            }
        }
        ~Spark() = default;
        std::vector<int> values() { return _values; }
        void values(std::vector<int> values) {
            _values.clear();
            for (auto& v : values) {
                _values.push_back(v);
            }
        }
        EZTools::EZString drawSimpleHorizontal() {
            std::vector<std::string> s = {"▁","▂","▃","▄","▅","▆","▇","█"};
            std::stringstream ss;
            std::vector<int> ranged;
            for (auto& m : _values) {
                int new_value = EZTools::ConvertRange<int,float>(m,_low,_high,0,6);
                ranged.emplace_back(new_value);
            }
            for (auto& me : ranged) {
                ss << s.at(me);
            }
            return ss.str();
        }
        EZTools::EZString drawComplexVerital(SparkGraphSettings data) {
            std::stringstream ss;
            std::vector<int> ranged;
            _w = data.width;
            size_t labWidth = 0;
            size_t valWidth = 0;
            size_t twidth;
            for (auto& v : _values) {
                if (v > _high) { _high = v; }
                if (v < _low) { _low = v; }
            }
            for (auto& l : data.lables) {
                if (l.length() > labWidth) {
                    labWidth = l.length();
                }
            }
            for (auto& ll : _values) {
                EZTools::EZString l = ll;
                if (l.length() > valWidth) {
                    valWidth = l.length();
                }
            }
            twidth = (labWidth + valWidth) - data.title.length() - 1 + data.width + 8;
            for (auto& m : _values) {
                int new_value = EZTools::ConvertRange<int,float>(m,_low,_high,1, data.width);
                ranged.emplace_back(new_value);
            }
            // Top Line
            ss << EZScreen::foreground(data.borderColor) << EZScreen::putChar(EZScreen::SC_UPPER_LEFT)
               << EZScreen::drawHorizontalLineInPlace(labWidth + valWidth + data.width + 8)
               << EZScreen::putChar(EZScreen::SC_UPPER_RIGHT) << std::endl;
            // Title Line
            ss << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << " " << EZScreen::foreground(data.titleColor)
                << data.title
                << rep(twidth, " ")
                << EZScreen::foreground(data.borderColor) << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << std::endl;
            // Below Title
            ss << EZScreen::foreground(data.borderColor) << EZScreen::putChar(EZScreen::SC_LEFT_T)
                << EZScreen::drawHorizontalLineInPlace(labWidth + valWidth + data.width + 8)
                << EZScreen::putChar(EZScreen::SC_RIGHT_T) << std::endl;
            // Data Lines
            for (size_t i = 0; i < data.lables.size(); i++) {
                ss << EZScreen::foreground(data.borderColor) << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << " "
                    << EZScreen::foreground(EZScreen::WHITE) << padRight(data.lables.at(i), labWidth)
                    << EZScreen::foreground(data.borderColor) << " " << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << " "
                    << EZScreen::foreground(EZScreen::WHITE) << padLeft(_values.at(i), valWidth)
                    << EZScreen::foreground(data.borderColor) << " " << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << " "
                    << bar(ranged.at(i), data)
                    << EZScreen::foreground(data.borderColor) << EZScreen::putChar(EZScreen::SC_VERICAL_LINE) << std::endl;
            }
            // Botom Line
            ss << EZScreen::putChar(EZScreen::SC_LOWER_LEFT)
               << EZScreen::drawHorizontalLineInPlace(labWidth + valWidth + data.width + 8)
                    << EZScreen::putChar(EZScreen::SC_LOWER_RIGHT) << std::endl;
//               << EZScreen::putChar(EZScreen::SC_LOWER_RIGHT) << EZScreen::resetReturn() << endl;
            return ss.str();
        }
        EZTools::EZString drawHTML(SparkGraphSettings data) {
            std::stringstream ss;
            int high = 0;
            for (auto& h : _values) {
                if (h > high) {high = h;}
            }
            int low = 999999999;
            for (auto& l : _values) {
                if (l < low) {low = l;}
            }
            EZTools::EZString top = _top;
            top.regexReplace("-TOPTITLE-", data.title);
            ss << top;
            for (int i = 0; i < data.lables.size(); i++) {
                EZTools::EZString e = _element;
                int width = EZTools::ConvertRange<int,float>(_values.at(i), low, high, 1, 100);
                e.regexReplace("-ELEMENTLABLE-", data.lables.at(i));
                e.regexReplace("-ELEMENTVALUE-", _values.at(i));
                std::stringstream eg;
                if (width <= data.lowSplit) {
                    eg << "<div class=\"w3-bar-block w3-green\" style=\"width:" << width << "%\">&nbsp</div>\n";
                } else if (width <= data.highSplit - 2) {
                    eg << "<div class=\"w3-bar-block w3-green\" style=\"width:" << data.lowSplit << "%\">&nbsp</div>\n";
                    eg << "<div class=\"w3-bar-block w3-yellow\" style=\"width:" << width - data.lowSplit << "%\">&nbsp</div>\n";
                } else {
                    eg << "<div class=\"w3-bar-block w3-green\" style=\"width:" << data.lowSplit << "%\">&nbsp</div>\n";
                    int mid = width - data.lowSplit - (100 - data.highSplit) - 2;
                    eg << "<div class=\"w3-bar-block w3-yellow\" style=\"width:" << mid << "%\">&nbsp</div>\n";
                    int hi = 100 - data.lowSplit - mid - 2;
                    eg << "<div class=\"w3-bar-block w3-red\" style=\"width:" << hi << "%\">&nbsp</div>\n";
                }
                e.regexReplace("-ELEMENTGRAPH-", eg.str());
                ss << e;
            }
            ss << _bottom;
            return ss.str();
        }
    private:
        std::vector<int> _values;
        int _high = 0;
        int _low = 999999999;
        int _w = 0;
        EZTools::EZString const _top = "<!DOCTYPE html>\n"
                                       "<html>\n"
                                       "<head>\n"
                                       "<style>\n"
                                       "div {\n"
                                       "\tdisplay:inline-block;\n"
                                       "}\n"
                                       "table.EZRows {\n"
                                       "\tfont-family: \"Times New Roman\", Times, serif;\n"
                                       "\tbackground-color: #FFFFFF;\n"
                                       "\twidth: 80%;\n"
                                       "\ttext-align: right;\n"
                                       "}\n"
                                       "table.EZRows tbody td {\n"
                                       "\tpadding: 2px;\n"
                                       "\tpadding-top: 0px;\n"
                                       "\tpadding-bottom: 0px;\n"
                                       "\tbackground-color: #FFFFFF;\n"
                                       "\tfont-size: 12px;\n"
                                       "\tcolor: #000000;\n"
                                       "}\n"
                                       "table.EZRows thead {\n"
                                       "\tbackground: #FFFFFF;\n"
                                       "}\n"
                                       "table.EZRows thead th {\n"
                                       "\tfont-size: 17px;\n"
                                       "\tfont-weight: bold;\n"
                                       "\tcolor: #000000;\n"
                                       "\ttext-align: left;\n"
                                       "}\n"
                                       "table.EZRows tfoot td {\n"
                                       "\tfont-size: 14px;\n"
                                       "}\n"
                                       ".w3-bar-block .w3-bar-item{display:block;}\n"
                                       ".w3-green,.w3-hover-green:hover{color:#fff!important;background-color:#4CAF50!important}\n"
                                       ".w3-red,.w3-hover-red:hover{color:#fff!important;background-color:#f44336!important}\n"
                                       ".w3-yellow,.w3-hover-yellow:hover{color:#000!important;background-color:#ffeb3b!important}\n"
                                       "</style>\n"
                                       "</head>\n"
                                       "<body style=\"background-color:#FFFFFF;\">\n"
                                       "<table class=\"EZRows\">\n"
                                       "<thead>\n"
                                       "<tr>\n"
                                       "<th width=\"30%\"></th>\n"
                                       "<th width=\"6%\"></th>\n"
                                       "<th>-TOPTITLE-<br>&nbsp</th>\n"
                                       "</tr>\n"
                                       "</thead>\n"
                                       "<tbody>";
        EZTools::EZString const _bottom = "</tbody>\n"
                                          "</tr>\n"
                                          "</table>\n"
                                          "</body>\n"
                                          "</html>";
        EZTools::EZString const _element = "<tr>\n"
                                           "<td align=\"left\">-ELEMENTLABLE-</td>\n"
                                           "<td>-ELEMENTVALUE-</td>\n"
                                           "<td align=\"left\">\n"
                                           "-ELEMENTGRAPH-"
                                           "</td>\n"
                                           "</tr>";
        static std::string rep(int times, EZTools::EZString s) {
            std::stringstream ss;
            for (int i = 0; i < times; i++) {
                ss << s;
            }
            return ss.str();
        }
        static std::string bar(int times, SparkGraphSettings d) {
            std::stringstream ss;
            for (int i = 0; i < times; i++) {
                if (i > d.highSplit) {
                    ss << EZScreen::foreground(d.high);
                } else if (i >= d.lowSplit) {
                    ss << EZScreen::foreground(d.mid);
                } else{
                    ss << EZScreen::foreground(d.base);
                }
                ss << "▇";
            }
            for (int p = 0; p < d.width - times + 1; p++) {
                ss << " ";
            }
//            ss << EZScreen::resetReturn();
            return ss.str();
        }
        static std::string padRight(EZTools::EZString st, size_t w) {
            std::stringstream ss;
            ss << st;
            for (int i = 0; i < w - st.length(); i++) {
                ss << " ";
            }
            return ss.str();
        }
        static std::string padLeft(EZTools::EZString st, size_t w) {
            std::stringstream ss;
            for (int i = 0; i < w - st.length(); i++) {
                ss << " ";
            }
            ss << st;
            return ss.str();
        }
    };

}

#endif //EZT_EZSPARK_H
