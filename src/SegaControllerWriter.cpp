//
// SegaControllerWriter.cpp
//
// Author:
//       Jon Thysell <thysell@gmail.com>
//
// Copyright (c) 2018 Jon Thysell <http://jonthysell.com>
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

#include "Arduino.h"
#include "SegaControllerWriter.h"

#define CONTROLLER_CONNECTED (_currentState & SC_CTL_ON == SC_CTL_ON)

SegaControllerWriter::SegaControllerWriter(byte db9_pin_7, byte db9_pin_1, byte db9_pin_2, byte db9_pin_3, byte db9_pin_4, byte db9_pin_6, byte db9_pin_9)
{
    // Set pins
    _selectPin = db9_pin_7;

    _outputPins[0] = db9_pin_1;
    _outputPins[1] = db9_pin_2;
    _outputPins[2] = db9_pin_3;
    _outputPins[3] = db9_pin_4;
    _outputPins[4] = db9_pin_6;
    _outputPins[5] = db9_pin_9;

    // Setup select pin
    pinMode(_selectPin, INPUT_PULLUP);

    // Setup input pins
    for (byte i = 0; i < SC_IO_PINS; i++)
    {
        pinMode(_outputPins[i], OUTPUT);
        digitalWrite(_outputPins[i], HIGH);
    }

    _currentState = 0;
    _currentCycle = SC_CYCLES - 1;
    _selectPinHigh = true;
    _sixButtonMode = true;
}

void SegaControllerWriter::readSelectPin()
{
    boolean last = _selectPinHigh;
    _selectPinHigh = digitalRead(_selectPin) == HIGH;
    
    if (last != _selectPinHigh)
    {
        _currentCycle = (_currentCycle + 1) % SC_CYCLES;
        writeCycle();
    }
}

void SegaControllerWriter::setState(word state)
{
    _currentState = state;
}

void SegaControllerWriter::writeCycle()
{
    switch (_currentCycle)
    {
        case 0:
        case 2:
        case 6:
            digitalWrite(_outputPins[0], HIGH);
            digitalWrite(_outputPins[1], HIGH);
            
            // Specify that a controller is connected
            digitalWrite(_outputPins[2], CONTROLLER_CONNECTED ? LOW : HIGH);
            digitalWrite(_outputPins[3], CONTROLLER_CONNECTED ? LOW : HIGH);
            
            // Write output pins for A, Start
            digitalWrite(_outputPins[4], CONTROLLER_CONNECTED && _currentState & SC_BTN_A == SC_BTN_A ? LOW : HIGH);
            digitalWrite(_outputPins[5], CONTROLLER_CONNECTED && _currentState & SC_BTN_START == SC_BTN_START ? LOW : HIGH);
            break;
        case 1:
        case 3:
        case 7:
            // Write output pins for Up, Down, Left, Right, B, C
            digitalWrite(_outputPins[0], CONTROLLER_CONNECTED && _currentState & SC_BTN_UP == SC_BTN_UP ? LOW : HIGH);
            digitalWrite(_outputPins[1], CONTROLLER_CONNECTED && _currentState & SC_BTN_DOWN == SC_BTN_DOWN ? LOW : HIGH);
            digitalWrite(_outputPins[2], CONTROLLER_CONNECTED && _currentState & SC_BTN_LEFT == SC_BTN_LEFT ? LOW : HIGH);
            digitalWrite(_outputPins[3], CONTROLLER_CONNECTED && _currentState & SC_BTN_RIGHT == SC_BTN_RIGHT ? LOW : HIGH);
            digitalWrite(_outputPins[4], CONTROLLER_CONNECTED && _currentState & SC_BTN_B == SC_BTN_B ? LOW : HIGH);
            digitalWrite(_outputPins[5], CONTROLLER_CONNECTED && _currentState & SC_BTN_C == SC_BTN_C ? LOW : HIGH);
            break;
        case 4:
            // Write Up and Down to specify a six-button controller
            digitalWrite(_outputPins[0], CONTROLLER_CONNECTED && _sixButtonMode ? LOW : HIGH);
            digitalWrite(_outputPins[1], CONTROLLER_CONNECTED && _sixButtonMode ? LOW : HIGH);
            
            digitalWrite(_outputPins[2], HIGH);
            digitalWrite(_outputPins[3], HIGH);
            digitalWrite(_outputPins[4], HIGH);
            digitalWrite(_outputPins[5], HIGH);
            break;
        case 5:
            // Write output pins for X, Y, Z, Mode
            digitalWrite(_outputPins[0], CONTROLLER_CONNECTED && _sixButtonMode && _currentState & SC_BTN_Z == SC_BTN_Z ? LOW : HIGH);
            digitalWrite(_outputPins[1], CONTROLLER_CONNECTED && _sixButtonMode && _currentState & SC_BTN_Y == SC_BTN_Y ? LOW : HIGH);
            digitalWrite(_outputPins[2], CONTROLLER_CONNECTED && _sixButtonMode && _currentState & SC_BTN_X == SC_BTN_X ? LOW : HIGH);
            digitalWrite(_outputPins[3], CONTROLLER_CONNECTED && _sixButtonMode && _currentState & SC_BTN_MODE == SC_BTN_MODE ? LOW : HIGH);
            
            digitalWrite(_outputPins[4], HIGH);
            digitalWrite(_outputPins[5], HIGH);
            break;
    }
}
