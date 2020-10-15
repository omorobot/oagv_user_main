#include "oagv_user.h"
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <mcp2515.h>


uint64_t    refresh_last_millis = millis();
//LiquidCrystal  
//  D22 --> rs
//  D23 --> rw
//  D24 --> enable
//  D25~D32 --> d0, d1, d2, d3, d4, d5, d6, d7
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {0x01,0x02,0x03,0x0A},
  {0x04,0x05,0x06,0x0B},
  {0x07,0x08,0x09,0x0C},
  {0x0E,0x10,0x0F,0x0D}   // Zero set to 0x10 not to be ignored
};
byte rowPins[rows] = {37, 38, 39, 40}; //connect to the row pinouts of the keypad
byte colPins[cols] = {33, 34, 35, 36}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

bool disp_online = false;
uint8_t disp_batLvl = 50;
uint8_t disp_bmp = 0;
uint8_t disp_sonar = 0;


char lcd_chars_logo[MAX_LOGO_SIZE] = "OAGV";
char lcd_chars_on[3] = "_ON";
char lcd_chars_off[3] = "OFF";

OAGV_USER::OAGV_USER() {
   lcd = new LiquidCrystal(22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32);
   keypad = new Keypad(makeKeymap(keys), rowPins, colPins, rows, cols );
   disp_state = disp_state_standby;
   station_1.type = 1;
   station_2.type = 2;
   info_update("Intialized!");
}

void OAGV_USER::onNewEvent(OAGV_NewUserEvent cbEvent) {
   _cbEvent = cbEvent;
}

void OAGV_USER::set_depot_max_num(uint16_t num) {
   station_1.num_max = num;
}
uint16_t OAGV_USER::get_depot_num(void) {
   return station_1.num_set;
}

void OAGV_USER::set_pou_max_num(uint16_t num) {
   station_2.num_max = num;
}
uint16_t OAGV_USER::get_pou_num(void)
{
   return station_2.num_set;
}

void OAGV_USER::begin() {
   lcd->begin(20, 4);
   lcd->clear();
   display_line1();
   display_line2();

   display_stationReset(&station_1);
   display_stationReset(&station_2);
   display_station(&station_1, false);
   display_station(&station_2, false);
}

void OAGV_USER::spin(void)
{
   char key = keypad->getKey();
   if(key) {
      key_in(key);
   } else if(millis() - refresh_last_millis > 24) {
      display_line1();
      display_line2();
      //display_info();
      if(disp_state == disp_state_A_in) {
         display_station(&station_1, true);
      } else if(disp_state == disp_state_B_in) {
         display_station(&station_2, true);
      } else {
         display_station(&station_1, false);
         display_station(&station_2, false);
      }
      refresh_last_millis = millis();
   }
}

void OAGV_USER::display_line1(void)
{
   lcd->setCursor(0,0);
   lcd->print(lcd_chars_logo);
   lcd->setCursor(12,0);
   if(disp_online) {
      lcd->print(lcd_chars_on);
   } else {
      lcd->print(lcd_chars_off);  
   }
   lcd->setCursor(15,0);
   lcd->print("_LINE");
}
void OAGV_USER::display_line2(void)
{
   lcd->setCursor(0,1);
   lcd->print("BAT:");
   lcd->print(disp_batLvl);
   lcd->setCursor(8,1);
   lcd->print("SN:");
   lcd->print(disp_sonar);
   lcd->print(" BP:");
   lcd->print(disp_bmp);
}
void OAGV_USER::display_info(void)
{
   lcd->setCursor(0, 3);
   lcd->print(_info_str);
}
int OAGV_USER::set_info_str(String str)
{
   if(str.length() > 20) {
      return -1;
   }
   info_update(str);
   return 0;
}
int OAGV_USER::set_info_1d(char* str, int x)
{
   char buf[20];
   sprintf(buf, "%s:%d, %d", str, x);
   Serial.println(buf);
   info_update(buf);
}
int OAGV_USER::set_info_2d(char* str, int x, int y)
{
   char buf[20];
   sprintf(buf, "%s:%d, %d", str, x, y);
   Serial.println(buf);
   info_update(buf);
}
void OAGV_USER::set_sonar_distance(int dist)
{
   disp_sonar = dist / 10;
}

int OAGV_USER::info_update(String str)
{
   int strlen = str.length();
   if(strlen > 20) return -1;
   int strlen_prev = _info_str.length();
   int blanks = strlen_prev - strlen;
   _info_str = str;
   lcd->setCursor(0, 3);
   lcd->print(_info_str);
   if(blanks > 0) {
      for(int i = 0; i<blanks; i++) {
         lcd->print(" ");
      }
   }
   return 0;
}
void OAGV_USER::display_stationReset(DispStation_TypeDef* station)
{
   station->num_set = 0;
   station->num_cnt = 0;
   station->is_set = false;
}
void OAGV_USER::display_station(DispStation_TypeDef* station, bool cursor_blink)
{
   if(station->type == 1) {
      lcd->setCursor(0,2);
      lcd->print("DEPOT:");
   } else if( station->type == 2) {
      lcd->setCursor(10,2);
      lcd->print("POU:");
   }
   if(station->num_set == 0) {
      lcd->print("___");
   }else if(station->num_set/100 > 0) {
      lcd->print(station->num_set);
   }else if(station->num_set/10 > 0) {
      lcd->print("_");
      lcd->print(station->num_set);
   }else {
      lcd->print("__");
      lcd->print(station->num_set);
   }
   if(cursor_blink) {
      lcd->blink();
   } else {
      lcd->noBlink();
   }
}
void OAGV_USER::display_station_numberSet(DispStation_TypeDef* station, uint8_t num_in)
{
   uint16_t temp_num = 0;
   switch(station->num_cnt) {
      case 0:
         if(num_in > station->num_max || num_in == 0) {
         station->num_set = 0;  
         } else {
         station->is_set = true;
         station->num_set = num_in;
         station->num_cnt++;
         }
      break;
      case 1:
         temp_num = station->num_set*10 + num_in;
         if(temp_num <= station->num_max) {
         station->num_set = station->num_set*10+num_in;
         station->num_cnt++;
         }
      break;
      case 2:
         temp_num = station->num_set*10 + num_in;
         if(temp_num <= station->num_max) {
         station->num_set = station->num_set*10+num_in;
         station->num_cnt = 0;
         }
      break;
   }
}
void OAGV_USER::key_in(char key)
{
   if(key == 0x0A) {       //Set station 1
      Serial.println("Key: A");
      disp_state = disp_state_A_in;
      display_stationReset(&station_1);
      display_station(&station_1, true);
   } 
   else if(key == 0x0B) {  //Set station 2
      Serial.println("Key: B");
      disp_state = disp_state_B_in;
      display_stationReset(&station_2);
      display_station(&station_2, true);
   } 
   else if(key == 0x0C) {
      Serial.println("Key: GO");
      if(!station_1.is_set&&!station_2.is_set) {
         _cbEvent(OAGV_GO_EMPTY);
         info_update("USER:Go empty");
      } else {
         _cbEvent(OAGV_GO_WITH_STATION);
         set_info_2d("Station", station_1.num_set, station_2.num_set);
      }
   }
   else if(key == 0x0D) {
      Serial.println("Key: STOP");
      _cbEvent(OAGV_STOP);
      info_update("USER: STOP");
   }
   else if( (key >0 && key < 0x0A) || key == 0x10) {    //Numeric button pressed
      if(key == 0x10) key = 0;
      if(disp_state == disp_state_A_in) {   //Currently in station 1 input mode
         display_station_numberSet(&station_1, (uint8_t)key);
         Serial.println(station_1.num_set);
         display_station(&station_1, true);
      } 
      else if(disp_state == disp_state_B_in) {  //Currently in station 2 input mode
         display_station_numberSet(&station_2, (uint8_t)key);
         Serial.println(station_2.num_set);
         display_station(&station_2, true);
      }
   } 
   else if(key == 0x0E) {  //Clear all items
      display_stationReset(&station_1);
      display_stationReset(&station_2);
      display_station(&station_1, false);
      display_station(&station_2, false);
      disp_state = disp_state_standby;
   }
   else if(key == 0x0F) {
   }
}

int OAGV_USER::set_logo_str(String str)
{
   if(str.length() > MAX_LOGO_SIZE) {   // String size Too long
      return 1;
   }
   else {
      str.toCharArray(lcd_chars_logo, MAX_LOGO_SIZE);
   }
   return 0;
}