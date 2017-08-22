//
// SegaControllerKeyboardReader.ino
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

#include <Keyboard.h>
#include <SegaController.h>

typedef struct
{
    word button;
    char key;
} output;

// Controller button to keyboard mapping
output outputMap[] = {
    { SC_BTN_UP,    KEY_UP_ARROW },
    { SC_BTN_DOWN,  KEY_DOWN_ARROW },
    { SC_BTN_LEFT,  KEY_LEFT_ARROW },
    { SC_BTN_RIGHT, KEY_RIGHT_ARROW },
    { SC_BTN_START, KEY_RETURN },
    { SC_BTN_A,     'z' },
    { SC_BTN_B,     'x' },
    { SC_BTN_C,     'c' },
    { SC_BTN_X,     'a' },
    { SC_BTN_Y,     's' },
    { SC_BTN_Z,     'd' },
    { SC_BTN_MODE,  'q' }
};

const byte BUTTONS = 12;

// Controller DB9 pins (looking face-on to the end of the plug):
//
// 5 4 3 2 1
//  9 8 7 6
//
// Connect pin 5 to +5V and pin 8 to GND
// Connect the remaining pins to digital I/O pins (see below)

// Specify the Arduino pins that are connected to
// DB9 Pin 7, DB9 Pin 1, DB9 Pin 2, DB9 Pin 3, DB9 Pin 4, DB9 Pin 6, DB9 Pin 9
SegaController controller(8, 2, 3, 4, 5, 6, 7);

// Controller states
word currentState = 0;
word lastState = 0;

void setup()
{
    Keyboard.begin();
}

void loop()
{
    currentState = controller.getState();
    sendStates();
}

void sendStates()
{
    for (byte i = 0; i < BUTTONS; i++)
    {
        word last = (lastState & outputMap[i].button);
        word current = (currentState & outputMap[i].button);

        if (last != current)
        {
            if (current)
            {
                Keyboard.press(outputMap[i].key);
            }
            else
            {
                Keyboard.release(outputMap[i].key);
            }
        }
    }

    lastState = currentState;
}