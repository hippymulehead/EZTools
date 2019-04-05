//
// Created by mromans on 4/5/19.
//

#include "../include/EZString.h"

vector<EZString> EZString::splitBy(EZString delimiter) const {
    vector<EZString> ret;
    size_t pos = 0;
    string s = *this;
    while ((pos = s.find(delimiter)) != string::npos) {
        ret.emplace_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    ret.emplace_back(s);
    return ret;
}

void EZString::join(EZString sept, vector<EZString> &list) {
    assign(list[0]);
    for(uint i = 1; i < list.size(); i++) {
        append(sept);
        append(list[i]);
    }
}

unsigned int EZString::count(const EZString s) const {
    uint total = 0;
    size_t end,pos = 0;

    while(true) {
        end = find(s,pos);
        if(end == string::npos)
            break;
        pos = end + s.size();
        total++;
    }
    return total;
}

EZString EZString::lower() {
    for(iterator i = begin(); i != end(); i++) {
        if(*i >= 'A' && *i <= 'Z') {
            *i += ('a' - 'A');
        }
    }
    return *this;
}

EZString EZString::upper() {
    for(iterator i = begin(); i != end(); i++) {
        if(*i >= 'a' && *i <= 'z') {
            *i -= ('a' - 'A');
        }
    }
    return *this;
}

bool EZString::islower() const {
    bool Bool = true;
    if (length() == 0)
        Bool = false;
    else {
        for(const_iterator i = begin(); i != end(); i++) {
            if(*i >= 'A' && *i <= 'Z') {
                Bool = false;
                break;
            }
        }
    }
    return Bool;
}

bool EZString::isupper() const {
    bool Bool = true;
    if (length() == 0)
        Bool = false;
    else {
        for(const_iterator i = begin(); i != end(); i++) {
            if(*i >= 'a' && *i <= 'z') {
                Bool = false;
                break;
            }
        }
    }
    return Bool;
}

bool EZString::isInt() const {
    bool Bool = true;
    if (length() == 0)
        Bool = false;
    else {
        for(const_iterator i = begin(); i != end(); i++) {
            if(*i < '0' || *i > '9') {
                Bool = false;
                break;
            }
        }
    }
    return Bool;
}

int EZString::asInt() const {
    if (this->isInt()) {
        return stoi(this->str());
    }
    return -1;
}

bool EZString::isFloat() const {
    bool Bool = true;
    if (length() == 0)
        Bool = false;
    else if( count(".") != 1 )
        Bool = false;
    else {
        for(const_iterator i = begin(); i != end(); i++) {
            if((*i < '0' || *i > '9') && *i != '.') {
                Bool = false;
                break;
            }
        }
    }
    return Bool;
}

float EZString::asFloat() const {
    if (this->isFloat()) {
        return stof(this->str());
    }
    return -1;
}

bool EZString::isAlpha() const {
    bool Bool = true;
    if (length() == 0)
        Bool = false;
    else {
        for(const_iterator i = begin(); i != end(); i++) {
            if((*i < 'a' || *i > 'z') && (*i < 'A' || *i > 'Z') && *i != ' ') {
                Bool = false;
                break;
            }
        }
    }
    return Bool;
}

string EZString::str() const {
    return *this;
}

bool EZString::includes(EZString lookingFor) const {
    size_t found = this->find(lookingFor);
    return found != std::string::npos;
}

bool EZString::beginsWith(EZString lookingFor) const {
    return this->find(lookingFor) == 0;
}

bool EZString::endsWith(EZString lookingFor) const {
    if (lookingFor.size() > this->size()) return false;
    return std::equal(lookingFor.rbegin(), lookingFor.rend(), this->rbegin());
}

EZString EZString::replaceFirst(EZString thisString, EZString withString) const {
    EZString s = *this;
    size_t start_pos = s.find(thisString);
    if (start_pos == std::string::npos)
        return "";
    s.replace(start_pos, thisString.length(), withString);
    return s;
}

EZString EZString::replaceAll(EZString thisString, EZString withString) const {
    EZString result;
    size_t find_len = thisString.size();
    size_t pos, from = 0;
    while (string::npos != (pos = this->find(thisString, from))) {
        result.append(*this, from, pos - from);
        result.append(withString);
        from = pos + find_len;
    }
    result.append(*this, from, string::npos);
    return result;
}

EZString EZString::regexReplace(EZString regexString, EZString replaceWith) const {
    string str = this->str();
    string newtext = replaceWith.str();
    boost::regex re(regexString.str());
    EZString result = boost::regex_replace(str, re, newtext);
    return result;
}

vector<EZString> EZString::splitAtLast(EZString &delimiter) const {
    EZString s = *this;
    size_t splitpos = s.find_last_of(delimiter);
    vector<EZString> ret;
    ret.emplace_back(s.substr(0, splitpos));
    ret.emplace_back(s.substr(splitpos + 1, (s.size() - (splitpos + 1))));
    return ret;
}

EZString EZString::chopAtLast(EZString &delimiter) const {
    EZString s = *this;
    size_t splitpos = s.find_last_of(delimiter);
    return s.substr(0, splitpos);
}

stringstream EZString::asStringstream() const {
    stringstream s;
    s << *this;
    return s;
}