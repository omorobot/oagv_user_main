#include "oagv_signal_lamp.h"
#include <Arduino.h>

OAGV_SIGNAL_LAMP::OAGV_SIGNAL_LAMP()
{
   lamp_init(&lampR, PIN_LAMP_R);
   lamp_init(&lampY, PIN_LAMP_Y);
   lamp_init(&lampG, PIN_LAMP_G);
   pinMode(PIN_LAMP_BUZ, OUTPUT);
}
void OAGV_SIGNAL_LAMP::reset()
{
   lamp_off(&lampR);
   lamp_off(&lampY);
   lamp_off(&lampG);
   digitalWrite(PIN_LAMP_BUZ, LOW);
}
void OAGV_SIGNAL_LAMP::SetSignal(SignalColor_Type color, SignalBlink_Type blink)
{
   SignalLampType *lamp;
   switch(color) {
      case Signal_Red:    lamp = &lampR; break;
      case Signal_Yellow: lamp = &lampY; break;
      case Signal_Green:  lamp = &lampG; break;
   }
   switch(blink) {
      case Blink_Off: lamp->isOn = false; break;
      case Blink_Solid: 
         lamp->isOn = true;
         lamp->interval = 0;
         break;
      case Blink_Slow:
         lamp->isOn = true;
         lamp->interval = 2;
         break;
      case Blink_Fast:
         lamp->isOn = true;
         lamp->interval = 1;
         break;
   }

}
void OAGV_SIGNAL_LAMP::spin()
{
   if(millis()-update_last_millis > 499) {
      update_cnt++;
      update_lamp_500ms(&lampR, update_cnt);
      update_lamp_500ms(&lampY, update_cnt);
      update_lamp_500ms(&lampG, update_cnt);
      update_last_millis = millis();
   }
}
// Initialize lamp object and set as output
void OAGV_SIGNAL_LAMP::lamp_init(SignalLampType *lamp, int pin)
{
   lamp->pin = pin;
   pinMode(pin, OUTPUT);
   lamp->isOn = false;
   lamp->interval = 0;
}
// For every 500ms update lamp
void OAGV_SIGNAL_LAMP::update_lamp_500ms(SignalLampType *lamp, uint32_t cnt)
{
   if(lamp->isOn) {
      if(lamp->interval > 0) {
         if(cnt%lamp->interval == 0) {
               digitalWrite(lamp->pin, !digitalRead(lamp->pin));
         }
      } else {
         digitalWrite(lamp->pin, HIGH);
      }
   } else {
      digitalWrite(lamp->pin, LOW);
   }
}
void OAGV_SIGNAL_LAMP::lamp_off(SignalLampType *lamp)
{
   lamp->isOn = false;
   digitalWrite(lamp->pin, LOW);
}