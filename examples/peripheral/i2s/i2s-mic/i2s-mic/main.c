#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "hosal_gpio.h"
#include "hosal_i2s.h"
#include "hosal_sysctrl.h"
#include "task.h"
#include "app_hooks.h"
#include "uart_stdio.h"

#define I2S_IO_BCK 0
#define I2S_IO_WS  1
#if defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583)
#define I2S_IO_SDO 2
#define I2S_IO_SDI 3
#else
#define I2S_IO_SDO 4
#define I2S_IO_SDI 5
#endif
#define I2S_IO_MCLK 14

#define I2S_IMCK_RATE HOSAL_I2S_IMCLK_8P192M
#define I2S_MCK_DIV   HOSAL_I2S_MCLK_DIV_2
#define I2S_BCK_RATIO HOSAL_I2S_BCK_RATIO_64
#define I2S_BCK_OSC   HOSAL_I2S_CFG_BCK_OSR_4 
#define I2S_SR        HOSAL_I2S_SR_48K
#define I2S_FMT       HOSAL_I2S_FMT_I2S
#define I2S_TRX_MODE  HOSAL_I2S_TRX_MODE_RX
#define I2S_CFG_WID   HOSAL_I2S_CFG_WID_24
#define I2S_CH        HOSAL_I2S_CH_MONO_L

#define I2S_RING_BUF_RATIO (16)
#define I2S_RDMA_SEG_SIZE  (256 * I2S_RING_BUF_RATIO)
#define I2S_WRMA_SEG_SIZE  (256 * I2S_RING_BUF_RATIO)
#define I2S_RDMA_BLK_SIZE  (256)
#define I2S_WRMA_BLK_SIZE  (256)

#if defined(CONFIG_RT581) || defined(CONFIG_RT582) || defined(CONFIG_RT583)
#define I2S_WR_BUFF 0x20002000
#else
#define I2S_WR_BUFF 0x30003000
#endif

uint32_t i2s_wdma_verify_addr;
uint32_t i2s_wdma_debug_buf_addr;
uint32_t i2s_wdma_write_ptr_last;

hosal_i2s_rdma_ctrl_ptr_t i2s_rdma_buf;
hosal_i2s_wdma_ctrl_ptr_t i2s_wdma_buf;
const uint32_t i2s_sr = I2S_SR;

volatile hosal_i2s_buff_ptr_t i2s_w, i2s_r;

void init_default_pin_mux(void) {
    hosal_pin_set_mode(I2S_IO_BCK, HOSAL_MODE_I2S_BCK);
    hosal_pin_set_mode(I2S_IO_WS, HOSAL_MODE_I2S_WCK);
    hosal_pin_set_mode(I2S_IO_SDO, HOSAL_MODE_I2S_SDO);
    hosal_pin_set_mode(I2S_IO_SDI, HOSAL_MODE_I2S_SDI);
    hosal_pin_set_mode(I2S_IO_MCLK, HOSAL_MODE_I2S_MCLK);

    return;
}

void init_i2s_parameter(hosal_i2s_para_set_t* i2s_para) {
    uint32_t loop;

    //Config I2S wdma
    i2s_wdma_buf.i2s_xdma_start_addr = I2S_WR_BUFF;
    i2s_wdma_buf.i2s_fw_access_addr = I2S_WR_BUFF;
    i2s_wdma_buf.i2s_xdma_seg_size = I2S_WRMA_SEG_SIZE;
    i2s_wdma_buf.i2s_xdma_blk_size = I2S_WRMA_BLK_SIZE;
    i2s_wdma_buf.i2s_xdma_seg_blk_ratio = I2S_RING_BUF_RATIO;

    i2s_para->rdma_config = NULL; //(i2s_rdma_ctrl_ptr_t *)&i2s_rdma_buf;
    i2s_para->wdma_config = (hosal_i2s_wdma_ctrl_ptr_t*)&i2s_wdma_buf;
    //i2s_para->wdma_config = (i2s_wdma_ctrl_ptr_t*)&i2s_wdma_buf;

    i2s_para->imck_rate = I2S_IMCK_RATE;
    i2s_para->mck_div = I2S_MCK_DIV;
    i2s_para->bck_osr = I2S_BCK_OSC;
    i2s_para->trx_mode = I2S_TRX_MODE;
    i2s_para->fmt = I2S_FMT;
    i2s_para->bck_ratio = I2S_BCK_RATIO;
    i2s_para->width = I2S_CFG_WID;
    i2s_para->ch = I2S_CH;
    i2s_para->sr = I2S_SR;
    //Clear Buffer

    for (loop = 0; loop < i2s_wdma_buf.i2s_xdma_seg_size; loop++) {
        *((uint32_t*)i2s_wdma_buf.i2s_xdma_start_addr + loop) = 0x11;
    }

    for (loop = 0; loop < i2s_wdma_buf.i2s_xdma_seg_size; loop++) {
        *((uint32_t*)i2s_wdma_buf.i2s_xdma_start_addr + loop) = 0x00;
    }

    i2s_w.address = i2s_wdma_buf.i2s_xdma_start_addr;
    i2s_w.size = I2S_WRMA_BLK_SIZE;
}

void i2s_cb(hosal_i2s_cb_t* p_cb) {

    if (p_cb->type == HOSAL_I2S_CB_WDMA) {

        i2s_w.flag = 1;

        if (i2s_w.index == p_cb->seg_size) {
            i2s_w.index = 0;
        }

        i2s_w.index += p_cb->blk_size;

        if (i2s_w.index == i2s_w.size) {
            i2s_w.offset = 0;
        } else {
            i2s_w.offset = i2s_w.index - i2s_w.size;
        }
    }

    if (p_cb->type == HOSAL_I2S_CB_RDMA) {
        i2s_r.flag = 1;

        if (i2s_r.index == p_cb->seg_size) {
            i2s_r.index = 0;
        }

        i2s_r.index += p_cb->blk_size;

        if (i2s_r.index == i2s_r.size) {
            i2s_r.offset = 0;
        } else {
            i2s_r.offset = i2s_r.index - i2s_r.size;
        }
    }
}

int main(void) {
    hosal_i2s_para_set_t i2s_config_para;

    uint32_t buf_index;
    uint32_t wdma_value;

    uart_stdio_init();
    vHeapRegionsInt();

    printf("Starting %s now %d.... \r\n", CONFIG_CHIP,
           CONFIG_HOSAL_SOC_MAIN_ENTRY_TASK_SIZE);

    init_default_pin_mux();

    /* fill the parameters will be used. */
    init_i2s_parameter(&i2s_config_para);

    /* Initial I2S */
    hosal_i2s_init(&i2s_config_para);

    /* Reigster Interrupt callback function */
    hosal_i2s_callback_register(i2s_cb);
    NVIC_EnableIRQ(I2s0_IRQn);
    
    printf("Digital Microphone SPH6405 Pin connect\r\n");
    printf("SEL  Pin <----> EVK_GND(Left channel)/EVK_VCC(Right channel)\r\n");
    printf("LRCL Pin <----> EVK_WCK\r\n");
    printf("BCK  Pin <----> EVK_BCK\r\n");
    printf("DOUT Pin <----> EVK_SDI\r\n");
    printf("I2S Start\r\n");

    hosal_i2s_start(&i2s_config_para);

    while (1) {
        if (i2s_w.flag == 1) {
            for (buf_index = 0; buf_index < i2s_w.size; buf_index++) {
                wdma_value = *((uint32_t*)(i2s_wdma_buf.i2s_fw_access_addr)
                               + i2s_w.offset + buf_index); //read wdam data
                printf("%ld,%lx\r\n", buf_index, wdma_value);
            }

            i2s_w.flag = 0;
        }
    }
}
