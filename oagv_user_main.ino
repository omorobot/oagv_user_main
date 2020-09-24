#include "src/omoduino/r1_driver.h"
#include "src/omoduino/sonar.h"
#include "src/oagv_user/oagv_user.h"

OMOROBOT_R1 r1(53);   //Initialize r1 with SS pin D53
OAGV_USER   user;

uint64_t timer_update_millis_last = millis();
uint16_t sec = 0;
uint16_t ms = 0;
char cbuff[20];

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
void newOAGV_User_event(User_Event event) {
   switch(event) {
   case OAGV_GO_EMPTY:
      break;
   case OAGV_GO_WITH_STATION:
      break;
   case OAGV_STOP:
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

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(PIN_STATUS_LED, OUTPUT);
  r1.set_driveMode(R1DRV_LineTracer);
  r1.set_lineoutTime(2000);
  r1.onNewData(newR1_message_event);
  r1.begin();
  
  user.onNewEvent(newOAGV_User_event);
  user.begin();
  
  // Set detection range for sonar
  sonar_L.set_range(60.0);
  sonar_R.set_range(60.0);
  digitalWrite(PIN_STATUS_LED, LOW);
}

void loop() {
  // put your main code here, to run repeatedly:
  r1.spin();
  user.spin();
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

  if(millis() - status_led_update_millis_last > 499) {
    digitalWrite(PIN_STATUS_LED, !digitalRead(PIN_STATUS_LED));
    status_led_update_millis_last = millis();
  }
  
  if(millis() - timer_update_millis_last > 99) {
    ms++;
    if(ms>9) { 
        ms = 0;
        sec++;
    }
    timer_update_millis_last = millis();
  }
}
