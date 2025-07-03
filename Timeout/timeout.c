#include "timeout.h"

static alarm_id_t id = -1;
static uint32_t timeout;
static alarm_callback_t callback_func;

void TIMEOUT_setTimeout_ms(uint32_t time, alarm_callback_t callback){
    timeout = time;
    callback_func = callback;
    id = add_alarm_in_ms(time, callback, NULL, false);
}

void TIMEOUT_reset(){
    if (id < 0) return;
    cancel_alarm(id);
    id = add_alarm_in_ms(timeout, callback_func, NULL, false);
}

void TIMEOUT_cancel(){
    cancel_alarm(id);
    TIMEOUT_stop();
}

void TIMEOUT_stop(){
    id = -1;
}