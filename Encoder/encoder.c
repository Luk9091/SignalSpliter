#include "encoder.h"
static volatile int rotation;
static volatile bool button;

static void detect_first_edge(uint gpio){
    uint state = gpio_get_all();
}

void ENCODER_callback(uint gpio, uint32_t events){
    static bool A_fall = false, B_fall = false;

    switch (gpio)
    {
    case ENCODER_A:{
        if (!B_fall){
            A_fall = true;
        } else {
            printf("B");
            B_fall = false;
            rotation = 1;
        }
    } break;
    case ENCODER_B:{
        if (!A_fall) {
            B_fall = true;
        } else {
            printf("A");
            A_fall = false;
            rotation = -1;
        }
    }break;
    case ENCODER_SWITCH:{
        printf("Push\n");
        button = true;
    }break;
    
    default:
        break;
    }
}

void ENCODER_enable_interrupts(){
    gpio_set_irq_enabled_with_callback(ENCODER_SWITCH, GPIO_IRQ_EDGE_FALL, true, &ENCODER_callback);
    gpio_set_irq_enabled(ENCODER_A, GPIO_IRQ_EDGE_FALL, true);
	gpio_set_irq_enabled(ENCODER_B, GPIO_IRQ_EDGE_FALL, true);
}

void ENCODER_disable_interrupts(){
    gpio_set_irq_enabled(ENCODER_A, GPIO_IRQ_EDGE_FALL, false);
	gpio_set_irq_enabled(ENCODER_B, GPIO_IRQ_EDGE_FALL, false);
    gpio_set_irq_enabled(ENCODER_SWITCH, GPIO_IRQ_EDGE_FALL, false);
}

void ENCODER_Init(){
    uint32_t mask = 1 << ENCODER_A | 1 << ENCODER_B | 1 << ENCODER_SWITCH;
    gpio_init_mask(mask);
    gpio_set_dir_in_masked(mask);

    gpio_pull_up(ENCODER_A);
    gpio_pull_up(ENCODER_B);
    gpio_pull_up(ENCODER_SWITCH);

    rotation = 0;
    button = false;
    ENCODER_enable_interrupts();
}

int ENCODER_getValue(){
    int status = rotation;
    rotation = 0;
    return status;
}

bool ENCODER_getAccept(){
    bool status = button;
    button = false;
    return status;
}

