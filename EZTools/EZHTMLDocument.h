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

#ifndef EZT_EZHTMLDOCUMENT_H
#define EZT_EZHTMLDOCUMENT_H
#include "json.h"
#include "EZTools.h"

namespace EZHTMLDocument {

    enum META_t {
        NAME,
        CHARSET,
        HTTPEQUIV
    };

    class Header {
    public:
        Header() = default;
        ~Header() = default;
        void title(EZTools::EZString title) {
            nlohmann::json m;
            m["entry"] = _document.size();
            m["type"] = "TITLE";
            m["value"] = title;
            _document.push_back(m);
        }
        void meta(META_t type, EZTools::EZString value, EZTools::EZString content = "") {
            nlohmann::json m;
            m["entry"] = _document.size();
            m["type"] = "META";
            switch (type) {
                case NAME:
                    m["name"] = value;
                    m["content"] = content;
                    break;
                case CHARSET:
                    m["charset"] = value;
                    break;
                case HTTPEQUIV:
                    m["http-equiv"] = value;
                    m["content"] = content;
                    break;
            }
            _document.push_back(m);
        }
        void style(EZTools::EZString Style) {
            nlohmann::json m;
            m["entry"] = _document.size();
            m["type"] = "STYLE";
            m["value"] = Style.str();
            _document.push_back(m);
        }
        void script(EZTools::EZString Script) {
            nlohmann::json m;
            m["entry"] = _document.size();
            m["type"] = "SCRIPT";
            m["value"] = Script.str();
            _document.push_back(m);
        }
        void link(EZTools::EZString rel, EZTools::EZString href) {
            nlohmann::json m;
            m["entry"] = _document.size();
            m["type"] = "LINK";
            m["rel"] = rel;
            m["href"] = href;
            _document.push_back(m);
        }
        nlohmann::json data() { return _document; }
    private:
        nlohmann::json _document;
    };

    class HTMLDocument {
    public:
        HTMLDocument() {
            _document["HTML"] = nlohmann::json::object();
        }
        ~HTMLDocument() = default;

        EZTools::EZString dump() {
            _document["HTML"]["HEAD"] = head.data();
            return _document.dump(4);
        }
        Header head;
    private:
        nlohmann::json _document;
    };

}

#endif //EZT_EZHTMLDOCUMENT_H
