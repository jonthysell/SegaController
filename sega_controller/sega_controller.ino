/*
 * Sega Controller Reader
 * Author: Jon Thysell <thysell@gmail.com>
 * Version: 1.0
 * Date: 7/26/2014
 *
 * Reads buttons presses from Sega Genesis 3/6 button controllers
 * and reports their state via the Serial connection. Handles hot
 * swapping of controllers and auto-switches between 3 and 6 button
 * polling patterns.
 *
 */
 
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
  { 0,  2,  UP,    UP,     Z}, // P0 DB9 Pin 1
  { 0,  3,  DOWN,  DOWN,   Y}, // P0 DB9 Pin 2
  { 0,  4,  ON,    LEFT,   X}, // P0 DB9 Pin 3
  { 0,  5,  ON,    RIGHT,  0}, // P0 DB9 Pin 4
  { 0,  6,  A,     B,      0}, // P0 DB9 Pin 6
  { 0,  7,  START, C,      0}, // P0 DB9 Pin 9
  { 1,  A0, UP,    UP,     Z}, // P1 DB9 Pin 1
  { 1,  A1, DOWN,  DOWN,   Y}, // P1 DB9 Pin 2
  { 1,  A2, ON,    LEFT,   X}, // P1 DB9 Pin 3
  { 1,  A3, ON,    RIGHT,  0}, // P1 DB9 Pin 4
  { 1,  A4, A,     B,      0}, // P1 DB9 Pin 6
  { 1,  A5, START, C,      0}  // P1 DB9 Pin 9
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
  for (int i = 0; i < 2; i++)
  {
    pinMode(SELECT[i], OUTPUT);
    digitalWrite(SELECT[i], HIGH);
  }
   
  Serial.begin(9600);
}
 
void loop()
{
  readButtons();
  sendStates();
}
 
void readButtons()
{
  for (int i = 0; i < 2; i++)
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
  // Only report controller states if at least one has changed
  boolean hasChanged = false;
   
  for (int i = 0; i < 2; i++)
  {
    if (currentState[i] != lastState[i])
    {
      hasChanged = true;
    }
  }
   
  if (hasChanged)
  { 
    for (int i = 0; i < 2; i++)
    {
      Serial.print((currentState[i] & ON) == ON ? "+" : "-");
      Serial.print((currentState[i] & UP) == UP ? "U" : "0");
      Serial.print((currentState[i] & DOWN) == DOWN ? "D" : "0");
      Serial.print((currentState[i] & LEFT) == LEFT ? "L" : "0");
      Serial.print((currentState[i] & RIGHT) == RIGHT ? "R" : "0");
      Serial.print((currentState[i] & START) == START ? "S" : "0");
      Serial.print((currentState[i] & A) == A ? "A" : "0");
      Serial.print((currentState[i] & B) == B ? "B" : "0");
      Serial.print((currentState[i] & C) == C ? "C" : "0");
      Serial.print((currentState[i] & X) == X ? "X" : "0");
      Serial.print((currentState[i] & Y) == Y ? "Y" : "0");
      Serial.print((currentState[i] & Z) == Z ? "Z" : "0");
         
      Serial.print((i == 0) ? "," : "\n");
      lastState[i] = currentState[i];
    }
  }
}

