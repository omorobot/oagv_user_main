#include "oagv_station.h"

OAGV_STATION::OAGV_STATION()
{
    wait_timer = 0;
    state_num = 0;
}

uint16_t OAGV_STATION::get_state_num(void)
{
    return state_num;
}
void OAGV_STATION::set_state_num(uint16_t num) 
{
    state_num = num;
}
/**
 * @brief reset wait timer
 * */
void OAGV_STATION::reset_wait_timer(void)
{
    wait_timer = 0;
}
/**
 * @brief add 1 to wait timer
 * */
uint16_t OAGV_STATION::add_wait_cnt(void)
{
    wait_timer++;
    return wait_timer;
}

void OAGV_STATION::set_station_dir(StationDirType dir)
{
    station_dir = dir;
}
StationDirType OAGV_STATION::get_station_dir()
{
    return station_dir;
}

void OAGV_STATION::set_id_num(uint16_t id)
{
    id_num = id;
}

uint16_t OAGV_STATION::get_id_num(void)
{
    return id_num;
}