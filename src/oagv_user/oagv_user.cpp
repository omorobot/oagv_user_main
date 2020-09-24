#include "oagv_user.h"
#include <LiquidCrystal.h>
#include <Keypad.h>

//LiquidCrystal  
//  D22 --> rs
//  D23 --> rw
//  D24 --> enable
//  D25~D32 --> d0, d1, d2, d3, d4, d5, d6, d7
LiquidCrystal lcd(22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32);

const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {0x01,0x02,0x03,0x0A},
  {0x04,0x05,0x06,0x0B},
  {0x07,0x08,0x09,0x0C},
  {0x0E,0x00,0x0F,0x0D}
};
byte rowPins[rows] = {37, 38, 39, 40}; //connect to the row pinouts of the keypad
byte colPins[cols] = {33, 34, 35, 36}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

char lcd_chars_logo[12] = "OAGV";
char lcd_chars_on[3] = "_ON";
char lcd_chars_off[3] = "OFF";

OAGV_USER::OAGV_USER() {
    
}