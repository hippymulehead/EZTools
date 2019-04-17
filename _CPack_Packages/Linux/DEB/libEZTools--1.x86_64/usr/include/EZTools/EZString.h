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

#ifndef EZTOOLS_EZSTRING_H
#define EZTOOLS_EZSTRING_H

#include <string>
#include <vector>
#include <boost/regex.hpp>
//#include <cryptopp/modes.h>
//#include <cryptopp/aes.h>
//#include <cryptopp/filters.h>

using namespace std;

class EZString;

class EZString : public string {

public:
    EZString() : string() {}
    EZString(const string &s) : string(s) {}
    EZString(const string &s, size_t n) : string(s, n) {}
    EZString(const char *s, size_t n) : string(s, n) {}
    EZString(const char *s) : string(s) {}
    EZString(size_t n, char c) : string(n, c) {}
    template<class InputIterator>
    EZString(InputIterator first, InputIterator last) : string(first, last) {}
    vector<EZString> splitBy(EZString delimiter) const;
    void join(EZString sept, vector<EZString> &list);
    unsigned int count(const EZString s) const;
    EZString lower();
    EZString upper();
    bool islower() const;
    bool isupper() const;
    bool isInt() const;
    int asInt() const;
    bool isFloat() const;
    float asFloat() const;
    bool isAlpha() const;
    string str() const;
    bool includes(EZString lookingFor) const;
    bool beginsWith(EZString lookingFor) const;
    bool endsWith(EZString lookingFor) const;
    EZString replaceFirst(EZString thisString, EZString withString) const;
    EZString replaceAll(EZString thisString, EZString withString) const;
    EZString regexReplace(EZString regexString, EZString replaceWith) const;
    vector<EZString> splitAtLast(EZString &delimiter) const;
    EZString chopAtLast(EZString &delimiter) const;
    stringstream asStringstream() const;
};

#endif //EZTOOLS_EZSTRING_H