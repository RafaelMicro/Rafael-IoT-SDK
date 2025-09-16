#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_swi.h"
#include "hosal_dma.h"
#include "uart_stdio.h"


void swi_0_callback(uint32_t id) {

    printf("Trigger SWI0 ID 0 %d\r\n",id);

}

void swi_1_callback(uint32_t id) {

    printf("Trigger SWI0 ID 1%d\r\n",id);
}


int main(void) {

    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("Build Chip:%s \r\n",CONFIG_CHIP);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : software interrupt demo\r\n");
    printf("----------------------------------------------------------------\r\n");


	hosal_swi_init();
    
    hosal_swi_callback_register(HOSAL_SWI0_ID, HOSAL_TRIG_0,swi_0_callback); 
    hosal_swi_trigger(HOSAL_SWI0_ID,HOSAL_TRIG_0);

    hosal_swi_callback_register(HOSAL_SWI0_ID, HOSAL_TRIG_1,swi_1_callback); 
    hosal_swi_trigger(HOSAL_SWI0_ID,HOSAL_TRIG_0);

    printf("\r\n\r\n");
    printf("hosal software interrupt demo finsh\r\n");
    while (1) {;}

}

