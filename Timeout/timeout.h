#ifndef __TIMEOUT_H__
#define __TIMEOUT_H__

#include <pico/stdlib.h>


void TIMEOUT_setTimeout_ms(uint32_t time, alarm_callback_t callback);
void TIMEOUT_reset();
void TIMEOUT_cancel();
void TIMEOUT_stop();


#endif