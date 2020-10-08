/**
 * @file oagv_button.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _OAGV_BUTTON_H_
#define _OAGV_BUTTON_H_

#include <inttypes.h>

#define PIN_SW_A      41
#define PIN_SW_B      42
#define PIN_SW_C      43
#define PIN_SW_D      44

enum Button_Event {
    BTN_A_Pressed,
    BTN_B_Pressed,
    BTN_C_Pressed,
    BTN_D_Pressed
};

class OAGV_BUTTON
{   
public:
    typedef struct {
        int pin;
        int state;
        int state_last;
    }ButtonType;
    typedef void (*OAGV_NewButtonEvent)(Button_Event);
    OAGV_BUTTON();
    void on_NewEvent(OAGV_NewButtonEvent cbEvent);
    void update(void);
private:
    OAGV_NewButtonEvent _cbEvent;
    ButtonType btnA;
    ButtonType btnB;
    ButtonType btnC;
    ButtonType btnD;
    void    button_init(ButtonType *btn, int pin, uint8_t pullup);
    int     check_button_pressed(ButtonType *btn);
};

#endif