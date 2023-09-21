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

//fixme: THIS IS A WORK IN PROGRESS, NO WHERE NEAR COMPLETE

#ifndef EZT_EZHTML_H
#define EZT_EZHTML_H

#pragma once

#include <ostream>
#include "EZTools.h"
#include "EZHTTP.h"

namespace EZHTML {

    enum AUDIO_FORMAT {MP3, OGG, WAV};
    enum AUDIO_OPTIONS {AUTOPLAY, CONTROLS, LOOP, MUTED};

    inline EZTools::EZString center(EZTools::EZString text) {
        std::stringstream ss;
        ss << "<CENTER>" << text << "</CENTER>";
        return ss.str();
    }

    inline EZTools::EZString h(int size, EZTools::EZString text) {
        std::stringstream ss;
        ss << "<H" << size << ">" << text << "</H" << size << ">";
        return ss.str();
    }

    inline EZTools::EZString br() { return "<BR>"; }

    inline EZTools::EZString hr() { return "<HR>"; }

    inline EZTools::EZString img(EZTools::EZString src, EZTools::EZString alt = "", int width = 0, int height = 0) {
        std::stringstream ss;
        ss << "<img src=\"" << src << "\"";
        if (!alt.empty()) {
            ss << " alt=\"" << alt << "\"";
        }
        if (width != 0) {
            ss << " width=\"" << width << "\"";
        }
        if (height != 0) {
            ss << " height=\"" << height << "\"";
        }
        ss << ">";
        return ss.str();
    }

    inline EZTools::EZString a(EZTools::EZString href, EZTools::EZString object) {
        std::stringstream ss;
        ss << "<a href=\"" << href << "\">" << object << "</a>";
        return ss.str();
    }

    inline EZTools::EZString audio(EZTools::EZString src, AUDIO_FORMAT audioType, std::vector<AUDIO_OPTIONS> options) {
        std::stringstream ss;
        ss << "<audio";
        for (auto& op : options) {
            switch (op) {
                case CONTROLS:
                    ss << " controls";
                    break;
                case AUTOPLAY:
                    ss << " autoplay";
                    break;
                case LOOP:
                    ss << " loop";
                    break;
                case MUTED:
                    ss << " muted";
            }
        }
        ss << ">\n";
        ss << "\t<source src=\"" << src << "\" type=\"";
        switch (audioType) {
            case MP3:
                ss << "audio/mpeg";
                break;
            case OGG:
                ss << "audio/ogg";
                break;
            case WAV:
                ss << "audio/wav";
                break;
        }
        ss << "\">\n</audio>";
        return ss.str();
    }

    /*
     * <label for="disk_c">Disk usage C:</label>
<meter id="disk_c" value="2" min="0" max="10">2 out of 10</meter><br>
     */
    inline EZTools::EZString meter(EZTools::EZString title, EZTools::EZString id, float value, float min, float max,
                                   bool lable = true) {
        std::stringstream ss;
        if (lable) {
            ss << "<label for=\"" << id << "\">" << title << "</lable>\n";
        }
        ss << "<meter id=\"" << id << "\" value=\"" << value << "\" min=\"" << min << "\" max=\""
            << max << "\">" << value << " out of " << max << "</meter>";
        return ss.str();
    }

    inline EZTools::EZString b(EZTools::EZString text) {
        std::stringstream ss;
        ss << "<B>" << text << "</B>";
        return ss.str();
    }

    class Body : public std::stringstream {
    public:
        EZTools::EZString output() {
            return this->str();
        }
    private:

    };

    class Template {
    public:
        Template() = default;
        ~Template() = default;
        EZTools::EZReturn<bool> init(EZHTTP::URI Template) {
            EZTools::EZReturn<bool> res;
            res.location("EZHTML::init");
            EZHTTP::URI uri(Template.base());
            EZHTTP::HTTPClient http(uri);
            auto tf = http.get(Template.path().regexReplace(Template.base(), ""));
            if (!tf.wasSuccessful()) {
                res.message(tf.message());
                return res;
            }
            _template = tf.data;
            res.wasSuccessful(true);
            return res;
        }
        void replace(EZTools::EZString lookFor, EZTools::EZString replaceWith) {
            std::stringstream ss;
            ss << "\\{" << lookFor << "\\}";
            _template.regexReplace(ss.str(), replaceWith);
        }
        EZTools::EZString output() {
            return _template;
        }
    private:
        EZTools::EZString _template;
    };

    class HTMLOutput : public std::ostream {
    public:
        HTMLOutput() : std::ostream(std::cout.rdbuf()) {}
        ~HTMLOutput() = default;
    private:
        template<typename T>
        friend HTMLOutput &operator<<(HTMLOutput &myo, T &v) {
            static_cast<std::ostream &>(myo) << v;
            return myo;
        }
    };

}

#endif //EZT_EZHTML_H
