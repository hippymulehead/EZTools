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

#ifndef EZTOOLS_EZSYSTEMTIME_H
#define EZTOOLS_EZSYSTEMTIME_H

#include "EZString.h"

class EZSystemTime {
public:
    EZSystemTime() = default;
    virtual ~EZSystemTime() = default;
    void update();
    EZString weekday() const { return _weekday; }
    EZString month() const { return _month; }
    EZString monthNumber() const { return _monthNumber; }
    EZString monthday() const { return _monthday; }
    EZString time() const { return _time; }
    EZString timeNoSec() const { return _timeNoSec; }
    EZString year() const { return _year; }
    void setDateSeperator(EZString dateSep) {_dateSep = dateSep; }
    EZString mdy() const;
    EZString ymd() const;
    EZString ymdt() const;
    EZString mdyt() const;
    EZString fancy() const;
    EZString fancyWithWeekDay() const;
    EZString dtstamp() const;

private:
    EZString _weekday;
    EZString _month;
    EZString _monthNumber;
    EZString _monthday;
    EZString _time;
    EZString _timeNoSec;
    EZString _year;
    EZString _dateSep = "/";
};

#endif //EZTOOLS_EZSYSTEMTIME_H