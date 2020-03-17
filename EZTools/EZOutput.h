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

#ifndef EZT_EZOUTPUT_H
#define EZT_EZOUTPUT_H

#include <map>
#include <sys/ioctl.h>
#include <termios.h>
#include <cstdio>
#include <unistd.h>
#include "EZTools.h"
#include "EZLinux.h"

namespace EZOutput {

    namespace EZScreen {

//        class ScreenMap : public std::map<EZTools::EZString, EZTools::EZString*> {
//        public:
//            EZTools::EZString* pointerForTitle(EZTools::EZString title) {
//                if (this->find(title) != this->end()) {
//                    return this->find(title)->second;
//                } else {
//                    return nullptr;
//                }
//            }
//            EZTools::EZString valueForTitle(EZTools::EZString title) {
//                if (this->find(title) != this->end()) {
//                    return this->find(title)->second->str();
//                } else {
//                    return "";
//                }
//            }
//            void addScreen(EZTools::EZString title, EZTools::EZString* pointerToString) {
//                this->insert(std::pair<EZTools::EZString, EZTools::EZString*>(title, pointerToString));
//            }
//        };

        enum _COLOR_T {
            EZ_DEFAULT = 39,
            EZ_BLACK = 30,
            EZ_RED = 31,
            EZ_GREEN = 32,
            EZ_YELLOW = 33,
            EZ_BLUE = 34,
            EZ_MAGENTA = 35,
            EZ_CYAN = 36,
            EZ_WHITE = 37,
            EZ_LAST_COLOR = 200
        };
        enum _CONSOLE_T {
            EZ_RESET = 0,
            EZ_BOLD_ON = 1,
            EZ_BOLD_OFF = 22,
            EZ_FAINT_ON = 2,
            EZ_FAINT_OFF = 22,
            EZ_UNDERLINE_ON = 4,
            EZ_UNDERLINE_OFF = 24,
            EZ_INVERSE_ON = 7,
            EZ_INVERSE_OFF = 27,
            EZ_HIDDEN_ON = 8,
            EZ_HIDDEN_OFF = 28,
            EZ_FAST_BLINK_ON = 6,
            EZ_SLOW_BLINK_ON = 5,
            EZ_BLINK_OFF = 25,
            EZ_LAST_CONSOLE = 200
        };

        enum _SPECIAL_CHAR_T {
            EZ_UNDERLINE_C = 95,
            EZ_DIAMOND_C = 96,
            EZ_SHADED_C = 97,
            EZ_DEGREES_C = 102,
            EZ_PLUSUNDER_C = 103,
            EZ_LOWER_RIGHT_C = 106,
            EZ_UPPER_RIGHT_C = 107,
            EZ_UPPER_LEFT_C = 108,
            EZ_LOWER_LEFT_C = 109,
            EZ_CROSS_C = 110,
            EZ_HORIZONTAL_LINE_TOP_C = 111,
            EZ_HORIZONTAL_LINE_TOP_MIDDLE_C = 112,
            EZ_HORIZONTAL_LINE_C = 113,
            EZ_HORIZONTAL_LINE_BOTTOM_MIDDLE_C = 114,
            EZ_HORIZONTAL_LINE_BOTTOM__C = 115,
            EZ_LEFT_T_C = 116,
            EZ_RIGHT_T_C = 117,
            EZ_BOTTOM_T_C = 118,
            EZ_TOP_T_C = 119,
            EZ_VERICAL_LINE_C = 120,
            EZ_NOT_LESS_THAN_C = 121,
            EZ_NOT_GREATER_THAN_C = 122,
            EZ_PI_C = 123,
            EZ_NOT_EQUAL_TO_C = 124,
            EZ_BRITISH_POUND_C = 125,
            EZ_CENTER_DOT_C = 126
        };

        enum _CC_T {
            INDEXREV = 0x8d,
            INDEX = 0x84
        };

        enum _MENUPOS_T {
            MENU_LEFT,
            MENU_RIGHT
        };

        enum _ERASE_DISPLAY_T {
            EZ_CURRENT_TO_END,
            EZ_CURRENT_TO_BEGINING,
            EZ_CLEAR_ALL,
            EZ_CLEAR_SCROLLBACK
        };

        enum _LINE_T {
            EZ_CURRENT_TO_EOL,
            EZ_CURRENT_TO_BOL,
            EZ_FULL_LINE
        };

        enum _ALIGNMENT_T {
            EZ_RIGHT_A,
            EZ_LEFT_A,
            EZ_CENTER_A
        };

        inline void showCursor(bool show)  {
            if (show) {
                std::cout << "\e[?25h";
            } else {
                std::cout << "\e[?25l";
            }
        }

        inline std::string putChar(EZScreen::_SPECIAL_CHAR_T ch, bool color = true) {
            if (color) {
                std::stringstream ss;
                ss << "\033(0" << char(ch) << "\033(B";
                return ss.str();
            } else {
                switch (ch) {
                    case EZ_UPPER_LEFT_C:
                        return "+";
                    case EZ_UPPER_RIGHT_C:
                        return "+";
                    case EZ_LOWER_LEFT_C:
                        return "+";
                    case EZ_LOWER_RIGHT_C:
                        return "+";
                    case EZ_RIGHT_T_C:
                        return "+";
                    case EZ_LEFT_T_C:
                        return "+";
                    case EZ_TOP_T_C:
                        return "+";
                    case EZ_BOTTOM_T_C:
                        return "+";
                    case EZ_HORIZONTAL_LINE_C:
                        return "-";
                    case EZ_VERICAL_LINE_C:
                        return "|";
                    case EZ_CROSS_C:
                        return "+";
                }
            }
            return std::string();
        }

        inline EZTools::EZString textCentered(EZTools::EZString text, unsigned long width) {
            std::stringstream ss;
            unsigned long side1 = (width / 2) - (text.length() / 2);
            unsigned long side2 = side1;
            for (unsigned long i = 0; i < side1; i++) { ss << " ";}
            ss << text;
            if (ss.str().length() + side2 > width) {
                side2--;
            }
            for (unsigned long i = 0; i < side2; i++) { ss << " ";}
            return ss.str();
        }

        inline EZTools::EZString textLeft(EZTools::EZString text, int width) {
            std::stringstream ss;
            ss << text;
            for (unsigned long i = 0; i < (width - text.length()); i++) {
                ss << " ";
            }
            return ss.str();
        }

        inline EZTools::EZString textRight(EZTools::EZString text, int width) {
            std::stringstream ss;
            for (unsigned long i = 0; i < (width - text.length()); i++) {
                ss << " ";
            }
            ss << text;
            return ss.str();
        }

//        class Window {
//        public:
//            Window(EZTools::EZString title, int minX, int minY, int maxX, int maxY);
//            ~Window() {
//                free(_base);
//            };
//        private:
//            EZTools::EZString _title;
//            int _minX, _minY, _maxX, _maxY;
//            int _vsize;
//            std::vector<char> *_base;
//            int _cursonPoint = 0;
//        };

        class ScreenOutput : public std::ostream {
        public:
            ScreenOutput() : std::ostream(std::cout.rdbuf()) {
                updateSizeInternal();
                _term = EZLinux::environmentVar("TERM");
                checkColors();
            }
            void color(bool showColor) { _color = showColor; }
            int maxX() { return _max_x; }
            int maxY() { return _max_y; }
            int minX() { return _min_x; }
            int minY() { return _min_y; }
            bool colorSupport() { return _color; }
            void updateSize() { updateSizeInternal(); }
//            void setCaptureString(EZTools::EZString *captureString) {
//                _capString = captureString;
//                _cap = true;
//            }
//            void captureOff() { _cap = false; }
//            void appendToCapture(EZTools::EZString cap) { _capString->append(cap); }
//            bool isCaptured() { return _cap; }
            void clrscr() {
                std::stringstream ss;
                ss << "\033[2J\033[1;1H";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void setColor(_COLOR_T foreground, _COLOR_T background = EZ_LAST_COLOR) {
                std::stringstream ss;
                if (_color) {
                    foreground == EZ_LAST_COLOR ? (foreground = _lastFG) : (_lastFG = foreground);
                    background == EZ_LAST_COLOR ? (background = _lastBG) : (_lastBG = background);
                    ss << "\033[" << foreground << "m\033[" << background + 10 << "m";
                }
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void reset() {
                if (_color) {
                    setConsole(EZ_RESET);
                    setColor(EZ_DEFAULT, EZ_DEFAULT);
                }
            }
            void setLast() {
                if (_color) {
                    setColor(EZ_LAST_COLOR, EZ_LAST_COLOR);
                    setConsole(EZ_LAST_CONSOLE);
                }
            }
            void setConsole(_CONSOLE_T console) {
                std::stringstream ss;
                if (_color) {
                    console == EZ_LAST_CONSOLE ? (console = _lastCON) : (_lastCON = console);
                    ss << "\033[" << console << "m";
                }
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void test(int ccode) {
                std::stringstream ss;
                if (_color) {
                    ss << "\033[" << ccode << "m";
                }
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorMoveToXY(int x, int y) {
                std::stringstream ss;
                ss << "\033[" << y << ";" << x << "H";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorMoveUp(int lines) {
                std::stringstream ss;
                ss << "\033[" << lines << "A";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorMoveDown(int lines) {
                std::stringstream ss;
                ss << "\033[" << lines << "B";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorMoveForward(int chars) {
                std::stringstream ss;
                ss << "\033[" << chars << "C";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorMoveBackwards(int chars) {
                std::stringstream ss;
                ss << "\033[" << chars << "D";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorSavePos() {
                std::stringstream ss;
                ss << "\033[s";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorRestorePos() {
                std::stringstream ss;
                ss << "\033[u";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void cursorOff() {
                std::cout << "\e[?25l";
            }
            void cursorOn() {
                std::cout << "\e[?25h";
            }
            void eraseDisplay(_ERASE_DISPLAY_T eType) {
                EZTools::EZString ch = "";
                switch(eType) {
                    case EZ_CURRENT_TO_END:
                        ch = "0";
                        break;
                    case EZ_CURRENT_TO_BEGINING:
                        ch = "1";
                        break;
                    case EZ_CLEAR_ALL:
                        ch = "2";
                        break;
                    case EZ_CLEAR_SCROLLBACK:
                        ch = "3";
                        break;
                }
                std::stringstream ss;
                ss << "\033[" << ch << "J";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void eraseLine(_LINE_T line) {
                EZTools::EZString ch = "";
                switch(line) {
                    case EZ_CURRENT_TO_EOL:
                        ch = "0";
                        break;
                    case EZ_CURRENT_TO_BOL:
                        ch = "1";
                        break;
                    case EZ_FULL_LINE:
                        ch = "2";
                        break;
                }
                std::stringstream ss;
                ss << "\033[" << ch << "K";
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
            }
            void drawHorizontalLine(int xLeft, int xRight, int y) {
                std::stringstream ss;
                if (_cap) {
                    for (auto i = xLeft; i <= xRight; i++) {
                        cursorMoveToXY(i, y);
                        _capString->append(putChar(EZ_HORIZONTAL_LINE_C, _color));
                    }
                } else {
                    for (auto i = xLeft; i <= xRight; i++) {
                        cursorMoveToXY(i, y);
                        std::cout << putChar(EZ_HORIZONTAL_LINE_C, _color);
                    }
                }
            }
            void drawHorizontalLineInPlace(int width) {
                std::stringstream ss;
                    for (auto i = 0; i < width; i++) {
                        ss << putChar(EZ_HORIZONTAL_LINE_C, _color);
                    }
                    if (_cap) {
                        _capString->append(ss.str());
                    } else {
                        std::cout << ss.str();
                    }
            }
            void drawHorizontalLineInPlaceWithTitle(int width, EZTools::EZString title,
                                                    _COLOR_T lineColor, _COLOR_T textColor, _ALIGNMENT_T alignment) {
                int tlen = title.length();
                int rem = width - tlen - 4;
                int side1 = 0;
                int side2 = 0;
                switch(alignment) {
                    case EZ_CENTER_A:
                        side1 = rem / 2;
                        side2 = side1;
                        if ((rem % 2) == 1) {
                            side1 = (rem - 1) / 2;
                            side2 = rem / 2 + 1;
                        }
                        break;
                    case EZ_LEFT_A:
                        side1 = 1;
                        side2 = rem - 1;
                        break;
                    case EZ_RIGHT_A:
                        side1 = rem - 1;
                        side2 = 1;
                        break;
                }
                setColor(lineColor);
                drawHorizontalLineInPlace(side1);
                setColor(textColor);
                std::stringstream ss;
                ss << "[ " << title << " ]";
                setConsole(EZ_BOLD_ON);
                if (_cap) {
                    _capString->append(ss.str());
                } else {
                    std::cout << ss.str();
                }
                setConsole(EZ_BOLD_OFF);
                setColor(lineColor);
                drawHorizontalLineInPlace(side2);
            }
            void drawVertical(int x, int yTop, int yBottom) {
                if (_cap) {
                    for (auto i = yTop; i <= yBottom; i++) {
                        cursorMoveToXY(x, i);
                        _capString->append(putChar(EZ_VERICAL_LINE_C, _color));
                    }
                } else {
                    for (auto i = yTop; i <= yBottom; i++) {
                        cursorMoveToXY(x, i);
                        std::cout << putChar(EZ_VERICAL_LINE_C, _color);
                    }
                }
            }
            void drawBox(int leftX, int leftY, int rightX, int rightY, bool fill = false) {
                if (_cap) {
                    cursorMoveToXY(leftX, leftY);
                    _capString->append(putChar(EZ_UPPER_LEFT_C, _color));
                    drawHorizontalLine(leftX + 1, rightX - 1, leftY);
                    _capString->append(putChar(EZ_UPPER_RIGHT_C, _color));
                    cursorMoveToXY(leftX, rightY);
                    _capString->append(putChar(EZ_LOWER_LEFT_C, _color));
                    drawHorizontalLine(leftX + 1, rightX - 1, rightY);
                    _capString->append(putChar(EZ_LOWER_RIGHT_C, _color));
                    drawVertical(leftX, leftY + 1, rightY - 1);
                    drawVertical(rightX, leftY + 1, rightY - 1);
                    if (fill) {
                        for (auto y = leftY + 1; y < rightY; y++) {
                            for (auto x = leftX + 1; x < rightX; x++) {
                                cursorMoveToXY(x, y);
                                _capString->append(" ");
                            }
                        }
                    }
                } else {
                    cursorMoveToXY(leftX, leftY);
                    std::cout << putChar(EZ_UPPER_LEFT_C, _color);
                    drawHorizontalLine(leftX + 1, rightX - 1, leftY);
                    std::cout << putChar(EZ_UPPER_RIGHT_C, _color);
                    cursorMoveToXY(leftX, rightY);
                    std::cout << putChar(EZ_LOWER_LEFT_C, _color);
                    drawHorizontalLine(leftX + 1, rightX - 1, rightY);
                    std::cout << putChar(EZ_LOWER_RIGHT_C, _color);
                    drawVertical(leftX, leftY + 1, rightY - 1);
                    drawVertical(rightX, leftY + 1, rightY - 1);
                    if (fill) {
                        for (auto y = leftY + 1; y < rightY; y++) {
                            for (auto x = leftX + 1; x < rightX; x++) {
                                cursorMoveToXY(x, y);
                                std::cout << " ";
                            }
                        }
                    }
                }
            }
            void drawBoxWithTitle(EZTools::EZString title, _ALIGNMENT_T alignment, _COLOR_T lineColor,
                                  _COLOR_T titleColor, int leftX, int leftY, int rightX, int rightY, bool fill = false) {
                if (_cap) {
                    setColor(lineColor);
                    cursorMoveToXY(leftX, leftY);
                    _capString->append(putChar(EZ_UPPER_LEFT_C, _color));
                    int wid = (rightX) - (leftX + 1);
                    drawHorizontalLineInPlaceWithTitle(wid, title,lineColor, titleColor,
                                                       EZ_LEFT_A);
                    setColor(lineColor);
                    _capString->append(putChar(EZ_UPPER_RIGHT_C, _color));
                    cursorMoveToXY(leftX, rightY);
                    _capString->append(putChar(EZ_LOWER_LEFT_C, _color));
                    drawHorizontalLine(leftX + 1, rightX - 1, rightY);
                    _capString->append(putChar(EZ_LOWER_RIGHT_C, _color));
                    drawVertical(leftX, leftY + 1, rightY - 1);
                    drawVertical(rightX, leftY + 1, rightY - 1);
                    if (fill) {
                        for (auto y = leftY + 1; y < rightY; y++) {
                            for (auto x = leftX + 1; x < rightX; x++) {
                                cursorMoveToXY(x, y);
                                _capString->append(" ");
                            }
                        }
                    }
                } else {
                    setColor(lineColor);
                    cursorMoveToXY(leftX, leftY);
                    std::cout << putChar(EZ_UPPER_LEFT_C, _color);
                    int wid = (rightX) - (leftX + 1);
                    drawHorizontalLineInPlaceWithTitle(wid, title,lineColor, titleColor,
                                                       EZ_LEFT_A);
                    setColor(lineColor);
                    std::cout << putChar(EZ_UPPER_RIGHT_C, _color);
                    cursorMoveToXY(leftX, rightY);
                    std::cout << putChar(EZ_LOWER_LEFT_C, _color);
                    drawHorizontalLine(leftX + 1, rightX - 1, rightY);
                    std::cout << putChar(EZ_LOWER_RIGHT_C, _color);
                    drawVertical(leftX, leftY + 1, rightY - 1);
                    drawVertical(rightX, leftY + 1, rightY - 1);
                    if (fill) {
                        for (auto y = leftY + 1; y < rightY; y++) {
                            for (auto x = leftX + 1; x < rightX; x++) {
                                cursorMoveToXY(x, y);
                                std::cout << " ";
                            }
                        }
                    }
                }
            }
            void getMouseClick() {
                setConsole(EZ_INVERSE_ON);
                cursorMoveToXY(20, 10);
                std::cout << " Yes ";
                cursorMoveToXY(40,10);
                std::cout << " No ";
                unsigned char buff [6];
                unsigned int x, y, btn;
                struct termios original, raw;
                // Save original serial communication configuration for stdin
                tcgetattr( STDIN_FILENO, &original);
                // Put stdin in raw mode so keys get through directly without
                // requiring pressing enter.
                cfmakeraw (&raw);
                tcsetattr (STDIN_FILENO, TCSANOW, &raw);
                // Switch to the alternate buffer screen
                ::write (STDOUT_FILENO, "\e[?47h", 6);

                // Enable mouse tracking
                std::cout <<
                          ::write (STDOUT_FILENO, "\e[?9h", 5);
                bool done = false;
                while (!done) {
                    read (STDIN_FILENO, &buff, 1);
                    if (buff[0] == 3) {
                        // User pressd Ctr+C
                        break;
                    } else if (buff[0] == '\x1B') {
                        // We assume all escape sequences received
                        // are mouse coordinates
                        read (STDIN_FILENO, &buff, 5);
                        btn = buff[2] - 32;
                        x = buff[3] - 32;
                        y = buff[4] - 32;
                        if (btn == 0 && y == 10) {
                            if (x >= 20 && x <= 25) {
                                cursorMoveToXY(1,20);
                                reset();
                                std::cout << "YES" << std::endl;
                                done = true;
                            } else if (x >= 40 && x <= 44) {
                                cursorMoveToXY(1,20);
                                reset();
                                std::cout << "YES" << std::endl;
                                done = true;
                            }
                        }
                    }
                }

                // Revert the terminal back to its original state
                ::write (STDOUT_FILENO, "\033[?9l", 5);
                ::write (STDOUT_FILENO, "\033[?47l", 6);
                tcsetattr (STDIN_FILENO, TCSANOW, &original);
            }
            EZTools::EZString dialogAB(_COLOR_T lineColor, _COLOR_T titleColor, _COLOR_T buttonTextColor,
                                       EZTools::EZString title, EZTools::EZString lButton, EZTools::EZString rButton,
                                       EZTools::EZString lValue, EZTools::EZString rValue,
                                       std::vector<EZTools::EZString> text = std::vector<EZTools::EZString>()) {
                int tsize = text.size();
                int lX = (maxX() - 40) / 2;
                drawBoxWithTitle(title, EZ_LEFT_A, lineColor, titleColor, lX,
                                 6, lX + 40, 9 + tsize, true);
                int textY = 7;
                reset();
                for (auto& t : text) {
                    cursorMoveToXY(lX + 2, textY);
                    std::cout << textCentered(t, 36);
                    textY++;
                }
                int tSize = lButton.length() + rButton.length() + 12;
                int tSide1 = lX + (20 -(tSize / 2));
                std::stringstream leftBut;
                leftBut << "[ " << lButton << " ]";
                std::stringstream righhtBut;
                righhtBut << "[ " << rButton << " ]";

                unsigned char buff [6];
                unsigned int x, y, btn;
                struct termios original, raw;
                tcgetattr( STDIN_FILENO, &original);
                cfmakeraw (&raw);
                tcsetattr (STDIN_FILENO, TCSANOW, &raw);

                bool done = false;
                int yn = 0;
                _COLOR_T bg1 = EZ_BLUE;
                _COLOR_T bg2 = EZ_BLACK;
                EZTools::EZString lastChar = "D";
                while (!done) {
                    switch(yn) {
                        case 0:
                            bg1 = titleColor;
                            bg2 = EZ_BLACK;
                            break;
                        case 1:
                            bg1 = EZ_BLACK;
                            bg2 = titleColor;
                            break;
                    }
                    cursorMoveToXY(tSide1, 8 + tsize);
                    setColor(buttonTextColor, bg1);
                    std::cout << leftBut.str();
                    reset();
                    std::cout << "    ";
                    setColor(buttonTextColor, bg2);
                    std::cout << righhtBut.str();
                    setColor(EZ_DEFAULT, EZ_DEFAULT);
                    reset();
                    setConsole(EZ_HIDDEN_ON);
                    showCursor(false);
                    char a = getchar();
                    std::stringstream ss;
                    ss << a;
                    EZTools::EZString s = ss.str();
                    s.lower();
                    int cval = static_cast<int>(s[0]);
                    if (cval == 13) {
                        done = true;
                    } else if (s == lValue.lower()) {
                        lastChar = "D";
                        done = true;
                    } else if (s == rValue.lower()) {
                        lastChar = "C";
                        done = true;
                    } else if (cval == 91) {
                        setConsole(EZ_HIDDEN_ON);
                        char c = getchar();
                        setConsole(EZ_HIDDEN_OFF);
                        std::stringstream sc;
                        sc << c;
                        int cval2 = static_cast<int>(sc.str()[0]);
                        cursorMoveToXY(1,16);
                        if (cval2 == 68) {
                            if (lastChar != sc.str()) {
                                yn = !yn;
                            }
                            lastChar = sc.str();
                        } else if (cval2 == 67) {
                            if (lastChar != sc.str()) {
                                yn = !yn;
                            }
                            lastChar = sc.str();
                        }
                    }
                }
                setConsole(EZ_HIDDEN_OFF);
                showCursor(true);
                tcsetattr(STDIN_FILENO, TCSANOW, &original);
                if (lastChar == "D") {
                    return lValue.upper();
                } else {
                    return rValue.upper();
                }
            }

        private:
            int _max_x = 0;
            int _max_y = 0;
            int _min_x = 0;
            int _min_y = 0;
            _CONSOLE_T _lastCON = EZ_RESET;
            _COLOR_T _lastFG = EZ_DEFAULT;
            _COLOR_T _lastBG = EZ_DEFAULT;
            EZTools::EZString _term;
            bool _color = false;
            EZTools::EZString* _capString;
            bool _cap = false;

            void updateSizeInternal() {
                winsize size;
                ioctl(0, TIOCGWINSZ, &size);
                _max_x = size.ws_col;
                _max_y = size.ws_row;
                _min_x = 1;
                _min_y = 1;
            }

            void checkColors() {
                EZTools::EZReturn<EZTools::EZString> t = EZLinux::exec("/usr/bin/tput colors");
                t.data.chomp();
                int i = 0;
                if (!t.data.empty() & t.data.isInt()) {
                    i = t.data.asInt();
                }
                _color = i != 0;
            }

        };

        template <typename T>
        ScreenOutput& operator<<(ScreenOutput& myo, const T& v) {
            static_cast<std::ostream &>(myo) << v;
            return myo;
        }

    }

}

#endif //EZT_EZOUTPUT_H
