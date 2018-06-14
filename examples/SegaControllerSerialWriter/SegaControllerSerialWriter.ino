//
// SegaControllerSerialWriter.ino
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

#include <Arduino.h>
#include <SegaControllerWriter.h>

// Console DB9 pins (looking face-on to the console jack):
//
// 1 2 3 4 5
//  6 7 8 9
//
// Connect the pins to digital I/O pins (see below)

// Specify the Arduino pins that are connected to
// DB9 Pin 7, DB9 Pin 1, DB9 Pin 2, DB9 Pin 3, DB9 Pin 4, DB9 Pin 6, DB9 Pin 9
SegaControllerWriter controller(8, 2, 3, 4, 5, 6, 7);

// Controller states
word currentState = 0;
word lastState = 0;

String buffer;

void setup()
{
    Serial.begin(9600);
}

void loop()
{
    controller.readSelectPin();
    getState();
}

void getState()
{
    currentState = 0;

    buffer = Serial.readStringUntil('\n');

    if (buffer[0] == '+') { currentState |= SC_CTL_ON; }
    if (buffer[1] == 'U') { currentState |= SC_BTN_UP; }
    if (buffer[2] == 'D') { currentState |= SC_BTN_DOWN; }
    if (buffer[3] == 'L') { currentState |= SC_BTN_LEFT; }
    if (buffer[4] == 'R') { currentState |= SC_BTN_RIGHT; }
    if (buffer[5] == 'S') { currentState |= SC_BTN_START; }
    if (buffer[6] == 'A') { currentState |= SC_BTN_A; }
    if (buffer[7] == 'B') { currentState |= SC_BTN_B; }
    if (buffer[8] == 'C') { currentState |= SC_BTN_C; }
    if (buffer[9] == 'X') { currentState |= SC_BTN_X; }
    if (buffer[10] == 'Y') { currentState |= SC_BTN_Y; }
    if (buffer[11] == 'Z') { currentState |= SC_BTN_Z; }
    if (buffer[12] == 'M') { currentState |= SC_BTN_MODE; }

    // Only update the controller if it's changed
    if (currentState != lastState)
    {
        controller.setState(currentState);
        lastState = currentState;
    }
}
