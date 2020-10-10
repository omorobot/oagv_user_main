/**
 * @file oagv_signal_lamp.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _OAGV_SIGNAL_LAMP_H_
#define _OAGV_SIGNAL_LAMP_H_

#include <inttypes.h>

#define PIN_LAMP_R      4
#define PIN_LAMP_Y      5
#define PIN_LAMP_G      6
#define PIN_LAMP_BUZ    7

enum SignalColor_Type
{
    Signal_Red ,
    Signal_Yellow,
    Signal_Green 
};
enum SignalBlink_Type
{
    Blink_Off = 0,
    Blink_Solid,
    Blink_Slow,         //Blink every 1000ms
    Blink_Fast          //Blink every 500ms
};

class OAGV_SIGNAL_LAMP
{
public:

    OAGV_SIGNAL_LAMP();
    void reset(void);
    void SetSignal(SignalColor_Type color, SignalBlink_Type blink);
    void SetAlternating(SignalColor_Type color1, SignalColor_Type color2, uint16_t interval);
    void BuzzerOn(void);
    void BuzzerOff(void);
    void spin(void);
private:
    typedef struct {
        uint8_t             pin;
        bool                isOn;
        uint16_t            interval;
    }SignalLampType;

    SignalLampType lampR;
    SignalLampType lampY;
    SignalLampType lampG;
    uint8_t buzzerPin;
    bool    buzzerOn;
    uint64_t        update_last_millis;
    uint32_t        update_cnt;
    void lamp_init(SignalLampType *lamp, int pin);
    void update_lamp_500ms(SignalLampType *lamp, uint32_t cnt);
    void lamp_off(SignalLampType *lamp);
};
#endif