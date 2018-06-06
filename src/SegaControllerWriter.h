//
// SegaControllerWriter.h
//
// Author:
//       Jon Thysell <thysell@gmail.com>
//
// Copyright (c) 2017-2018 Jon Thysell <http://jonthysell.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "constants.h"

#ifndef SegaControllerWriter_h
#define SegaControllerWriter_h

class SegaControllerWriter {
    public:
        SegaControllerWriter(byte db9_pin_7, byte db9_pin_1, byte db9_pin_2, byte db9_pin_3, byte db9_pin_4, byte db9_pin_6, byte db9_pin_9);

        void readSelectPin();
        void setState(word state);

    private:
        void writeCycle();

        word _currentState;
        
        byte _currentCycle;
        
        boolean _selectPinHigh;

        unsigned long _lastSelectTime;

        boolean _sixButtonMode;

        byte _selectPin; // input select pin
        byte _outputPins[SC_IO_PINS];
};

#endif