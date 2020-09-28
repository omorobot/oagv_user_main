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

class MCP2515;

enum ConveyorModeType {
    Conv_mode_none = 0,
    Conv_mode_manual_loading = 1,
    Conv_mode_unloading_right = 2,
    Conv_mode_unloading_left = 3
};

class OAGV_CONVEYOR 
{
public:

    OAGV_CONVEYOR(MCP2515* mcp2515);

    void        set_mode(ConveyorModeType mode);


private:    
    MCP2515 *_mcp2515;
    
};

#endif