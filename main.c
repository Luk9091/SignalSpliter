#include <pico/stdio.h>
#include <pico/stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#include <hardware/dma.h>

#include <FreeRTOS.h>
#include <task.h>
#include "FreeRTOSConfig.h"


#include "led.h"
#include "encoder.h"
#include "timeout.h"

#include "spi.h"

#include "draw.h"
#include "interface.h"


typedef enum SELECT_STATE{
    SELECT_NONE,
    SELECT_INPUT,
    SELECT_OUTPUT,
} SELECT_STATE_t;

static SELECT_STATE_t state = SELECT_NONE;
static int channel = 0;
static int channel_out0;
static int channel_out1;
static int out = 0;

void blink_task(__unused void *param){
    while (1){
        LED_toggle();
        vTaskDelay(pdTICKS_TO_MS(250));
    }
}


static inline void change_input_channel(int *channel, int rotation){
    INTERFACE_channel_frame(*channel, 0);
    *channel =(*channel + rotation) % 2; 
    INTERFACE_channel_frame(*channel, DISPLAY_BRIGHTNESS);
}


int64_t auto_disable(alarm_id_t id, __unused void *data){
    TIMEOUT_stop();

    int tmp;
    if (channel == 0){
        tmp = channel_out0;
    } else {
        tmp = channel_out1;
    }

    INTERFACE_channel_fill_frame(channel, 0);
    INTERFACE_draw_text(channel, DISPLAY_BRIGHTNESS);

    INTERFACE_set_out_number(tmp + 1, channel, DISPLAY_BRIGHTNESS);
    INTERFACE_set_rect(tmp, channel, DISPLAY_BRIGHTNESS);
    INTERFACE_set_rect(out, channel, 0);

    state = SELECT_NONE;
    INTERFACE_update();
    return 0;
}

static inline void change_output_channel(int channel, int *out, int rotation){
    INTERFACE_set_rect(*out, channel, 0);
    *out = (*out + rotation) % 16;

    if (*out < 0){
        *out += 16;
    }

    if (channel == 0){
        if (*out == channel_out1){
            *out = (*out + rotation) % 16;
        }
    } else {
        if (*out == channel_out0){
            *out = (*out + rotation) % 16;
        }
    }

    if (*out < 0){
        *out += 16;
    }
    INTERFACE_set_rect(*out, channel, DISPLAY_BRIGHTNESS);
    INTERFACE_set_smallRect(*out, channel, 0);

    INTERFACE_clear_out_number(channel, DISPLAY_BRIGHTNESS);
    INTERFACE_set_out_number(*out + 1, channel, 0);
}


void update_display(__unused void *param){

    while(1){
        if (DRAW_isRun()) vTaskDelay(pdTICKS_TO_MS(20));

        int rotation = ENCODER_getValue();
        bool accept = ENCODER_getAccept();
        if (rotation == 0 && accept == false) {
            vTaskDelay(pdTICKS_TO_MS(50));
            continue;
        }
        TIMEOUT_reset();

        switch (state){
        case SELECT_NONE:{
            state = SELECT_INPUT;
            change_input_channel(&channel, 0);
            TIMEOUT_setTimeout_ms(10000, auto_disable);
        } break;

        case SELECT_INPUT:{
            if (!accept){
                change_input_channel(&channel, rotation);
            } else {
                INTERFACE_channel_fill_frame(channel, DISPLAY_BRIGHTNESS);
                INTERFACE_draw_text(channel, 0);
                if (channel == 0){
                    out = channel_out0;
                } else {
                    out = channel_out1;
                }

                INTERFACE_set_out_number(out+1, channel, 0);
                INTERFACE_set_smallRect(out, channel, 0);
                state = SELECT_OUTPUT;
            }
        }break;

        case SELECT_OUTPUT:{
            if(!accept){
                change_output_channel(channel, &out, rotation);
            } else {
                INTERFACE_set_rect(out, channel, DISPLAY_BRIGHTNESS);
                INTERFACE_channel_fill_frame(channel, 0);
                INTERFACE_draw_text(channel, DISPLAY_BRIGHTNESS);
                INTERFACE_set_out_number(out + 1, channel, DISPLAY_BRIGHTNESS);

                if (channel == 0){
                    channel_out0 = out;
                } else {
                    channel_out1 = out;
                }
                state = SELECT_INPUT;
                TIMEOUT_cancel();
            }
        }break;
        
        default:
            break;
        }

        INTERFACE_update();
        vTaskDelay(pdTICKS_TO_MS(50));
    }
}



int main(){
    stdio_init_all();
    LED_Init();

    ENCODER_Init();

    SPI_Init();
    INTERFACE_Init();
    INTERFACE_draw();
    INTERFACE_update();

    channel_out0 = 0;
    channel_out1 = 1;
    xTaskCreate(blink_task, "Blink task", configMINIMAL_STACK_SIZE, NULL, 3, NULL);
    xTaskCreate(update_display, "Update display", 2048, NULL, 3, NULL);
    vTaskStartScheduler();

    while (1){}

    return 0;
}