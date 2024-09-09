#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "FreeRTOS.h"
#include "task.h"
#include "cli.h"
#include "log.h"
#include "hosal_swi.h"


void swi_0_callback(uint32_t id) {
    printf("Trigger SWI ID O \r\n");
    hosal_swi_trigger(HOSAL_SWI_1);

}

void swi_1_callback(uint32_t id) {
    printf("Trigger SWI ID 1 \r\n");

}


int main(void) {

    printf("hosal software interrupt demo examples %s %s \r\n", __DATE__, __TIME__);

    hosal_swi_init();
    hosal_swi_callback_register(HOSAL_SWI_0, swi_0_callback);
    hosal_swi_callback_register(HOSAL_SWI_1, swi_1_callback);
    hosal_swi_trigger(HOSAL_SWI_0);

    while (1) {;}

}

