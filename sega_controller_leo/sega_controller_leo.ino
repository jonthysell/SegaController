/*
 * Sega Controller Reader (Keyboard)
 * Author: Jon Thysell <thysell@gmail.com>
 * Version: 1.1
 * Date: 9/29/2014
 *
 * Reads buttons presses from Sega Genesis 3/6 button controllers
 * and reports their state via keyboard button presses. Handles hot
 * swapping of controllers and auto-switches between 3 and 6 button
 * polling patterns.
 *
 */
 
const int PLAYERS = 2;
 
// Controller Button Flags
const int ON = 1;
const int UP = 2;
const int DOWN = 4;
const int LEFT = 8;
const int RIGHT = 16;
const int START = 32;
const int A = 64;
const int B = 128;
const int C = 256;
const int X = 512;
const int Y = 1024;
const int Z = 2048;
const int MODE = 4096;
 
// Controller DB9 Pin 7 Mappings
const int SELECT[] = { 8, 9 };
 
typedef struct
{
  int player;
  int pin;
  int lowFlag;
  int highFlag;
  int pulse3Flag;
} input;
 
// Controller DB9 Pin to Button Flag Mappings
// First column is the controller index, second column
// is the Arduino pin that the controller's DB9 pin is
// attached to
input inputMap[] = {
  { 0,  2,  UP,    UP,     Z    }, // P0 DB9 Pin 1
  { 0,  3,  DOWN,  DOWN,   Y    }, // P0 DB9 Pin 2
  { 0,  4,  ON,    LEFT,   X    }, // P0 DB9 Pin 3
  { 0,  5,  ON,    RIGHT,  MODE }, // P0 DB9 Pin 4
  { 0,  6,  A,     B,      0    }, // P0 DB9 Pin 6
  { 0,  7,  START, C,      0    }, // P0 DB9 Pin 9
  { 1,  A0, UP,    UP,     Z    }, // P1 DB9 Pin 1
  { 1,  A1, DOWN,  DOWN,   Y    }, // P1 DB9 Pin 2
  { 1,  A2, ON,    LEFT,   X    }, // P1 DB9 Pin 3
  { 1,  A3, ON,    RIGHT,  MODE }, // P1 DB9 Pin 4
  { 1,  A4, A,     B,      0    }, // P1 DB9 Pin 6
  { 1,  A5, START, C,      0    }  // P1 DB9 Pin 9
};
 
typedef struct
{
  int player;
  int flag;
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
 
// Controller State
int currentState[] = { 0, 0 };
int lastState[] = { -1, -1 };
 
// Default to three-button mode until six-button connects
boolean sixButtonMode[] = { false, false };
 
void setup()
{
  // Setup input pins
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    pinMode(inputMap[i].pin, INPUT);
    digitalWrite(inputMap[i].pin, HIGH);
  }
  
  // Setup select pins
  for (int i = 0; i < PLAYERS; i++)
  {
    pinMode(SELECT[i], OUTPUT);
    digitalWrite(SELECT[i], HIGH);
  }
  
  Keyboard.begin();
}
 
void loop()
{
  readButtons();
  sendStates();
}
 
void readButtons()
{
  for (int i = 0; i < PLAYERS; i++)
  {
    resetState(i);
    if (sixButtonMode[i])
    {
      read6buttons(i);
    }
    else
    {
      read3buttons(i);
    }
  }
}
 
void resetState(int player)
{
  currentState[player] = 0;
}
 
void read3buttons(int player)
{
  // Set SELECT LOW and read lowFlag
  digitalWrite(SELECT[player], LOW);
     
  delayMicroseconds(20);
     
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].lowFlag;
    }
  }
 
  // Set SELECT HIGH and read highFlag
  digitalWrite(SELECT[player], HIGH);
     
  delayMicroseconds(20);
     
  for (int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].highFlag;
    }
  }
    
  // When a six-button first connects, it'll spam UP and DOWN,
  // which signals the game to switch to 6-button polling
  if (currentState[player] == (ON | UP | DOWN))
  {
    sixButtonMode[player] = true;
  }
  // When a controller disconnects, revert to three-button polling
  else if ((currentState[player] & ON) == 0)
  {
    sixButtonMode[player] = false;
  }
  
  delayMicroseconds(20);
}
 
void read6buttons(int player)
{
  // Poll for three-button states twice
  read3buttons(player);
  read3buttons(player);
  
  // After two three-button polls, pulse the SELECT line
  // so the six-button reports the higher button states
  digitalWrite(SELECT[player], LOW);
  delayMicroseconds(20);
  digitalWrite(SELECT[player], HIGH);
  
  for(int i = 0; i < sizeof(inputMap) / sizeof(input); i++)
  {
    if (inputMap[i].player == player && digitalRead(inputMap[i].pin) == LOW)
    {
      currentState[player] |= inputMap[i].pulse3Flag;
    }
  }
  
  delayMicroseconds(1000);
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
