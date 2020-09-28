#include "oagv_conveyor.h"
#include <mcp2515.h>


struct can_frame txMsg_conv;
const uint8_t conveyor_id = 5;

OAGV_CONVEYOR::OAGV_CONVEYOR(MCP2515* mcp2515) {
    _mcp2515 = mcp2515;
    txMsg_conv.can_id = (1<<4)|conveyor_id;
    txMsg_conv.can_dlc = 8;
}