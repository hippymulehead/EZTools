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

#ifndef SCR_EZSCREEN_H
#define SCR_EZSCREEN_H

#include <iostream>
#include <string>
#include <sstream>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <cstdio>
#include <fstream>
#include "EZTools.h"
#include "EZLinux.h"
#include "EZConfig.h"

using namespace EZTools;

namespace EZScreen {

    enum EZDRAWCHAR {
        EZ_UPPER_RIGHT_CORNER,
        EZ_UPPER_LEFT_CORNER,
        EZ_LOWER_RIGHT_CORNER,
        EZ_LOWER_LEFT_CORNER,
        EZ_TOP_T,
        EZ_BOTTOM_T,
        EZ_LEFT_T,
        EZ_RIGHT_T,
        EZ_HORIZONTAL_LINE,
        EZ_VERTICAL_LINE,
        EZ_CROSS
    };

    enum COLOR {
        EZ_DEFAULT = 39,
        EZ_BLACK = 30,
        EZ_RED = 31,
        EZ_GREEN = 32,
        EZ_YELLOW = 33,
        EZ_BLUE = 34,
        EZ_MAGENTA = 35,
        EZ_CYAN = 36,
        EZ_WHITE = 37
    };
    enum CON {
        EZ_RESET = 0,
        EZ_BOLD = 1,
        EZ_BOLDOFF = 2,
        EZ_UNDERLINE = 4,
        EZ_UNDERLINEOFF = 24,
        EZ_INVERSE = 7,
        EZ_INVERSEOFF = 27,
        EZ_HIDDEN = 8,
        EZ_REVEAL = 28,
        EZ_FASTBLINK = 6,
        EZ_SLOWBLINK = 5,
        EZ_BLINKOFF = 25
    };

    enum CC {
        INDEXREV = 0x8d,
        INDEX = 0x84
    };

    enum MENUPOS {
        MENU_LEFT,
        MENU_RIGHT
    };

    enum EZERASEDISPLAY {
        EZ_CURRENT_TO_END,
        EZ_CURRENT_TO_BEGINING,
        EZ_CLEAR_ALL,
        EZ_CLEAR_SCROLLBACK
    };

    enum EZLINETYPE {
        EZ_CURRENT_TO_EOL,
        EZ_CURRENT_TO_BOL,
        EZ_FULL_LINE
    };

    struct HelpOption {
        EZString option;
        EZString description;
    };

    struct MenuOptions {
        EZString text;
        int optionNumber;
        bool ignore = false;
    };

    class EZMenuObject {
    public:
        EZMenuObject(EZString title);
        EZMenuObject(EZString title, MENUPOS pos, int x, int y);
        ~EZMenuObject() = default;
        void addOption(EZString text, int optionNumber, bool ignore = false);
        void setDefault(int defaultOption);
        vector<MenuOptions> menu() { return _menuOptions; }
        EZString menuTitle() { return _menuTitle; }
        void defaultOption(int defaultOption) { _defaultOption = defaultOption; }
        int defaultOption() { return _defaultOption; }
        int x() { return _x; }
        int y() { return _y; }

    private:
        vector<MenuOptions> _menuOptions;
        EZString _menuTitle;
        int _defaultOption;
        MENUPOS _pos;
        int _x;
        int _y;
    };

    class EZScreenControl {
    public:
        EZScreenControl();
        ~EZScreenControl() = default;
        int maxX() { return _max_x; }
        int maxY() { return _max_y; }
        int minX() { return _min_x; }
        int minY() { return _min_y; }
        bool colorSupport() { return _color; }
        static void clear();
        string set(COLOR fg, COLOR bg, CON con);
        string reset();
        string fg(COLOR fg);
        string fg(int fg);
        string fg24(int r, int g, int b);
        string fgbg(COLOR fg, COLOR bg);
        string bg(int bg);
        string bg24(int r, int g, int b);
        void cursorMoveToXY(int x, int y);
        void cursorMoveUp(int lines);
        void cursorMoveDown(int lines);
        void cursorMoveForward(int chars);
        void cursorMoveBackward(int chars);
        void cursorSavePos();
        void cursorRestorePos();
        void eraseDisplay(EZERASEDISPLAY edType);
        void eraseLine(EZLINETYPE line);
        string con(CON con);
        string termName() { return _term; }
        string textCentered(EZString text, int width);
        string textLeft(EZString text, int width);
        string textRight(EZString text, int width);
        string putChar(EZDRAWCHAR ch);
        void drawHorizonalLine(int xa, int xb, int y);
        string drawHorizonalLineInPlace(int width);
        void drawVertical(int x, int ya, int yb);
        void drawBox(int lx, int ly, int rx, int ry, COLOR fg = EZ_DEFAULT, COLOR bg = EZ_DEFAULT, bool fill = false);
        EZString menuName(EZString name, EZString ulChar, COLOR fg, COLOR bg);
        //TODO: finish popupMenu & DropdownMenu
        int popupMenu(EZMenuObject menuobj);
        int dropdownMenu(EZMenuObject menuobj);

    private:
        int _max_x;
        int _max_y;
        int _min_x = 1;
        int _min_y = 1;
        bool _color = false;
        int _colors = -1;
        COLOR _lastFG = EZ_WHITE;
        COLOR _lastBG = EZ_DEFAULT;
        CON _lastCON = EZ_RESET;
        EZString _term = "";

        int checkColors() {
            EZString t = EZLinux::exec("/usr/bin/tput colors");
            t.chomp();
            int i = 0;
            if (!t.empty()) {
                i = t.asInt();
            }
            return i;
        }
    };

}

#endif //SCR_EZSCREEN_H
