/**
 * @file oagv_user.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _OAGV_USER_H_
#define _OAGV_USER_H_

#include <Arduino.h>

#define MAX_LOGO_SIZE       11

class LiquidCrystal;
class Keypad;
class MCP2515;

enum User_Event {
   OAGV_GO_EMPTY,
   OAGV_GO_WITH_STATION,
   OAGV_STOP
};

class OAGV_USER
{   
public:
   typedef void (*OAGV_NewUserEvent)(User_Event);
   OAGV_USER();
   
   void        begin(void);
   void        onNewEvent(OAGV_NewUserEvent cbEvent);
   void        spin(void);
   void        set_depot_max_num(uint16_t num);
   void        set_pou_max_num(uint16_t num);
   uint16_t    get_depot_num(void);
   uint16_t    get_pou_num(void);
   int         set_logo_str(String str);
   int         set_info_str(String str);
   int         set_info_1d(char* str, int x);
   int         set_info_2d(char* str, int x, int y);
   void        set_sonar_distance(int dist);
private:
   typedef struct DispStation_TypeDef{
      uint16_t  num_max;
      uint8_t   type;
      uint16_t  num_set;
      uint8_t   num_cnt;
      bool      is_set;
   }DispStation_TypeDef;

   typedef enum {
      disp_state_standby = 0,
      disp_state_A_in = 1,
      disp_state_B_in = 2,
      disp_state_C_in = 3,
      disp_state_D_in = 4,
      disp_state_enter = 5
   }disp_State_Type;

   LiquidCrystal *lcd;
   Keypad *keypad;
   MCP2515 *canbus;

   DispStation_TypeDef station_1;
   DispStation_TypeDef station_2;
   disp_State_Type disp_state;
   OAGV_NewUserEvent _cbEvent;
   String _info_str;

   void    display_line1(void);
   void    display_line2(void);
   void    display_info(void);
   void    display_stationReset(DispStation_TypeDef* station);
   void    display_station(DispStation_TypeDef* station, bool cursor_blink);
   void    display_init(void);
   void    display_station_numberSet(DispStation_TypeDef* station, uint8_t num_in);
   void    key_in(char key);
   int     info_update(String str);
};

#endif
