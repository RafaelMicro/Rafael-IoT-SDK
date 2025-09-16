#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "hosal_lpm.h"
#include "hosal_dma.h"
#include "hosal_sysctrl.h"
#include "uart_stdio.h"



#define LPM_SRAM0_RETAIN 0x1E

int main(void) {

    uart_stdio_init();
    hosal_dma_init();
    printf("\r\n----------------------------------------------------------------\r\n");
    printf("Build Date:%s \r\n",__DATE__);
    printf("Build Time:%s \r\n",__TIME__);
    printf("----------------------------------------------------------------\r\n");
    printf("Examples    : hosal low power mode demo (Sleep)\r\n");
    printf("HOST        : Sleep\r\n");
    printf("RF          : Sleep (Control by host mcu)\r\n");
    printf("----------------------------------------------------------------\r\n");
    

    hosal_lpm_ioctrl(HOSAL_LPM_SET_POWER_LEVEL, HOSAL_LPM_SLEEP);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_ENTER_LOW_POWER, HOSAL_COMMUMICATION_SUBSYSTEM_PWR_STATE_SLEEP);
    #if defined(RT581) || defined(RT582) || defined(RT583)
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_SRAM_DEEP_SLEEP_INIT, HOSAL_LPM_PARAM_NONE);
    hosal_lpm_ioctrl(HOSAL_LPM_SUBSYSTEM_DISABLE_LDO_MODE, HOSAL_LPM_PARAM_NONE);
    hosal_lpm_ioctrl(HOSAL_LPM_SRAM0_RETAIN, LPM_SRAM0_RETAIN);
    #endif
    printf("Wiat 1 sec\r\n");
    hosal_delay_ms(1000);
    hosal_lpm_ioctrl(HOSAL_LPM_ENTER_LOW_POWER, HOSAL_LPM_PARAM_NONE);

    while (1) {;}
}

/** @} */ /* end of examples group */
