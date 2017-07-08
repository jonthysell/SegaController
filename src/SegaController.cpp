//
// SegaController.cpp
//
// Author:
//       Jon Thysell <thysell@gmail.com>
//
// Copyright (c) 2017 Jon Thysell <http://jonthysell.com>
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
#include "SegaController.h"

SegaController::SegaController(int db9_pin_7, int db9_pin_1, int db9_pin_2, int db9_pin_3, int db9_pin_4, int db9_pin_6, int db9_pin_9)
{
    // Set pins
    _selectPin = db9_pin_7;

    _inputPins[0] = db9_pin_1;
    _inputPins[1] = db9_pin_2;
    _inputPins[2] = db9_pin_3;
    _inputPins[3] = db9_pin_4;
    _inputPins[4] = db9_pin_6;
    _inputPins[5] = db9_pin_9;

    // Setup output pin
    pinMode(_selectPin, OUTPUT);
    digitalWrite(_selectPin, HIGH);

    // Setup input pins
    for (int i = 0; i < SC_INPUT_PINS; i++)
    {
        pinMode(_inputPins[i], INPUT);
        digitalWrite(_inputPins[i], HIGH);
    }

    _currentState = 0;
    _sixButtonMode = false;
    _lastReadTime = millis();
}

word SegaController::getState()
{
    if (max(millis() - _lastReadTime, 0) < SC_READ_DELAY_MS)
    {
        // Not enough time has elapsed, return previous state
        return _currentState;
    }

    // Update state before returning
    _currentState = 0;

    digitalWrite(_selectPin, LOW);

    for (int cycle = 0; cycle < SC_CYCLES; cycle++)
    {
        readCycle(cycle);
    }

    // When a controller disconnects, revert to three-button polling
    if ((_currentState & SC_CTL_ON) == 0)
    {
        _sixButtonMode = false;
    }

    _lastReadTime = millis();

    return _currentState;
}

void SegaController::readCycle(int cycle)
{
    digitalWrite(_selectPin, cycle % 2 == 0 ? LOW : HIGH);

    // Read flags
    switch (cycle)
    {
        case 2:
            // Check that a controller is connected
            _currentState |= (digitalRead(_inputPins[2]) == LOW && digitalRead(_inputPins[3]) == LOW) ? SC_CTL_ON : 0;
            
            // Check controller is connected before reading A/Start to prevent bad reads when inserting/removing cable
            if (_currentState & SC_CTL_ON)
            {
                _currentState |= (digitalRead(_inputPins[4]) == LOW) ? SC_BTN_A : 0;
                _currentState |= (digitalRead(_inputPins[5]) == LOW) ? SC_BTN_START : 0;
            }
            break;
        case 3:
            _currentState |= (digitalRead(_inputPins[0]) == LOW) ? SC_BTN_UP : 0;
            _currentState |= (digitalRead(_inputPins[1]) == LOW) ? SC_BTN_DOWN : 0;
            _currentState |= (digitalRead(_inputPins[2]) == LOW) ? SC_BTN_LEFT : 0;
            _currentState |= (digitalRead(_inputPins[3]) == LOW) ? SC_BTN_RIGHT : 0;
            _currentState |= (digitalRead(_inputPins[4]) == LOW) ? SC_BTN_B : 0;
            _currentState |= (digitalRead(_inputPins[5]) == LOW) ? SC_BTN_C : 0;
            break;
        case 4:
            _sixButtonMode = (digitalRead(_inputPins[0]) == LOW && digitalRead(_inputPins[1]) == LOW);
            break;
        case 5:
            if (_sixButtonMode)
            {
                _currentState |= (digitalRead(_inputPins[0]) == LOW) ? SC_BTN_Z : 0;
                _currentState |= (digitalRead(_inputPins[1]) == LOW) ? SC_BTN_Y : 0;
                _currentState |= (digitalRead(_inputPins[2]) == LOW) ? SC_BTN_X : 0;
                _currentState |= (digitalRead(_inputPins[3]) == LOW) ? SC_BTN_MODE : 0;
            }
            break;
    }
}
