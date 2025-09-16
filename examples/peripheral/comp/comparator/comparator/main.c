#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hosal_status.h"
#include "hosal_comparator.h"
#include "app_hooks.h"
#include "uart_stdio.h"


#define COMP_VBAT_DEMO           0
#define COMP_PCH_IOLDO_DEMO      1
#define COMP_DCDCLLDO_NCH_DEMO   2
#define COMP_PCH_NCH_DEMO        3

#define COMP_DEMO_CASE           COMP_VBAT_DEMO

hosal_comp_config_t p_comp_config;

/*Comparator Callback */
void comparator_Callback(uint32_t status) {

    printf(" Comparator interrupt is triggered, \r\n");

    if (HOSAL_COMP_OUT_GET()) {
        printf("output high. \r\n");
    } else {
        printf("output low. \r\n");
    }

    return;
}

uint32_t comparator_inti(hosal_comp_config_t* p_config)
{

    if (p_config == NULL)
    {
        return HOSAL_STATUS_INVALID_PARAM;
    }

#if (COMP_DEMO_CASE == COMP_VBAT_DEMO)
    p_config->comp_selref = HOSAL_COMP_CONFIG_SELREF_INTERNAL;          /*Select the Comparator input N source: BG 0.6V*/
    p_config->comp_ref_sel = HOSAL_COMP_CONFIG_REFSEL_CHANNEL_0;        /*Select the Comparator N input for comparision: Channel 0*/
    p_config->comp_selinput = HOSAL_COMP_CONFIG_SELINPUT_INTERNAL;      /*Select the Comparator input P source: Internal vdd div*/
    p_config->comp_ch_sel = HOSAL_COMP_CONFIG_CHSEL_CHANNEL_0;          /*Select the Comparator P input for comparision: Channel 0*/
#elif (COMP_DEMO_CASE == COMP_PCH_IOLDO_DEMO)
    p_config->comp_selref = HOSAL_COMP_CONFIG_SELREF_EXTERNAL;          /*Select the Comparator input N source: external*/
    p_config->comp_ref_sel = HOSAL_COMP_CONFIG_REFSEL_IOLDO;            /*Select the Comparator N input for comparision: IOLDO*/
    p_config->comp_selinput = HOSAL_COMP_CONFIG_SELINPUT_EXTERNAL;      /*Select the Comparator input P source: external*/
    p_config->comp_ch_sel = HOSAL_COMP_CONFIG_CHSEL_CHANNEL_7;          /*Select the Comparator P input for comparision: Channel 7*/
#elif (COMP_DEMO_CASE == COMP_DCDCLLDO_NCH_DEMO)
    p_config->comp_selref = HOSAL_COMP_CONFIG_SELREF_EXTERNAL;          /*Select the Comparator input N source: external*/
    p_config->comp_ref_sel = HOSAL_COMP_CONFIG_REFSEL_CHANNEL_7;        /*Select the Comparator N input for comparision: Channel 7*/
    p_config->comp_selinput = HOSAL_COMP_CONFIG_SELINPUT_EXTERNAL;      /*Select the Comparator input P source: external*/
    p_config->comp_ch_sel = HOSAL_COMP_CONFIG_CHSEL_AVDD_1V;            /*Select the Comparator P input for comparision: DCDC/LLDO AVDD 1.2V*/
#elif (COMP_DEMO_CASE == COMP_PCH_NCH_DEMO)
    p_config->comp_selref = HOSAL_COMP_CONFIG_SELREF_EXTERNAL;          /*Select the Comparator input N source: external*/
    p_config->comp_ref_sel = HOSAL_COMP_CONFIG_REFSEL_CHANNEL_5;        /*Select the Comparator N input for comparision: Channel 5*/
    p_config->comp_selinput = HOSAL_COMP_CONFIG_SELINPUT_EXTERNAL;      /*Select the Comparator input P source: external*/
    p_config->comp_ch_sel = HOSAL_COMP_CONFIG_CHSEL_CHANNEL_7;          /*Select the Comparator P input for comparision: Channel 7*/
#endif

    p_config->comp_swdiv = HOSAL_COMP_CONFIG_SWDIV_RES;                 /*Switch the Comparator vdd div type: RES div*/
    p_config->comp_v_sel = (hosal_comp_config_v_sel_t)HOSAL_COMP_V_SEL_GET(); /*Select the Comparator internal vdd div voltage for Power Fail: By MP setting*/
    p_config->comp_int_pol = HOSAL_COMP_CONFIG_INT_POL_BOTH;            /*Set the Comparator interrupt polarity: Both edge*/
    p_config->comp_ds_wakeup = HOSAL_COMP_CONFIG_DS_WAKEUP_ENABLE;      /*Select the Comparator wakeup in DeepSleep: Enable*/
    p_config->comp_ds_inv = HOSAL_COMP_CONFIG_DS_INVERT_DISABLE;        /*Select the invert of the Comparator output for waking up from DeepSleep: Disable*/

    p_config->comp_pw = HOSAL_COMP_CONFIG_PW_LARGEST;                   /*Select the Comparator current: Largest*/
    p_config->comp_hys_sel = HOSAL_COMP_CONFIG_HYS_130;                 /*Select the Comparator hys window: 130mV*/

    return HOSAL_STATUS_SUCCESS;
}


int32_t main(void) {
    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);


    printf("/**********Comparator Examples******************/\r\n");
 
    printf("Comparator enable!!!\r\n");

    comparator_inti(&p_comp_config);

    hosal_comparator_open(&p_comp_config, comparator_Callback);    /*Init Comparator*/
    
    hosal_comparator_enable();

    printf(" \r\n Comparator Vsel: %d\n", HOSAL_COMP_V_SEL_GET());

    while (1) {
    }
}

