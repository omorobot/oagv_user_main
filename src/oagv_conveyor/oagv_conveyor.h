/**
 * @file oagv_conveyor.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _OAGV_CONVEYOR_H_
#define _OAGV_CONVEYOR_H_


#include <inttypes.h>

#define CONV_CONTROL_STOP       0xFF    ///Stop belt
#define CONV_LOADING            0x50    ///Belt will move when box detected
#define CONV_UNLOAD_LEFT        0x51    ///Belt will move left to unload box
#define CONV_UNLOAD_RIGHT       0x52    ///Belt will move right to unload box

#define CONV_UNLOAD_FINISHED    0x53


class MCP2515;

enum ConveyorModeType {
    Conveyor_stop =         0,  ///Belt will not move
    Conveyor_loading =      1,  ///Belt will move when box detected
    Conveyor_unload_left =  2,  ///Belt will move left to unload
    Conveyor_unload_right = 3   ///Belt will move right to unload
};
enum ConveyorEventType {
    Conveyor_event_none =       0,
    Conveyor_unload_finished =  1, ///Uloading finished
    Conveyor_lift_finished =    2, ///Left is finished motion
};

class OAGV_CONVEYOR 
{
public:
    typedef void (*NewConveyorEvent)(ConveyorEventType);
    OAGV_CONVEYOR(MCP2515* mcp2515);

    void        onNewEvent(NewConveyorEvent cbEvent);
    void        set_mode(ConveyorModeType mode);
    void        spin(void);
    void        new_can_frame(struct can_frame rxMsg);
    bool        is_finished(void);
private:    
    MCP2515 *_mcp2515;
    NewConveyorEvent _cbEvent;
    bool        box_out_finished;
};

#endif