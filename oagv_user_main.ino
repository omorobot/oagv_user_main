#include <mcp2515.h>
#include "src/omoduino/r1_driver.h"
#include "src/omoduino/sonar.h"
#include "src/oagv_user/oagv_user.h"
#include "src/oagv_conveyor/oagv_conveyor.h"
#include "src/oagv_station/oagv_station.h"



MCP2515 mcp2515 = new MCP2515(53);    //Initialize CAN bus with SS pin D53

OMOROBOT_R1   r1(&mcp2515);   //Initialize R1 with MCP2515 as external reference
OAGV_USER     user;           //Initialize User interface (LCD display, Keypad)
OAGV_CONVEYOR conv(&mcp2515); //Initialize Conveyor and Lift controller with MCP2515 as external reference

OAGV_STATION depot;
OAGV_STATION pou;

struct can_frame canRxMsg;
struct can_frame canTxMsg;

const int PIN_TRIGGER1  = 55;  //A10
const int PIN_ECHO1     = 56;  //A11
const int PIN_TRIGGER2  = 57;  //A12
const int PIN_ECHO2     = 58;  //A13

SONAR sonar_L = SONAR(PIN_TRIGGER1, PIN_ECHO1);
SONAR sonar_R = SONAR(PIN_TRIGGER2, PIN_ECHO2);
uint64_t  sonar_update_millis_last = millis();
double    sonar_distance_L = 0.0;
double    sonar_distance_R = 0.0;
int       sonar_read_state = 0;


const int PIN_STATUS_LED    = 48;
uint64_t  status_led_update_millis_last = millis();

uint64_t  process_update_millis_last = millis();

void newR1_message_event(R1_MessageType msgType) {
   switch (msgType) {
   case R1MSG_ODO:
      break;
   case R1MSG_LINEPOS:
      break;
   case R1MSG_LINEOUT:
      break;
   default:
      break;
   }
}
// Handle user input event from Keypad
void newOAGV_User_event(User_Event event) {
   switch(event) {
   case OAGV_GO_EMPTY:
      r1.go(250);
      Serial.print("Empty GO");
      break;
   case OAGV_GO_WITH_STATION:
      mcp2515.sendMessage(&canTxMsg);
      pou.set_id_num(user.get_pou_num());
      depot.set_id_num(user.get_depot_num());
      r1.go(250);
      Serial.print("GO DEPOT: ");
      Serial.print(user.get_depot_num());
      Serial.print(" POU: ");
      Serial.println(user.get_pou_num());
      break;
   case OAGV_STOP:
      r1.stop();
      break;
   }
}
// Handle new conveyor event
void newConveyor_event(ConveyorEventType event) {
  switch(event) {
  case Conveyor_unload_finished:
    break;
  case Conveyor_lift_finished:
    break;
  }
}
void loop_update_sonar()
{
   // Read left and right sonar measurement
   if(sonar_read_state++%2) {
      sonar_distance_L = sonar_L.measure_cm();
   } else {
      sonar_distance_R = sonar_R.measure_cm();
   }
}
// For any new can message from nodes
void process_new_can_frame(struct can_frame rxMsg)
{
   int senderID = (rxMsg.can_id>>4)&0x0F;
   int dlc = rxMsg.can_dlc;
   switch(senderID) {
   case 0x02:     //From LINE Sensor
      r1.can_linePos(rxMsg);
      break;
   case 0x04:     //From Motor Driver
      r1.can_odo(rxMsg);
      break;
   case 0x05:     //From conveyor module
      conv.new_can_frame(rxMsg);
      break;
   }
}
// Process Point of Loading (DEPOT) station
void process_depot()
{
   switch (depot.get_state_num())
   {
   case 0:
      
      break;
   case 1:
      r1.pause();
      if(depot.add_wait_cnt() > 100) {
         depot.set_state_num(2);
      }
      break;
   case 2:
      conv.set_mode(Conveyor_loading);
      depot.reset_wait_timer();
      depot.set_state_num(3);
      break;
   case 3:
      if(conv.is_finished()) {
         depot.set_state_num(4);
      }
      break;
   case 4:
      depot.set_state_num(0);
      r1.go(300);
      break;
   default:
      break;
   }
}
// Process Point of Unloading(POU) station
void process_pou()
{
   switch(pou.get_state_num()) {
   case 0:
      break;
   case 1:
      r1.pause();
      if(pou.add_wait_cnt() > 100) {
         pou.set_state_num(2);
         pou.reset_wait_timer();
      }
      break;
   case 2:
      if(pou.get_station_dir() == station_dir_left) {
         conv.set_mode(Conveyor_unload_left);
      } else if(pou.get_station_dir() == station_dir_right) {
         conv.set_mode(Conveyor_unload_right);
      } else {
         conv.set_mode(Conveyor_loading);
      }
      pou.set_state_num(3);
      break;
   case 3:
      if(conv.is_finished()) {
         conv.set_mode(Conveyor_stop);
         pou.set_state_num(4);
      }
      break;
   case 4:
      r1.go(300);
      pou.set_state_num(0);
      break;
   }
}


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_STATUS_LED, OUTPUT);
  r1.set_driveMode(R1DRV_LineTracer);
  r1.set_lineoutTime(2000);
  r1.onNewData(newR1_message_event);
  r1.begin();
  
  user.onNewEvent(newOAGV_User_event);
  user.set_depot_max_num(10);
  user.set_pou_max_num(100);
  user.begin();

  conv.onNewEvent(newConveyor_event);
  // Set detection range for sonar
  sonar_L.set_range(60.0);
  sonar_R.set_range(60.0);
  digitalWrite(PIN_STATUS_LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  if (mcp2515.readMessage(&canRxMsg) == MCP2515::ERROR_OK) {
     process_new_can_frame(canRxMsg);
  }
  r1.spin();
  user.spin();
  conv.spin();
  /*
  if(millis() - sonar_update_millis_last > 19) {    //For every 20ms
    loop_update_sonar();
    //Check if anything detected in the sensor
    if(sonar_L.detected() || sonar_R.detected()) {
      r1.pause();
    } else {
    //Robot is cleared
    if(r1.is_going()) {
      r1.go();
      }
    }
    sonar_update_millis_last = millis();
  }
  */
  if(millis() - process_update_millis_last > 9) {

  }
  if(millis() - status_led_update_millis_last > 499) {
    digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
    status_led_update_millis_last = millis();
  }
}
