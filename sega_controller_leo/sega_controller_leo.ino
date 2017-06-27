// 
// sega_controller_leo.ino
//  
// Author:
//       Jon Thysell <thysell@gmail.com>
// 
// Copyright (c) 2014, 2017 Jon Thysell <http://jonthysell.com>
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
 
const int PLAYERS = 2;
const int INPUT_PINS_PER_PLAYER = 6;

// Mapping controller DB9 pin 7 to Arduino pin
const int SELECT[] = {
  8, // Player 1
  9 // Player 2
};

// Mapping controller DB9 input pins to Arduino pins
int inputPins[][INPUT_PINS_PER_PLAYER] = {
  // { DB9 Pin 1, DB9 Pin 2, DB9 Pin 3, DB9 Pin 4, DB9 Pin 6, DB9 Pin 9 }
  { 2, 3, 4, 5, 6, 7 }, // Player 1
  { A0, A1, A2, A3, A4, A5 } // Player 2
};

// Controller button flags
enum flags {
  ON = 1,
  UP = 2,
  DOWN = 4,
  LEFT = 8,
  RIGHT = 16,
  START = 32,
  A = 64,
  B = 128,
  C = 256,
  X = 512,
  Y = 1024,
  Z = 2048,
  MODE = 4096
};
 
typedef struct
{
  int player;
  flags flag;
  char key;
} output;
 
// Controller Button Flag to Keyboard Mappings
// First column is the controller index, second column
// is the button flag, third is keyboard key
output outputMap[] = {
  { 0, UP,    KEY_UP_ARROW },
  { 0, DOWN,  KEY_DOWN_ARROW },
  { 0, LEFT,  KEY_LEFT_ARROW },
  { 0, RIGHT, KEY_RIGHT_ARROW },
  { 0, START, KEY_RETURN },
  { 0, A,     'z' },
  { 0, B,     'x' },
  { 0, C,     'c' },
  { 0, X,     'a' },
  { 0, Y,     's' },
  { 0, Z,     'd' },
  { 0, MODE,  'q' },
  { 1, UP,    'i' },
  { 1, DOWN,  'k' },
  { 1, LEFT,  'j' },
  { 1, RIGHT, 'l' },
  { 1, START, 't' },
  { 1, A,     'v' },
  { 1, B,     'b' },
  { 1, C,     'n' },
  { 1, X,     'f' },
  { 1, Y,     'g' },
  { 1, Z,     'h' },
  { 1, MODE,  'r' }
};

// Controller states
int currentState[] = { 0, 0 };
int lastState[] = { -1, -1 };
 
// Default to three-button mode until six-button connects
boolean sixButtonMode[] = { false, false };

const int CYCLES = 8;
 
void setup()
{
  for (int player = 0; player < PLAYERS; player++)
  {
    // Setup select pin
    pinMode(SELECT[player], OUTPUT);
    digitalWrite(SELECT[player], HIGH);

    // Setup input pins
    for (int pin = 0; pin < INPUT_PINS_PER_PLAYER; pin++)
    {
      pinMode(inputPins[player][pin], INPUT);
      digitalWrite(inputPins[player][pin], HIGH);
    }
  }
  
  Keyboard.begin();
}
 
void loop()
{
  unsigned long loopStart = millis();
  
  readButtons();
  sendStates();
  
  // Delay up to 16ms in order to simulate one frame @ 60 fps
  delay(max(16 - (millis() - loopStart), 0));
}
 
void readButtons()
{
  for (int player = 0; player < PLAYERS; player++)
  {
    resetState(player);

    digitalWrite(SELECT[player], LOW);

    for (int cycle = 0; cycle < CYCLES; cycle++)
    {
      readButtons(player, cycle);
    }

    // When a controller disconnects, revert to three-button polling
    if ((currentState[player] & ON) == 0)
    {
      sixButtonMode[player] = false;
    }
  }
}
 
void resetState(int player)
{
  currentState[player] = 0;
}

void readButtons(int player, int cycle)
{
  digitalWrite(SELECT[player], cycle % 2 == 0 ? LOW : HIGH);
  
  // Read flags
  switch (cycle)
  {
    case 2:
      currentState[player] |= (digitalRead(inputPins[player][0]) == LOW) ? UP : 0;
      currentState[player] |= (digitalRead(inputPins[player][1]) == LOW) ? DOWN : 0;
      currentState[player] |= (digitalRead(inputPins[player][2]) == LOW) ? ON : 0;
      currentState[player] |= (digitalRead(inputPins[player][3]) == LOW) ? ON : 0;
      currentState[player] |= (digitalRead(inputPins[player][4]) == LOW) ? A : 0;
      currentState[player] |= (digitalRead(inputPins[player][5]) == LOW) ? START : 0;
      break;
    case 3:
      currentState[player] |= (digitalRead(inputPins[player][0]) == LOW) ? UP : 0;
      currentState[player] |= (digitalRead(inputPins[player][1]) == LOW) ? DOWN : 0;
      currentState[player] |= (digitalRead(inputPins[player][2]) == LOW) ? LEFT : 0;
      currentState[player] |= (digitalRead(inputPins[player][3]) == LOW) ? RIGHT : 0;
      currentState[player] |= (digitalRead(inputPins[player][4]) == LOW) ? B : 0;
      currentState[player] |= (digitalRead(inputPins[player][5]) == LOW) ? C : 0;
      break;
    case 4:
      sixButtonMode[player] = (digitalRead(inputPins[player][0]) == LOW && digitalRead(inputPins[player][1]) == LOW);
      break;
    case 5:
      if (sixButtonMode[player])
      {
        currentState[player] |= (digitalRead(inputPins[player][0]) == LOW) ? Z : 0;
        currentState[player] |= (digitalRead(inputPins[player][1]) == LOW) ? Y : 0;
        currentState[player] |= (digitalRead(inputPins[player][2]) == LOW) ? X : 0;
        currentState[player] |= (digitalRead(inputPins[player][3]) == LOW) ? MODE : 0;
      }
      break;
  }
}
 
void sendStates()
{
  for (int i = 0; i < sizeof(outputMap) / sizeof(output); i++)
  {
    int last = (lastState[outputMap[i].player] & outputMap[i].flag);
    int current = (currentState[outputMap[i].player] & outputMap[i].flag);
     
    if (last != current)
    {
      if (current == outputMap[i].flag)
      {
        Keyboard.press(outputMap[i].key);
      }
      else
      {
        Keyboard.release(outputMap[i].key);
      }
    }
  }
  
  for (int i = 0; i < PLAYERS; i++)
  {
    lastState[i] = currentState[i];
  }
}
