#include "oagv_conveyor.h"
#include <mcp2515.h>


struct can_frame txMsg;
const uint8_t conveyor_id = 5;
uint64_t update_millis_last = millis();

OAGV_CONVEYOR::OAGV_CONVEYOR(MCP2515* mcp2515) {
   _mcp2515 = mcp2515;
   txMsg.can_id = (1<<4)|conveyor_id;
   txMsg.can_dlc = 8;
   box_out_finished = false;
}
void OAGV_CONVEYOR::onNewEvent(NewConveyorEvent cbEvent)
{
   _cbEvent = cbEvent;
}
void OAGV_CONVEYOR::set_mode(ConveyorModeType mode)
{
   switch(mode) {
   case Conveyor_stop:
      txMsg.data[0] = CONV_CONTROL_STOP;
      break;
   case Conveyor_loading:
      txMsg.data[0] = CONV_LOADING;
      box_out_finished = false;
      break;
   case Conveyor_unload_left:
      txMsg.data[0] = CONV_UNLOAD_LEFT;
      box_out_finished = false;
      break;
   case Conveyor_unload_right:
      txMsg.data[0] = CONV_UNLOAD_RIGHT;
      box_out_finished = false;
      break;
   default:
      break;
   }
}

void OAGV_CONVEYOR::spin(void) 
{
   if((millis() - update_millis_last > 99)) {
      _mcp2515->sendMessage(&txMsg);
      update_millis_last = millis();
   }
}

void OAGV_CONVEYOR::new_can_frame(struct can_frame rxMsg)
{
   if(rxMsg.data[0] == CONV_UNLOAD_FINISHED) {
      _cbEvent(Conveyor_unload_finished);
      box_out_finished = true;
   }
}

bool OAGV_CONVEYOR::is_finished(void)
{
   return box_out_finished;
}