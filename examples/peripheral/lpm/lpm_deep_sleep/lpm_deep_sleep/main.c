#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_lpm.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"

#define LPM_SRAM0_RETAIN 0x1E

int main(void) {



    uart_stdio_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal low power mode demo (Deep Sleep)\r\n");
    printf("HOST        : Deep Sleep\r\n");
    printf("RF          : Deep Sleep (Control by host mcu)\r\n");
    printf("----------------------------------------------------------------\r\n");

    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_DEEP_SLEEP);
    /*
       if no load rf mcu code,need to add below code
    */
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER,HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_DEEP_SLEEP);
    printf("Wiat 1 sec\r\n");
    hosal_delay_ms(1000);
    hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);

    while (1) {;}
}

/** @} */ /* end of examples group */
