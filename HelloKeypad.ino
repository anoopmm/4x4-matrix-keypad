#include "U8glib.h"
#include <Keypad.h>
U8GLIB_PCD8544 u8g(13, 11, 10, 12, A0);
const byte ROWS = 4; //four rows
const byte COLS = 4; //three columns
static byte kpadState;

unsigned long currentTimeInMill;
int timeDiff = 1500;//time diff for mutilple press
int keyIndex = 1;//index of first char of each key
int prevKeyValue;
String finalTextOut = "";//output text
String action = "";
bool longpress = false;
char keys[ROWS][COLS] = {
  { 0, 1, 2, 3 },
  { 4, 5, 6, 7 },
  { 8, 9, 10, 11 },
  { 12, 13, 14, 15 }
};
String keyStringArray[16] = {
  "1abcABC", "2defDEF", "3ghiGHI", "up",
  "4jklJKL", "5mnoMNO", "6pqrPQR", "left",
  "7stuSTU", "8vwxVWX", "9yz.YZ.", "right",
  "ok", "0+ -*#@", "cancel", "down",
};
int numbersArray[10] = {0, 1, 2, 4, 5, 6, 8, 9, 10, 13};
char* longPress[16] = {
  "", "", "", "",
  "", "", "", "",
  "", "", "", "",
  "", "mode", "", ""
};
char* keymodes[3] = {"no", "small", "cap"};
int keymodeIndex = 0;
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {9, 8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup() {
  Serial.begin(9600);
  keypad.addEventListener(keypadEvent);
  keypad.setHoldTime(1500);
  currentTimeInMill = millis();
}

void loop() {
  char key = keypad.getKey();
  char * cstr = new char [finalTextOut.length() + 1];
  cstr = finalTextOut.c_str();
  //delay(1000);
  Serial.println(finalTextOut);

  u8g.firstPage();
  do {
    u8g.setFont(u8g_font_unifont);
    //u8g.setFont(u8g_font_osb21);
    u8g.drawStr( 0, 22, cstr);
  } while ( u8g.nextPage() );

}
void getCharFromKey(int keyVal) {
  int modeRef = keymodeIndex == 2 ? 3 : 0;
  if ((millis() - currentTimeInMill) < timeDiff && prevKeyValue == keyVal) {
    keyIndex++;
    if (keyIndex == 4) {
      keyIndex = 1;
    }
    finalTextOut.remove(finalTextOut.length() - 1);
    finalTextOut = finalTextOut + keyStringArray[keyVal].substring(keyIndex + modeRef, keyIndex + modeRef + 1);
  } else {
    keyIndex = 1;
    finalTextOut = finalTextOut + keyStringArray[keyVal].substring(keyIndex + modeRef, keyIndex + modeRef + 1);
  }
  prevKeyValue = keyVal;
  currentTimeInMill = millis();
}
bool isAplhaNum(int keyVal) {
  int arrayIndex = 0;
  while (arrayIndex < 10) {
    if (numbersArray[arrayIndex] == keyVal) {
      return true;
    }
    arrayIndex++;
  }
  return false;
}
void getLongPressValue(int keyValue) {
  if (longPress[keyValue] == "mode") {
    keymodeIndex++;
    if (keymodeIndex == 3) {
      keymodeIndex = 0;

    }
  }
}
void keypadEvent(KeypadEvent key) {
  kpadState = keypad.getState();
  int keyVal = key;
  //Serial.println(keyVal);
  switch (kpadState) {
    case PRESSED:
      //Serial.println("pressed");
      break;

    case HOLD:
      longpress = true;
      break;

    case RELEASED:
      // Serial.println("release");
      if (longpress) {
        getLongPressValue(keyVal);
        longpress = false;
      } else {
        //Serial.println(keyStringArray[keyVal]);
        if (isAplhaNum(keyVal)) {
          if (keymodeIndex == 0) {
            finalTextOut = finalTextOut + keyStringArray[keyVal].substring(0, 1);
          } else if (keymodeIndex == 1) {
            getCharFromKey(keyVal);
          }
          else if (keymodeIndex == 2) {
            getCharFromKey(keyVal);
          }
        } else {
          action = keyStringArray[keyVal];
          if (action == "cancel") {
            finalTextOut.remove(finalTextOut.length() - 1);
          }

        }
      }

      break;
  }
}
