#include "oagv_button.h"
#include "Arduino.h"

OAGV_BUTTON::OAGV_BUTTON()
{
   button_init(&btnA, PIN_SW_A, INPUT_PULLUP);
   button_init(&btnB, PIN_SW_B, INPUT_PULLUP);
   button_init(&btnC, PIN_SW_C, INPUT_PULLUP);
   button_init(&btnD, PIN_SW_D, INPUT_PULLUP);
}

void OAGV_BUTTON::on_NewEvent(OAGV_NewButtonEvent cbEvent)
{
   _cbEvent = cbEvent;
}

void OAGV_BUTTON::update(void)
{
   if(check_button_pressed(&btnA)) _cbEvent(BTN_A_Pressed);
   if(check_button_pressed(&btnB)) _cbEvent(BTN_B_Pressed);
   if(check_button_pressed(&btnC)) _cbEvent(BTN_C_Pressed);
   if(check_button_pressed(&btnD)) _cbEvent(BTN_D_Pressed);
}

void OAGV_BUTTON::button_init(ButtonType *btn, int pin, uint8_t pullup)
{
   btn->pin = pin;
   if(pullup == INPUT_PULLUP) {
      pinMode(pin, INPUT_PULLUP);
   } else {
      pinMode(pin, INPUT);
   }
   
   btn->state = 0;
   btn->state_last = 0;
}

int OAGV_BUTTON::check_button_pressed(ButtonType *btn)
{
   btn->state = digitalRead(btn->pin);
   if(btn->state !=btn->state_last) {
      uint64_t diff_millis = millis() - btn->update_last_millis;
      if(diff_millis > 50) {
         btn->state_last = btn->state;
         btn->update_last_millis = millis();
         if(!btn->state) {
               return 1;
         }
      }
   }
   return 0;
}