#ifndef __SCREENSAVER_H__
#define __SCREENSAVER_H__

#include <pico/stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include "FreeRTOSConfig.h"

#include "interface.h"

void SCREENSAVER_enable();
void SCREENSAVER_disable(int channel_out0, int channel_out1);
bool SCREENSAVER_isRun();


#endif