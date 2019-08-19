//
// Created by mromans on 8/5/19.
//

#include "EZScreen.h"

EZScreen::EZScreenControl::EZScreenControl() {
    winsize size;
    ioctl(0, TIOCGWINSZ, &size);
    _max_x = size.ws_col;
    _max_y = size.ws_row;
    _min_x = 1;
    _min_y = 1;
    _term = EZLinux::environmentVar("TERM");
    EZString home = EZLinux::homeDir();
    EZConfig conf(home + "/.ezscreen");
    if (conf.isThere()) {
        for (auto x = 0; x < conf.jsonRoot.size(); x++) {
            if (conf.jsonRoot[x]["termName"].asString() == _term) {
                _color = conf.jsonRoot[x]["color"].asBool();
                _colors = conf.jsonRoot[x]["colorsSupported"].asInt();
            }
        }
        if (_colors == -1) {
            int c = checkColors();
            if (c == 0) {
                _color = false;
            }
            if (!conf.isThere()) {
                conf.jsonRoot = Json::Value(Json::arrayValue);
            }
            int t = conf.jsonRoot.size();
            conf.jsonRoot[t] = Json::Value(Json::objectValue);
            conf.jsonRoot[t]["termName"] = _term;
            conf.jsonRoot[t]["color"] = _color;
            conf.jsonRoot[t]["colorsSupported"] = c;
            conf.write();
        }
    } else {
        int c = checkColors();
        if (c != 0) {
            _color = true;
        }
        conf.jsonRoot = Json::Value(Json::arrayValue);
        conf.jsonRoot[0] = Json::Value(Json::objectValue);
        conf.jsonRoot[0]["termName"] = _term;
        conf.jsonRoot[0]["color"] = _color;
        conf.jsonRoot[0]["colorsSupported"] = c;
        conf.write();
    }
}

string EZScreen::EZScreenControl::putChar(EZScreen::EZDRAWCHAR ch) {
    if (_color) {
        switch (ch) {
            case EZ_UPPER_RIGHT_CORNER:
                return "\342\224\220";
            case EZ_UPPER_LEFT_CORNER:
                return "\342\224\214";
            case EZ_LOWER_RIGHT_CORNER:
                return "\342\224\230";
            case EZ_LOWER_LEFT_CORNER:
                return "\342\224\224";
            case EZ_LEFT_T:
                return "\342\224\234";
            case EZ_RIGHT_T:
                return "\342\224\244";
            case EZ_TOP_T:
                return "\342\224\254";
            case EZ_BOTTOM_T:
                return "\342\224\264";
            case EZ_HORIZONTAL_LINE:
                return "\342\224\200";
            case EZ_VERTICAL_LINE:
                return "\342\224\202";
            case EZ_CROSS:
                return "\342\224\274";
        }
    } else {
        switch (ch) {
            case EZ_UPPER_LEFT_CORNER:
                return "+";
            case EZ_UPPER_RIGHT_CORNER:
                return "+";
            case EZ_LOWER_LEFT_CORNER:
                return "+";
            case EZ_LOWER_RIGHT_CORNER:
                return "+";
            case EZ_RIGHT_T:
                return "+";
            case EZ_LEFT_T:
                return "+";
            case EZ_TOP_T:
                return "+";
            case EZ_BOTTOM_T:
                return "+";
            case EZ_HORIZONTAL_LINE:
                return "-";
            case EZ_VERTICAL_LINE:
                return "|";
            case EZ_CROSS:
                return "+";
        }
    }
    return string();
}

string EZScreen::EZScreenControl::set(COLOR fg, COLOR bg, CON con) {
    if (_color) {
        stringstream ss;
        ss << "\033[" << con << ";" << fg << ";" << bg + 10 << "m";
        _lastCON = con;
        _lastFG = fg;
        _lastBG = bg;
        return ss.str();
    } else {
        return "";
    }
}

string EZScreen::EZScreenControl::fg(COLOR fg) {
    if (_color) {
        stringstream ss;
        _lastFG = fg;
        ss << "\033[" << EZ_RESET << ";" << fg << ";" << EZ_DEFAULT + 10 << "m";
        return ss.str();
    } else {
        return "";
    }
}

string EZScreen::EZScreenControl::fgbg(EZScreen::COLOR fg, EZScreen::COLOR bg) {
    if (_color) {
        stringstream ss;
        _lastFG = fg;
        _lastBG = bg;
        ss << "\033[" << EZ_RESET << ";" << fg << ";" << bg + 10 << "m";
        return ss.str();
    } else {
        return "";
    }
}

string EZScreen::EZScreenControl::con(EZScreen::CON con) {
    if (_color) {
        stringstream ss;
        ss << "\033[" << con << ";" << _lastFG << ";" << _lastBG + 10 << "m";
        return ss.str();
    }
    return "";
}

string EZScreen::EZScreenControl::reset() {
    if (_color) {
        stringstream ss;
        _lastFG = EZ_DEFAULT;
        _lastBG = EZ_DEFAULT;
        ss << "\033[" << EZ_RESET << ";" << EZ_DEFAULT << ";" << EZ_DEFAULT + 10 << "m";
        return ss.str();
    }
    return "";
}

void EZScreen::EZScreenControl::clear() {
    cout << "\033[2J\033[1;1H";
}

string EZScreen::EZScreenControl::fg(int fg) {
    if (_color) {
        stringstream ss;
        ss << "\033[38;5;" << fg << "m";
        return ss.str();
    }
    return "";
}

string EZScreen::EZScreenControl::bg(int bg) {
    if (_color) {
        stringstream ss;
        ss << "\033[48;5;" << bg << "m";
        return ss.str();
    }
    return "";
}

string EZScreen::EZScreenControl::bg24(int r, int g, int b) {
    if (_color) {
        stringstream ss;
        ss << "\033[48;2;" << r << ";" << g << ";" << b << "m";
        return ss.str();
    }
    return "";
}

string EZScreen::EZScreenControl::fg24(int r, int g, int b) {
    if (_color) {
        stringstream ss;
        ss << "\033[38;2;" << r << ";" << g << ";" << b << "m";
        return ss.str();
    }
    return "";
}

string EZScreen::EZScreenControl::textCentered(EZString text, int width) {
    stringstream ss;
    int sp = (width - text.length()) / 2;
    for (auto i = 0; i < sp; i++) {
        ss << " ";
    }
    ss << text;
    int le = width - text.length();
    for (auto i = 0; i < le; i++) {
        ss << " ";
    }
    return ss.str();
}

string EZScreen::EZScreenControl::textLeft(EZString text, int width) {
    stringstream ss;
    int wl = width - text.length();
    ss << text;
    for (auto i = 0; i < wl; i++) {
        ss << " ";
    }
    return ss.str();
}

string EZScreen::EZScreenControl::textRight(EZString text, int width) {
    stringstream ss;
    int wl = width - text.length();
    for (auto i = 0; i < wl; i++) {
        ss << " ";
    }
    ss << text;
    return ss.str();
}

void EZScreen::EZScreenControl::cursorMoveToXY(int x, int y) {
    cout << "\033[" << y << ";" << x << "H";
}

void EZScreen::EZScreenControl::cursorMoveDown(int lines) {
    cout << "\033[" << lines << "B";
}

void EZScreen::EZScreenControl::cursorMoveUp(int lines) {
    cout << "\033[" << lines << "A";
}

void EZScreen::EZScreenControl::cursorMoveForward(int chars) {
    cout << "\033[" << chars << "C";
}

void EZScreen::EZScreenControl::cursorMoveBackward(int chars) {
    cout << "\033[" << chars << "D";
}

void EZScreen::EZScreenControl::cursorSavePos() {
    cout << "\033[s";
}

void EZScreen::EZScreenControl::cursorRestorePos() {
    cout << "\033[u";
}

void EZScreen::EZScreenControl::eraseDisplay(EZERASEDISPLAY edType) {
    EZString ch = "";
    switch(edType) {
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
    cout << "\033[" << ch << "J";
}

void EZScreen::EZScreenControl::eraseLine(EZLINETYPE line) {
    EZString ch = "";
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
    cout << "\033[" << ch << "K";
}

void EZScreen::EZScreenControl::drawHorizonalLine(int xa, int xb, int y) {
    stringstream ss;
    for (auto i = xa; i <= xb; i++) {
        cursorMoveToXY(i, y);
        cout << putChar(EZ_HORIZONTAL_LINE);
    }
}

void EZScreen::EZScreenControl::drawVertical(int x, int ya, int yb) {
    stringstream ss;
    for (auto i = ya; i <= yb; i++) {
        cursorMoveToXY(x, i);
        cout << putChar(EZ_VERTICAL_LINE);
    }
}

void EZScreen::EZScreenControl::drawBox(int lx, int ly, int rx, int ry, COLOR fg, COLOR bg, bool fill) {
    cout << this->fgbg(fg,bg);
    cursorMoveToXY(lx,ly);
    cout << putChar(EZ_UPPER_LEFT_CORNER);
    drawHorizonalLine(lx + 1, rx - 1, ly);
    cout << putChar(EZ_UPPER_RIGHT_CORNER);
    cursorMoveToXY(lx,ry);
    cout << putChar(EZ_LOWER_LEFT_CORNER);
    drawHorizonalLine(lx + 1, rx - 1, ry);
    cout << putChar(EZ_LOWER_RIGHT_CORNER);
    drawVertical(lx, ly + 1, ry -1);
    drawVertical(rx, ly + 1, ry -1);
    if (fill) {
        for (auto y = ly + 1; y < ry; y++) {
            for (auto x = lx + 1; x < rx; x++) {
                this->cursorMoveToXY(x,y);
                cout << " ";
            }
        }
    }
    cout << this->reset();
}

int EZScreen::EZScreenControl::popupMenu(EZMenuObject menuobj) {
    int maxLen = 0;
    for (auto i = 0; i < menuobj.menu().size(); i++) {
        if (maxLen < menuobj.menu().at(i).text.length()) {
            maxLen = menuobj.menu().at(i).text.length();
        }
    }
    if (maxLen < menuobj.menuTitle().length()) {
        maxLen = menuobj.menuTitle().length();
    }
    maxLen++;
    if (maxLen % 2 != 0) {
        maxLen++;
    }
    int maxLenP = maxLen + 4;
    int x = maxX() / 2 - (maxLenP / 2);
    int y = (maxY() / 2 - ((menuobj.menu().size() + 2) / 2));
    cursorMoveToXY(x, y);
    fgbg(EZ_WHITE, EZ_BLUE);
    int maxWidthPos = x + maxLen + 1;
    drawBox(x, y, maxWidthPos, menuobj.menu().size() + y + 1);
    con(EZ_BOLD);
    fgbg(EZ_WHITE, EZ_BLUE);
    cursorMoveToXY(x + 1, y);
    textLeft(menuobj.menuTitle(), menuobj.menuTitle().length());
    for (auto i = 0; i < menuobj.menu().size(); i++) {
        cursorMoveToXY(x + 1, y + i + 1);
        if (menuobj.menu().at(i).optionNumber == menuobj.defaultOption()) {
            fgbg(EZ_BLACK, EZ_GREEN);
        } else {
            fgbg(EZ_WHITE, EZ_BLACK);
        }
        textLeft(menuobj.menu().at(i).text, maxLen);
    }
    return 1;
}

int EZScreen::EZScreenControl::dropdownMenu(EZScreen::EZMenuObject menuobj) {
    int maxLen = 0;
    for (auto i = 0; i < menuobj.menu().size(); i++) {
        if (maxLen < menuobj.menu().at(i).text.length()) {
            maxLen = menuobj.menu().at(i).text.length();
        }
    }
    if (maxLen < menuobj.menuTitle().length()) {
        maxLen = menuobj.menuTitle().length();
    }
    maxLen++;
    if (maxLen % 2 != 0) {
        maxLen++;
    }
    int maxLenP = maxLen + 4;
    int x = menuobj.x();
    int y = menuobj.y();
    cursorMoveToXY(x, y);
    fgbg(EZ_WHITE, EZ_BLUE);
    int maxWidthPos = x + maxLen + 1;
    drawBox(x, y, maxWidthPos, menuobj.menu().size() + y + 1);
    con(EZ_BOLD);
    fgbg(EZ_WHITE, EZ_BLUE);
    cursorMoveToXY(x + 1, y);
    textLeft(menuobj.menuTitle(), menuobj.menuTitle().length());
    for (auto i = 0; i < menuobj.menu().size(); i++) {
        cursorMoveToXY(x + 1, y + i + 1);
        if (menuobj.menu().at(i).optionNumber == menuobj.defaultOption()) {
            fgbg(EZ_BLACK, EZ_GREEN);
        } else {
            fgbg(EZ_WHITE, EZ_BLACK);
        }
        textLeft(menuobj.menu().at(i).text, maxLen);
    }
    return 1;
}

string EZScreen::EZScreenControl::drawHorizonalLineInPlace(int width) {
    stringstream ss;
    for (auto i = 0; i < width; i++) {
        ss << putChar(EZ_HORIZONTAL_LINE);
    }
    return ss.str();
}

EZString EZScreen::EZScreenControl::menuName(EZString name, EZString ulChar, EZScreen::COLOR fg, EZScreen::COLOR bg) {
    name.regexReplace(ulChar, this->fgbg(fg,bg)+ulChar+this->fgbg(EZ_DEFAULT,EZ_DEFAULT));
    return name;
}

//void EZScreen::EZHelpObject::addCommandLineOption(EZString option, EZString description) {
//    HelpOption ho;
//    ho.option = option;
//    ho.description = description;
//    _helpOptions.push_back(ho);
//}
//
//EZScreen::EZHelpObject::EZHelpObject(EZString programName, EZString version) {
//    _programName = programName;
//    _version = version;
//}
//
//void EZScreen::EZHelpObject::addCopyright(EZString copyright) {
//    _copyright = copyright;
//}
//
//void EZScreen::EZHelpObject::addExtraMessage(EZString extraMessage) {
//    _extraMessage = extraMessage;
//}
//
//void EZScreen::EZHelpObject::showHelp() {
//    cout << endl << _programName << " " << _version << " " << _copyright << endl;
//    if (!_extraMessage.empty()) {
//        cout << _extraMessage << endl;
//    }
//    cout << "Help: " << _programName << " <";
//    for (auto &_helpOption : _helpOptions) {
//        cout << _helpOption.option;
//    }
//    cout << "> ";
//    if (_fileOption) {cout << "<Filename ...>"; }
//    cout << endl;
//
//    // Show Options
//    for (auto &_helpOption : _helpOptions) {
//        cout << "  " << _helpOption.option << "\t: " << _helpOption.description << endl;
//    }
//    cout << endl;
//}
//
//void EZScreen::EZHelpObject::showVersion() {
//    cout << endl << _programName << " " << _version << endl << endl;
//}
//
//EZString EZScreen::EZHelpObject::getoptSwitches() {
//    stringstream ss;
//    ss << ":";
//    for (auto& opt : _helpOptions) {
//        ss << opt.option;
//    }
//    return ss.str();
//}

EZScreen::EZMenuObject::EZMenuObject(EZString title) {
    _menuTitle = title;
}

EZScreen::EZMenuObject::EZMenuObject(EZString title, EZScreen::MENUPOS pos, int x, int y) {
    _menuTitle = title;
    _pos = pos;
    _x = x;
    _y = y;
}

void EZScreen::EZMenuObject::addOption(EZString text, int optionNumber, bool ignore) {
    MenuOptions op;
    op.text = text;
    op.optionNumber = optionNumber;
    _menuOptions.push_back(op);
}

void EZScreen::EZMenuObject::setDefault(int defaultOption) {
    _defaultOption = defaultOption;
}