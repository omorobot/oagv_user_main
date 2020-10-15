#include <mcp2515.h>
#include "src/omoduino/r1_driver.h"
#include "src/omoduino/sonar.h"
#include "src/oagv_user/oagv_user.h"
#include "src/oagv_conveyor/oagv_conveyor.h"
#include "src/oagv_station/oagv_station.h"
#include "src/oagv_user/oagv_button.h"
#include "src/oagv_user/oagv_signal_lamp.h"


MCP2515           mcp2515(53);      //Initialize CAN bus with SS pin D53
OMOROBOT_R1       r1(&mcp2515);     //Initialize R1 with MCP2515 as external reference
OAGV_USER         user;             //Initialize User interface (LCD display, Keypad)
OAGV_BUTTON       buttons;          //Buttons object to monitor user input
OAGV_SIGNAL_LAMP  signal;           //Signal lamp object to notify user
OAGV_CONVEYOR     conv(&mcp2515);   //Initialize Conveyor and Lift controller with MCP2515 as external reference
OAGV_STATION      depot;            //Depot(Point of Loading) process
OAGV_STATION      pou;              //POU (Point of Unloading) process

struct can_frame  canRxMsg;

const int         PIN_TRIGGER1      = 64;  //A10
const int         PIN_ECHO1         = 65;  //A11
const int         PIN_TRIGGER2      = 66;  //A12
const int         PIN_ECHO2         = 67;  //A13

SONAR             sonar_L = SONAR(PIN_TRIGGER1, PIN_ECHO1);
SONAR             sonar_R = SONAR(PIN_TRIGGER2, PIN_ECHO2);

uint64_t          sonar_update_millis_last = millis();
double            sonar_distance_L  = 0.0;
double            sonar_distance_R  = 0.0;
int               sonar_read_state  = 0;

const int         PIN_STATUS_LED    = 48;
uint64_t          status_led_update_millis_last = millis();
uint64_t          process_update_millis_last = millis();
char              disp_info[20];

//Whenever the user pressed a button
void newUserButton_event(Button_Event event)
{
   switch (event)
   {
   case BTN_A_Pressed:                 // Go button pressed
      Serial.println("BTN_A pressed");
      user.set_info_str("BTN STOP");
      r1.stop();
      Serial1.print("$NONE\r\n"); 
      break;
   case BTN_B_Pressed:                 // Stop button pressed
      Serial.println("BTN_B pressed");
      user.set_info_str("BTN GO");
      r1.go(1500);
      //Serial1.print("$GO\r\n");
      break;
   case BTN_C_Pressed:
      break;
   case BTN_D_Pressed:
      break;   
   default:
      break;
   }
}
/// For any new data from motor driver module
void newR1_message_event(R1_MessageType msgType) 
{
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
// For any new tag data received from Line detector module.
void newR1_TagRead_event(Tag_Struct tag) 
{
   if(tag.type!=TAG_None)
   {
      sprintf(disp_info, "TAG:%02X,%02X,%02X,%02X",tag.bytes[0], tag.bytes[1], tag.bytes[2], tag.bytes[3]);
      user.set_info_str(disp_info);
   }
   switch(tag.type) {
   case TAG_None:
      break;
   case TAG_DEPOT:
      if(tag.bytes[2] == depot.get_id_num()) {
         depot.set_state_num(1);
         r1.go(100);
      }
      break;
   case TAG_POU:
      if(tag.bytes[2] == pou.get_id_num()) {
         if(tag.bytes[1] == 1) {
            pou.set_station_dir(station_dir_left);
         }else if(tag.bytes[1] == 2) {
            pou.set_station_dir(station_dir_right);
         } else {
            pou.set_station_dir(station_dir_none);
         }
         r1.pause();
         pou.set_state_num(1);
      }
      break;
   case TAG_APPROACH:
      if(tag.bytes[2] == pou.get_id_num()) {
         r1.go(100);
      }
      break;
   case TAG_TURN:
      break;
   case TAG_CIN:
      break;
   case TAG_COUT:
      break;
   case TAG_SPEED:
      r1.go(tag.bytes[2]*10);
      break;
   case TAG_SONAR:
      break;
   case TAG_READY:
      r1.stop();
      break;
   }
}
// Handle user input event from Keypad
void newOAGV_User_event(User_Event event) {
   switch(event) {
   case OAGV_GO_EMPTY:                 //Just go without any station assigned
      r1.go(250);
      break;
   case OAGV_GO_WITH_STATION:          //Station number is set from user input
      pou.set_id_num(user.get_pou_num());
      depot.set_id_num(user.get_depot_num());
      r1.go(250);
      Serial.print("GO DEPOT: ");
      Serial.print(user.get_depot_num());
      Serial.print(" POU: ");
      Serial.println(user.get_pou_num());
      break;
   case OAGV_STOP:                     //Stop commanded from user input
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
   //Serial.print("CAN:");
   //Serial.print(rxMsg.can_id);
   switch(senderID) {
   case 0x02:     //From LINE Sensor
      r1.new_can_line(rxMsg);
      break;
   case 0x04:     //From Motor Driver
      r1.new_can_odo(rxMsg);
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
   case 1:        //Stop the vehicle and wait
      r1.pause();
      if(depot.add_wait_cnt() > 100) {
         depot.set_state_num(2);
         depot.reset_wait_timer();
      }
      break;
   case 2:        //Move conveyor for loading boxes
      conv.set_mode(Conveyor_loading);
      depot.set_state_num(3);
      break;
   case 3:        //Wait until loading job is finished
      if(conv.is_finished()) {
         depot.set_state_num(4);
      }
      break;
   case 4:        //Start the vehicle
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
   case 1:        //Stop the vehicle and wait
      r1.pause();
      if(pou.add_wait_cnt() > 100) {
         pou.set_state_num(2);
         pou.reset_wait_timer();
      }
      break;
   case 2:        //Move conveyor per direction L/R
      if(pou.get_station_dir() == station_dir_left) {
         conv.set_mode(Conveyor_unload_left);
      } else if(pou.get_station_dir() == station_dir_right) {
         conv.set_mode(Conveyor_unload_right);
      } else {
         conv.set_mode(Conveyor_loading);
      }
      pou.set_state_num(3);
      break;
   case 3:        //Wait for conveyor job to be finished
      if(conv.is_finished()) {
         conv.set_mode(Conveyor_stop);
         pou.set_state_num(4);
      }
      break;
   case 4:        //Start move
      r1.go(300);
      pou.set_state_num(0);   //Reset state
      break;
   }
}

void setup() {
   // put your setup code here, to run once:
   Serial.begin(115200);
   Serial1.begin(115200);
   SPI.begin();
   mcp2515.reset();
   mcp2515.setBitrate(CAN_500KBPS);
   mcp2515.setNormalMode();
   Serial.println("CAN setup");
   pinMode(PIN_STATUS_LED, OUTPUT);
   /// Setup for Motor Controller:
   r1.set_driveMode(R1_VEHICLE_TYPE_PL153, R1DRV_LineTracerMode);
   //r1.set_vehicle_type(R1_VEHICLE_TYPE_PL153);
   //r1.set_drive_direction(Drive_Reverse, Line_Reverse);  //Motor driver reversed, Line sensor facing rear
   r1.set_lineoutTime(2000);
   r1.onNewData(newR1_message_event);
   r1.onNewTag(newR1_TagRead_event);
   r1.set_v_accel(50);
   r1.begin();

   ///Setup for user interface (LCD, Keypad input):
   user.onNewEvent(newOAGV_User_event);
   user.set_depot_max_num(10);
   user.set_pou_max_num(100);
   user.begin();
   user.set_logo_str("OAGV-PL");          // LOGO on top left display (11 characters max)
   
   ///Setup for other devices:
   buttons.on_NewEvent(newUserButton_event);
   conv.onNewEvent(newConveyor_event);

   ///Setup for sonar detection range
   sonar_L.set_range(60.0);
   sonar_R.set_range(60.0);
   ///Setup for signal lamp, buzzer
   signal.reset();
   signal.SetSignal(Signal_Green, Blink_Slow);
   digitalWrite(PIN_STATUS_LED, LOW);
}

void loop() {
   // put your main code here, to run repeatedly:
   if (mcp2515.readMessage(&canRxMsg) == MCP2515::ERROR_OK) {
      process_new_can_frame(canRxMsg);
   }
   r1.spin();
   user.spin();
   buttons.update();
   conv.spin();
   signal.spin();
  
   if(millis() - sonar_update_millis_last > 199) {    //For every 200ms
      loop_update_sonar();
      //Check if anything detected in the sensor
      if(sonar_L.detected()) {
         //r1.pause();
         user.set_sonar_distance(sonar_distance_L);
      } else if(sonar_R.detected()) {
         user.set_sonar_distance(sonar_distance_R);
         //r1.pause(); 
      } else {
         user.set_sonar_distance(sonar_distance_L);
         //Robot is cleared
         if(r1.is_going()) {
            r1.go();
         }
      }
      sonar_update_millis_last = millis();
   }
  
   if(millis() - process_update_millis_last > 9) {

   }
   if(millis() - status_led_update_millis_last > 499) {
      digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
      status_led_update_millis_last = millis();
   }
}
