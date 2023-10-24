#include <Key.h>
#include <Keypad.h>
#include <Joystick.h>
#include <Encoder.h>

//description - TODO

//define staff here
//general
#define MYDELAY 50
#define BUTTONLENGTH 25  //total buttons count
#define NUMROTARIES 4    //4 rotaries (2xdual)
//530/430 sitch pin
#define SWITCHPIN A5
#define SWITCHBUTTON 24

//button matrix definitions
#define NUMBUTTONS 16    //do we  ever use it?
#define NUMROWS 4
#define NUMCOLS 4

//create Joystick
Joystick_ Joystick(JOYSTICK_DEFAULT_REPORT_ID, JOYSTICK_TYPE_JOYSTICK,
                   BUTTONLENGTH, 0,    // Button Count, Hat Switch Count
                   false, false, false,   // No X, Y, or Z Axes
                   false, false, false,   // No Rx, Ry, or Rz
                   false, false,          // No rudder or throttle
                   false, false, false);  // No accelerator, brake, or steering

//define buttons matrix:
//joystick buttons matrix (starts with 0)
byte buttons[NUMROWS][NUMCOLS] = {
  { 13, 12, 7, 2 },
  { 11, 10, 6, 3 },
  {  9,  8, 5, 4 },
  { 15, 14, 1, 0 },
};
//related rows and columns pins
byte rowPins[NUMROWS] = { 6, 7, 8, 9 };
byte colPins[NUMCOLS] = { A1, A2, A3, A4};

//initialize result keypad
Keypad RevButtonBox1 = Keypad(makeKeymap(buttons), rowPins, colPins, NUMROWS, NUMCOLS);

int switchButtonState = 0;

//define rotaries set
//struct to map another encoder properties
struct rotariesdef {
  Encoder* encoder;
  int ccwchar;
  int cwchar;
  int state;
};

//todo: map encoders and generate this array - now it's hardcoded
//Encoder oinnage: Outer (a,b), Inner (A, B) - order is important.
rotariesdef rotaries[NUMROTARIES] {
{new Encoder( 2,  3), 16, 17, -999},
{new Encoder( 4,  5), 18, 19, -999},
{new Encoder(10, 11), 20, 21, -999},
{new Encoder(12, 13), 22, 23, -999},
};


void setup() {

  //define single button for 530/430 switch
  pinMode(SWITCHPIN, INPUT_PULLUP);

  //setup Joystick
  Joystick.begin();

  Serial.begin(9600); // DEBUG delete
}

void loop () {

  // ask 530/430 wsitch here (function needed)
  checkSwitch();
  //ask buttons in matrix
  CheckAllButtons();
  //ask rotaries
  askRotaries();
}

void checkSwitch(void){
  // read button state
  switchButtonState = digitalRead(SWITCHPIN);

  //process state to Joystick
  if (switchButtonState == LOW) {
    Joystick.setButton(SWITCHBUTTON, 1);
  } else {
    Joystick.setButton(SWITCHBUTTON, 0);
  }
}

void askRotaries(void){
  for ( int i = 0; i < NUMROTARIES; i++) {
    long newPosition = rotaries[i].encoder -> read() >> 1;
    // Serial.print("State: "); Serial.print(rotaries[i].state); Serial.print(". New pos: "); Serial.println(newPosition);
    if (newPosition != rotaries[i].state) {
      if (newPosition > rotaries[i].state) {
        Joystick.pressButton(rotaries[i].cwchar);  // function!
        delay(MYDELAY);
        Joystick.releaseButton(rotaries[i].cwchar);
      } else {
        Joystick.pressButton(rotaries[i].ccwchar);
        delay(MYDELAY);
        Joystick.releaseButton(rotaries[i].ccwchar);
      }
      rotaries[i].state = newPosition;
      // DEBUG output
      for (int i = 0; i < abs(newPosition); i++) {
            Serial.print(newPosition > 0 ? '|' : '_');
      }
      Serial.print('|');
      Serial.println(' ');
    }
  }
}

void CheckAllButtons(void) {
  // matrix read
  if (RevButtonBox1.getKeys()) {
    for (int i = 0; i < LIST_MAX; i++) {
      if (RevButtonBox1.key[i].stateChanged) {
        switch (RevButtonBox1.key[i].kstate) {
          case PRESSED:
          case HOLD:
            Joystick.setButton(RevButtonBox1.key[i].kchar, 1);
            break;
          case RELEASED:
          case IDLE:
            Joystick.setButton(RevButtonBox1.key[i].kchar, 0);
            break;
        }
      }
    }
  }
}