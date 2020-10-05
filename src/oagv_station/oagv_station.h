/**
 * @file oagv_station.h 
 * @brief R1 driver library is for driving OMO-R1 robot along with other OMOROBOT's proprietary sensor modules using CAN transceiver such as MCP2515 
 * For more info, please visit www.omorobot.com 
 *  
 * @License
 * Copyright (c) OMOROBOT INC. All rights reserved. Copyright (c) Kyuhyong You. All rights reserved. 
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General Public License as published by the Free Software Foundation; either version 2.1 of the License, or (at your option) any later version.
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to the Free Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 * */
#ifndef _OAGV_STATION_H_
#define _OAGV_STATION_H_
#include <inttypes.h>

typedef enum StationDirType
{
    station_dir_none = 0,
    station_dir_left = 1,
    station_dir_right =2
};

class OAGV_STATION 
{
public:
    OAGV_STATION();
    uint16_t    get_state_num(void);
    void        set_state_num(uint16_t num);
    uint16_t    add_wait_cnt(void);
    void        reset_wait_timer(void);
    StationDirType get_station_dir(void);
    void        set_station_dir(StationDirType dir);
    uint16_t    get_id_num(void);
    void        set_id_num(uint16_t id);
private:
    uint16_t    wait_timer;
    uint16_t    state_num;
    StationDirType  station_dir;
    uint16_t    id_num;
};
#endif