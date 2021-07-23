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

#ifndef EZT_EZSCREEN_H
#define EZT_EZSCREEN_H

#include <map>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <unistd.h>
#include "EZTools.h"
#include "EZLinux.h"
#include "EZHTTP.h"

namespace EZScreen {

    // Cursor movement
#define CUR_UP "\u001b[{n}A"
#define CUR_DOWN "\u001b[1B"
#define CUR_LEFT "\u001b[1D"
#define CUR_RIGHT "\u001b[1C"

    // Console Commands
#define CON_RESET "\u001b[0m"
#define CON_BOLD_ON "\u001b[1m"
#define CON_BOLD_OFF "\u001b[22m"
#define CON_INVERSE_ON "\u001b[7m"
#define CON_INVERSE_OFF "\u001b[27m"
#define CON_DIM_ON "\u001b[2m"
#define CON_DIM_OFF "\u001b[22m"
#define CON_UNDERLINE_ON "\u001b[4m"
#define CON_UNDERLINE_OFF "\u001b[24m"
#define CON_HIDDEN_ON "\u001b[8m"
#define CON_HIDDEN_OFF "\u001b[28m"
#define CON_BLIBK_ON "\u001b[6m"
#define CON_BLINK_OFF "\u001b[25m"
#define CON_CURSOR_ON "\e[?25h"
#define CON_CURSOR_OFF "\e[?25l"
#define CON_BELL "\07"

    struct RGB {
        int red;
        int green;
        int blue;
        RGB(int Red, int Green, int Blue) {
            red = Red;
            green = Green;
            blue = Blue;
        }
    };

    const RGB RED(190,0,0);
    const RGB BRIGHT_RED(255,0,0);
    const RGB GREEN(0,190,0);
    const RGB BRIGHT_GREEN(0,255,0);
    const RGB BLUE(0,0,190);
    const RGB BRIGHT_BLUE(0,0,255);
    const RGB YELLOW(190,190,0);
    const RGB BRIGHT_YELLOW(255,255,0);
    const RGB CYAN(0,190,190);
    const RGB BRIGHT_CYAN(0,255,255);
    const RGB MAGENTA(190,0,190);
    const RGB BRIGHT_MAGENTA(255,0,255);
    const RGB BLACK(0,0,0);
    const RGB BRIGHT_BLACK(80,80,80);
    const RGB WHITE(190,190,190);
    const RGB BRIGHT_WHITE(255,255,255);
    const RGB ORANGE(190,130,0);
    const RGB BRIGHT_ORANGE(255,165,0);
    const RGB LIMEGREEN(90,190,0);
    const RGB BRIGHT_LIMEGREEN(160,255,0);
    const RGB HOTPINK(190,0,140);
    const RGB BRIGHT_HOTPINK(255,0,140);

    bool color = false;
    int maxX = 0;
    int maxY = 0;
    int minX = 1;
    int minY = 1;
    EZTools::EZString term;

    RGB _foreground_l(170,170,170);
    RGB _background_l(0,0,0);

    enum ALIGNMENT {
        RIGHT,
        LEFT,
        CENTER,
        TOP,
        BOTTOM
    };

    enum SPECIAL_CHAR {
        SC_UNDERLINE = 95,
        SC_DIAMOND = 96,
        SC_SHADED = 97,
        SC_DEGREES = 102,
        SC_PLUSUNDER = 103,
        SC_LOWER_RIGHT = 106,
        SC_UPPER_RIGHT = 107,
        SC_UPPER_LEFT = 108,
        SC_LOWER_LEFT = 109,
        SC_CROSS = 110,
        SC_HORIZONTAL_LINE_TOP = 111,
        SC_HORIZONTAL_LINE_TOP_MIDDLE = 112,
        SC_HORIZONTAL_LINE = 113,
        SC_HORIZONTAL_LINE_BOTTOM_MIDDLE = 114,
        SC_HORIZONTAL_LINE_BOTTOM = 115,
        SC_LEFT_T = 116,
        SC_RIGHT_T = 117,
        SC_BOTTOM_T = 118,
        SC_TOP_T = 119,
        SC_VERICAL_LINE = 120,
        SC_NOT_LESS_THAN = 121,
        SC_NOT_GREATER_THAN = 122,
        SC_PI = 123,
        SC_NOT_EQUAL_TO = 124,
        SC_BRITISH_POUND = 125,
        SC_CENTER_DOT = 126
    };

    inline void clrscr() {
        if (EZScreen::color) {
            std::stringstream ss;
            ss << "\033[2J\033[1;1H";
            std::cout << ss.str();
        } else {
            EZLinux::exec("/usr/bin/clear");
        }
    }

    inline EZTools::EZString foreground(RGB Color) {
        if (EZScreen::color) {
            _foreground_l = Color;
            std::stringstream ss;
            ss << "\033[38;2;" << Color.red << ";" << Color.green << ";" << Color.blue << "m";
            return ss.str();
        } else return "";
    }

    inline EZTools::EZString background(RGB Color) {
        if (EZScreen::color) {
            _background_l = Color;
            std::stringstream ss;
            ss << "\033[48;2;" << Color.red << ";" << Color.green << ";" << Color.blue << "m";
            return ss.str();
        } else return "";
    }

    inline std::string putChar(EZScreen::SPECIAL_CHAR ch, bool color = true) {
        if (color) {
            std::stringstream ss;
            ss << "\033(0" << char(ch) << "\033(B";
            return ss.str();
        } else {
            switch (ch) {
                case SC_UPPER_LEFT:
                    return "+";
                case SC_UPPER_RIGHT:
                    return "+";
                case SC_LOWER_LEFT:
                    return "+";
                case SC_LOWER_RIGHT:
                    return "+";
                case SC_RIGHT_T:
                    return "+";
                case SC_LEFT_T:
                    return "+";
                case SC_TOP_T:
                    return "+";
                case SC_BOTTOM_T:
                    return "+";
                case SC_HORIZONTAL_LINE:
                    return "-";
                case SC_VERICAL_LINE:
                    return "|";
                case SC_CROSS:
                    return "+";
            }
        }
        return std::string();
    }

    inline EZTools::EZString textCentered(EZTools::EZString text, unsigned long width) {
        std::stringstream ss;
        unsigned long side1 = (width / 2) - (text.length() / 2);
        unsigned long side2 = side1;
        for (unsigned long i = 0; i < side1; i++) {
            ss << " ";
        }
        ss << text;
        int lo = width - (side1 + text.length() + side2);
        side2 = side2 + lo;
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

    inline EZTools::EZString cursorXY(int x, int y) {
        std::stringstream ss;
        ss << "\033[" << y << ";" << x << "H";
        return ss.str();
    }

    inline EZTools::EZString cursorMoveUp(int lines) {
        std::stringstream ss;
        ss << "\033[" << lines << "A";
        return ss.str();
    }
    inline EZTools::EZString cursorMoveDown(int lines) {
        std::stringstream ss;
        ss << "\033[" << lines << "B";
        return ss.str();
    }
    inline EZTools::EZString cursorMoveForward(int chars) {
        std::stringstream ss;
        ss << "\033[" << chars << "C";
        return ss.str();
    }
    inline EZTools::EZString cursorMoveBackwards(int chars) {
        std::stringstream ss;
        ss << "\033[" << chars << "D";
        return ss.str();
    }
    inline EZTools::EZString cursorSavePos() {
        std::stringstream ss;
        ss << "\033[s";
        return ss.str();
    }
    inline EZTools::EZString cursorRestorePos() {
        std::stringstream ss;
        ss << "\033[u";
        return ss.str();
    }
    inline EZTools::EZString cursorOff() {
        return "\e[?25l";
    }
    inline EZTools::EZString cursorOn() {
        return "\e[?25h";
    }

    inline EZTools::EZString drawHorizontalLine(int xLeft, int xRight, int y) {
        std::stringstream ss;
        for (auto i = xLeft; i <= xRight; i++) {
            ss << cursorXY(i, y) << putChar(SC_HORIZONTAL_LINE, EZScreen::color);
        }
        return ss.str();
    }
    inline EZTools::EZString drawHorizontalLineInPlace(int width) {
        std::stringstream ss;
        for (auto i = 0; i < width; i++) {
            ss << putChar(SC_HORIZONTAL_LINE, EZScreen::color);
        }
//        ss << CON_RESET;
        return ss.str();
    }

    inline EZTools::EZString drawHorizontalLineInPlaceWithTitle(int width, EZTools::EZString title,
                                            RGB lineColor, RGB textColor, ALIGNMENT alignment) {
        int tlen = title.length();
        int rem = width - tlen - 4;
        int side1 = 0;
        int side2 = 0;
        switch(alignment) {
            case CENTER:
                side1 = rem / 2;
                side2 = side1;
                if ((rem % 2) == 1) {
                    side1 = (rem - 1) / 2;
                    side2 = rem / 2 + 1;
                }
                break;
            case LEFT:
                side1 = 1;
                side2 = rem - 1;
                break;
            case RIGHT:
                side1 = rem - 1;
                side2 = 1;
                break;
        }
        std::stringstream ss;
        ss << foreground(lineColor);
        ss << drawHorizontalLineInPlace(side1);
        ss << foreground(textColor);
        ss << CON_BOLD_ON;
        ss << "[ " << title << " ]";
        ss << CON_BOLD_OFF;
        ss << foreground(lineColor);
        ss << drawHorizontalLineInPlace(side2);
//        ss << CON_RESET;
        return ss.str();
    }

    inline EZTools::EZString drawVerticalLine(int x, int yTop, int yBottom) {
        std::stringstream ss;
        for (auto i = yTop; i <= yBottom; i++) {
            ss << cursorXY(x, i);
            ss << putChar(SC_VERICAL_LINE, EZScreen::color);
        }
        return ss.str();
    }

    inline EZTools::EZString drawBox(int leftX, int leftY, int rightX, int rightY, bool fill = false) {
        std::stringstream ss;
        ss << cursorXY(leftX, leftY);
        ss << putChar(SC_UPPER_LEFT, EZScreen::color);
        ss << drawHorizontalLine(leftX + 1, rightX - 1, leftY);
        ss << putChar(SC_UPPER_RIGHT, EZScreen::color);
        ss << cursorXY(leftX, rightY);
        ss << putChar(SC_LOWER_LEFT, EZScreen::color);
        ss << drawHorizontalLine(leftX + 1, rightX - 1, rightY);
        ss << putChar(SC_LOWER_RIGHT, EZScreen::color);
        ss << drawVerticalLine(leftX, leftY + 1, rightY - 1);
        ss << drawVerticalLine(rightX, leftY + 1, rightY - 1);
        if (fill) {
            for (auto y = leftY + 1; y < rightY; y++) {
                for (auto x = leftX + 1; x < rightX; x++) {
                    ss << cursorXY(x, y);
                    ss << " ";
                }
            }
        }
        return ss.str();
    }

    inline EZTools::EZString drawBoxWithTitle(EZTools::EZString title, ALIGNMENT alignment, RGB lineColor,
                          RGB titleColor, int leftX, int leftY, int rightX, int rightY,
                          bool fill = false) {
        std::stringstream ss;
        ss << foreground(lineColor);
        ss << cursorXY(leftX, leftY);
        ss << putChar(SC_UPPER_LEFT, EZScreen::color);
        int wid = (rightX) - (leftX + 1);
        ss << drawHorizontalLineInPlaceWithTitle(wid, title,lineColor, titleColor,
                                           alignment);
        ss << foreground(lineColor);
        ss << putChar(SC_UPPER_RIGHT, EZScreen::color);
        ss << cursorXY(leftX, rightY);
        ss << putChar(SC_LOWER_LEFT, EZScreen::color);
        ss << drawHorizontalLine(leftX + 1, rightX - 1, rightY);
        ss << putChar(SC_LOWER_RIGHT, EZScreen::color);
        ss << drawVerticalLine(leftX, leftY + 1, rightY - 1);
        ss << drawVerticalLine(rightX, leftY + 1, rightY - 1);
        if (fill) {
            for (auto y = leftY + 1; y < rightY; y++) {
                for (auto x = leftX + 1; x < rightX; x++) {
                    ss << cursorXY(x, y);
                    ss << " ";
                }
            }
        }
        return ss.str();
    }

    inline void initScreen() {
        winsize size;
        ioctl(0, TIOCGWINSZ, &size);
        EZScreen::maxX = size.ws_col;
        EZScreen::maxY = size.ws_row;
        EZScreen::minX = 1;
        EZScreen::minY = 1;
        EZScreen::term = EZLinux::environmentVar("TERM");
        if (!EZScreen::term.empty()) {
            EZTools::EZReturn<EZTools::EZString> t = EZLinux::exec("/usr/bin/tput colors");
            t.data.chomp();
            int i = 0;
            if (!t.data.empty() & t.data.isInt()) {
                i = t.data.asInt();
            }
            EZScreen::color = i != 0;
        } else {
            EZScreen::color = false;
        }
    }

    struct TerminalReadLine_t {
        EZTools::EZString data;
        bool tabForwards = false;
        bool tabBackards = false;
        bool wasSuccessful = false;
        EZTools::EZString message;
    };

    class TerminalInput {
    public:
        TerminalInput() {
            tcgetattr(STDIN_FILENO, &oldt);
        }
        ~TerminalInput() = default;
        void echoOn() {
            tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        }
        void echoOff() {
            termios newt = oldt;
            newt.c_lflag &= ~ECHO;
            tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        }
        void setTabHandling(bool tabForward = false, bool tabBackwards = false) {
            _tabForward = tabForward;
            _tabBackwards = tabBackwards;
        }
        TerminalReadLine_t readLine(EZTools::EZString mask = "") {
            TerminalReadLine_t i;
            struct termios t;
            struct termios t_saved;
            tcgetattr(fileno(stdin), &t);
            t_saved = t;
            t.c_lflag &= (~ICANON & ~ECHO);
            t.c_cc[VTIME] = 0;
            t.c_cc[VMIN] = 1;
            if (tcsetattr(fileno(stdin), TCSANOW, &t) < 0) {
                i.message = "Unable to set terminal to single character mode";
                return i;
            }
            std::cin.ignore(std::cin.rdbuf()->in_avail());
            std::streambuf *pbuf = std::cin.rdbuf();
            bool done = false;
            while (!done) {
                char c;
                if (pbuf->sgetc() == EOF) done = true;
                c = pbuf->sbumpc();
                if (c == 27) {
                    //fixme: handle left and right arrow keys
                    std::cin.ignore(std::cin.rdbuf()->in_avail());
                    c = pbuf->sbumpc();
                    std::cin.ignore(std::cin.rdbuf()->in_avail());
                    c = pbuf->sbumpc();
                    std::cin.ignore(std::cin.rdbuf()->in_avail());
                } else {
                    if (_tabForward && c == 9) {
                        done = true;
                    } else if (c == 9) {
                        // do nothing with tab
                    } else if (c == '\n') {
                        done = true;
                    } else if (c == 0x7f) {
                        std::cout << "\b \b";
                        i.data.pop_back();
                    } else {
                        EZTools::EZString cc(c);
                        if (!cc.regexMatch("[^ -~]+")) {
                            if (!mask.empty()) {
                                std::cout << mask;
                            } else {
                                std::cout << c;
                            }
                            i.data += c;
                        }
                    }
                }
            }
            if (tcsetattr(fileno(stdin), TCSANOW, &t_saved) < 0) {
                i.message = "Unable to restore terminal mode";
                return i;
            }
            std::cout << std::endl;
            i.wasSuccessful = true;
            return i;
        }

    protected:
        termios oldt;
        bool _tabForward = false;
        bool _tabBackwards = false;
    };

    inline void consoleBell() {
        std::cout << CON_BELL << std::flush;
    }

    inline void errorMessage(EZTools::EZReturnMetaData metaData, bool fatal = true) {
        consoleBell();
        if (color) {
            std::cout << foreground(BRIGHT_RED) << CON_BOLD_ON;
            std::cout << "Error " << metaData.exitCode << " in " << metaData.location << std::endl
                << foreground(BRIGHT_YELLOW) << CON_BOLD_OFF
                << metaData.message << std::endl << CON_RESET << std::flush;
        } else {
            std::cout << "Error " << metaData.exitCode << " in " << metaData.location << std::endl
                << metaData.message << std::endl;
        }
        if (fatal) {
            exit(metaData.exitCode);
        }
    }

    inline void HTTPErrorMessage(EZHTTP::HTTPResponseMetaData metaData, bool fatal = true) {
        consoleBell();
        if (color) {
            std::cout << foreground(BRIGHT_RED) << CON_BOLD_ON;
            std::cout << "Error " << metaData.curlCode << " in " << metaData.location << std::endl
                      << foreground(BRIGHT_YELLOW) << CON_BOLD_OFF
                      << metaData.message << std::endl;
            std::cout << foreground(BRIGHT_CYAN) << "HTTP Code: " << metaData.httpResponseCode
                      << std::endl << CON_RESET << std::flush;
        } else {
            std::cout << "Error " << metaData.curlCode << " in " << metaData.location << std::endl
                      << metaData.message << std::endl;
            std::cout << "HTTP Code: " << metaData.httpResponseCode << std::endl;
        }
        if (fatal) {
            exit(metaData.curlCode);
        }
    }

    inline void reset() {
        std::cout << CON_RESET << std::flush;
    }

}

#endif //EZT_EZSCREEN_H
