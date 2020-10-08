#include "oagv_button.h"


OAGV_BUTTON::OAGV_BUTTON()
{
    pinMode(PIN_SW_A, INPUT);
    pinMode(PIN_SW_B, INPUT);
    pinMode(PIN_SW_C, INPUT);
    pinMode(PIN_SW_D, INPUT);
}