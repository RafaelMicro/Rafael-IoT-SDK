#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_i2s.h"

// ---------- D E F I N I T I O N S -------------------- //
#define I2S_IO_BCK  0 //  0 or 10, default: 0
#define I2S_IO_WS   1 //  1 or 11, default: 1
#define I2S_IO_SDO  2 //  2 or 12, default: 2
#define I2S_IO_SDI  3 //  3 or 13, default: 3
#define I2S_IO_MCLK 14

#define I2S_RD_BUFF       0x20002000
#define I2S_WR_BUFF       0x20004000
#define I2S_WR_DEBUG_BUFF 0x20007000

#define I2S_WR_BUFF_OFFSET (I2S_WR_DEBUG_BUFF - I2S_WR_BUFF)

#define I2S_RING_BUF_RATIO 3
#define I2S_RDMA_BLK_ZIZE  (96)
#define I2S_WRMA_BLK_ZIZE  (96)
#define I2S_RDMA_SEG_ZIZE  (96 * I2S_RING_BUF_RATIO)
#define I2S_WRMA_SEG_ZIZE  (96 * I2S_RING_BUF_RATIO)

#define I2S_FMT      HOSAL_I2S_FMT_I2S
#define I2S_CH       HOSAL_I2S_CH_STEREO
#define I2S_TRX_MODE HOSAL_I2S_TRX_MODE_TXRX

#define I2S_SR        HOSAL_I2S_SR_48K
#define I2S_IMCK_RATE HOSAL_I2S_IMCLK_12P288M
#define I2S_MCK_DIV   HOSAL_I2S_MCLK_DIV_2
#define I2S_BCK_OSC   HOSAL_I2S_CFG_BCK_OSR_2
#define I2S_BCK_RATIO HOSAL_I2S_BCK_RATIO_64
#define I2S_CFG_WID   HOSAL_I2S_CFG_WID_32

#define rxbufsize 512
#define txbufsize 128

uint32_t i2s_wdma_verify_addr;
uint32_t i2s_wdma_debug_buf_addr;
uint32_t i2s_wdma_write_ptr_last;

hosal_i2s_rdma_ctrl_ptr_t i2s_rdma_buf;
hosal_i2s_wdma_ctrl_ptr_t i2s_wdma_buf;
const hosal_i2s_sample_rate_t i2s_sr = I2S_SR;


void init_default_pin_mux(void) {
    /* set GPIO00 as I2S_BCK, GPIO01 as I2S_MCK, GPIO02 as I2S0_SD, GPIO03 as I2S_SDI ,and GPIO08 as I2S_MCLK */
    pin_set_mode(I2S_IO_BCK, MODE_I2S);
    pin_set_mode(I2S_IO_WS, MODE_I2S);
    pin_set_mode(I2S_IO_SDO, MODE_I2S);
    pin_set_mode(I2S_IO_SDI, MODE_I2S);
    pin_set_mode(I2S_IO_MCLK, MODE_I2S);

    return;
}

void init_i2s_fill_data_to_memory(hosal_i2s_rdma_ctrl_ptr_t* i2s_rdma_para) {
    uint32_t idx;
    for (idx = 0; idx < 3 * 2; idx++) {
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 0) = 0x7FFF0000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 1) = 0x7BA310B5;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 2) = 0x6EDA2121;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 3) = 0x5A8230FC;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 4) = 0x40004000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 5) = 0x21214DEC;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 6) = 0x00005A82;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 7) = 0xDEDF658D;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 8) = 0xC0006EDA;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 9) = 0xA57E7642;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 10) = 0x91267BA3;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 11) = 0x845D7EE8;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 12) = 0x80007FFF;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 13) = 0x845D7EE8;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 14) = 0x91267BA3;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 15) = 0xA57E7642;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 16) = 0xC0006EDA;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 17) = 0xDEDF658D;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 18) = 0x00005A82;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 19) = 0x21214DEC;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 20) = 0x40004000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 21) = 0x5A8230FC;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 22) = 0x6EDA2121;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 23) = 0x7BA310B5;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 24) = 0x7FFF0000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 25) = 0x7BA3EF4B;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 26) = 0x6EDADEDF;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 27) = 0x5A82CF04;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 28) = 0x4000C000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 29) = 0x2121B214;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 30) = 0x0000A57E;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 31) = 0xDEDF9A73;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 32) = 0xC0009126;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 33) = 0xA57E89BE;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 34) = 0x9126845D;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 35) = 0x845D8118;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 36) = 0x80008000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 37) = 0x845D8118;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 38) = 0x9126845D;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 39) = 0xA57E89BE;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 40) = 0xC0009126;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 41) = 0xDEDF9A73;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 42) = 0x0000A57E;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 43) = 0x2121B214;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 44) = 0x4000C000;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 45) = 0x5A82CF04;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 46) = 0x6EDADEDF;
        *((uint32_t*)(I2S_WR_DEBUG_BUFF) + idx * 48 + 47) = 0x7BA3EF4B;
    }
}

void i2s_inc_fw_addr(hosal_i2s_xdma_ctrl_ptr_t* i2s_xdma_para) {
    i2s_xdma_para->i2s_fw_access_addr += (i2s_xdma_para->i2s_xdma_blk_size * 4);
    if ((i2s_xdma_para->i2s_fw_access_addr - i2s_xdma_para->i2s_xdma_start_addr)
        >= i2s_xdma_para->i2s_xdma_seg_size * 4) {
        i2s_xdma_para->i2s_fw_access_addr = i2s_xdma_para->i2s_xdma_start_addr;
    }
}

void i2s_fill_data_to_memory(hosal_i2s_para_set_t* i2s_para)
{
    hosal_i2s_rdma_ctrl_ptr_t* i2s_rdma_para = i2s_para->rdma_config;
    hosal_i2s_wdma_ctrl_ptr_t* i2s_wdma_para = i2s_para->wdma_config;
    uint32_t i2s_rdma_read_ptr = (I2S0->rdma_r0);
    uint8_t loop;

    /* The same phase */
    if ((i2s_rdma_read_ptr > i2s_rdma_para->i2s_fw_access_addr)
        && (i2s_rdma_para->i2s_fw_access_addr
                + 4 * i2s_rdma_para->i2s_xdma_blk_size
            > i2s_rdma_read_ptr)) {
        /* Fill new data here */
        for (loop = 0; loop <= i2s_rdma_para->i2s_xdma_blk_size; loop++) {
            *((uint32_t*)i2s_rdma_para->i2s_fw_access_addr
              + loop) = *((uint32_t*)(i2s_wdma_para->i2s_fw_access_addr
                                      + I2S_WR_BUFF_OFFSET)
                          + loop);
        }
        i2s_inc_fw_addr(i2s_rdma_para);
        i2s_inc_fw_addr(i2s_wdma_para);
    }
    /* The different phase */
    else if ((i2s_rdma_read_ptr < i2s_rdma_para->i2s_fw_access_addr)) {
        /* Fill new data here */
        for (loop = 0; loop <= i2s_rdma_para->i2s_xdma_blk_size; loop++) {
            *((uint32_t*)i2s_rdma_para->i2s_fw_access_addr
              + loop) = *((uint32_t*)(i2s_wdma_para->i2s_fw_access_addr
                                      + I2S_WR_BUFF_OFFSET)
                          + loop);
        }
        i2s_inc_fw_addr(i2s_rdma_para);
        i2s_inc_fw_addr(i2s_wdma_para);
    }
}

void init_i2s_data_memory_verification(void) {

    i2s_wdma_verify_addr = I2S_WR_BUFF + (2 * 4);
    i2s_wdma_debug_buf_addr = I2S_WR_DEBUG_BUFF;
    i2s_wdma_write_ptr_last = I2S_WR_BUFF;
}

void i2s_data_memory_verification() {
    uint32_t i2s_wdma_write_ptr = i2s_get_wdma_access_pos();
    uint32_t error_cnt = 0;
    volatile uint8_t i = 0;

    if (i2s_wdma_write_ptr_last == i2s_wdma_write_ptr) {
        /* data buffer empty */
        return; 
    } else {
        i2s_wdma_write_ptr_last = i2s_wdma_write_ptr;
    }

    while (i2s_wdma_verify_addr != i2s_wdma_write_ptr) {

        if (*((uint32_t*)i2s_wdma_verify_addr)
            != *((uint32_t*)i2s_wdma_debug_buf_addr)) {
            printf(
                "I2S loopback test fail: Address 0x%x value = 0x%x (0x%x)\r\n",
                i2s_wdma_verify_addr, (*((uint32_t*)i2s_wdma_verify_addr)),
                (*((uint32_t*)i2s_wdma_debug_buf_addr)));

            error_cnt++;
            if (error_cnt == 10) {
                puts("Too much error!!\r\n");
            }
        }

        *((uint32_t*)i2s_wdma_verify_addr) = 0;
        if (*((uint32_t*)i2s_wdma_verify_addr) != 0) {
            printf(
                "I2S loopback test fail: Address 0x%x value = 0x%x (0x%x)\r\n",
                i2s_wdma_verify_addr, (*((uint32_t*)i2s_wdma_verify_addr)), 0);
        }

        i2s_wdma_verify_addr += (1 * 4);
        if (i2s_wdma_verify_addr >= (I2S_WR_BUFF + (I2S_WRMA_SEG_ZIZE * 4))) {
            i2s_wdma_verify_addr = I2S_WR_BUFF;
        }

        i2s_wdma_debug_buf_addr += (1 * 4);
        if (i2s_wdma_debug_buf_addr
            >= (I2S_WR_DEBUG_BUFF + (I2S_WRMA_SEG_ZIZE * 4))) {
            i2s_wdma_debug_buf_addr = I2S_WR_DEBUG_BUFF;
        }
    }
}

/* this is used to fill configuations for I2S parameter set */
void init_i2s_parameter(hosal_i2s_para_set_t* i2s_para) {
    uint16_t loop;
    i2s_rdma_buf.i2s_xdma_start_addr = I2S_RD_BUFF;
    i2s_rdma_buf.i2s_fw_access_addr = I2S_RD_BUFF;
    i2s_rdma_buf.i2s_xdma_seg_size = I2S_RDMA_SEG_ZIZE;
    i2s_rdma_buf.i2s_xdma_blk_size = I2S_RDMA_BLK_ZIZE;
    i2s_rdma_buf.i2s_xdma_seg_blk_ratio = I2S_RING_BUF_RATIO;

    i2s_wdma_buf.i2s_xdma_start_addr = I2S_WR_BUFF;
    i2s_wdma_buf.i2s_fw_access_addr = I2S_WR_BUFF;
    i2s_wdma_buf.i2s_xdma_seg_size = I2S_WRMA_SEG_ZIZE;
    i2s_wdma_buf.i2s_xdma_blk_size = I2S_WRMA_BLK_ZIZE;
    i2s_wdma_buf.i2s_xdma_seg_blk_ratio = I2S_RING_BUF_RATIO;

    i2s_para->rdma_config = (hosal_i2s_rdma_ctrl_ptr_t*)&i2s_rdma_buf;
    i2s_para->wdma_config = (hosal_i2s_wdma_ctrl_ptr_t*)&i2s_wdma_buf;

    i2s_para->sr = I2S_SR;
    i2s_para->fmt = I2S_FMT;
    i2s_para->ch = I2S_CH;
    i2s_para->trx_mode = I2S_TRX_MODE;
    i2s_para->width = I2S_CFG_WID;
    i2s_para->bck_ratio = I2S_BCK_RATIO;
    i2s_para->mck_div = I2S_MCK_DIV;
    i2s_para->bck_osr = I2S_BCK_OSC;
    i2s_para->imck_rate = I2S_IMCK_RATE;

    for (loop = 0; loop < i2s_wdma_buf.i2s_xdma_seg_size; loop++) {
        *((uint32_t*)i2s_wdma_buf.i2s_xdma_start_addr + loop) = 0x00;
    }

    for (loop = 0; loop < i2s_rdma_buf.i2s_xdma_seg_size; loop++) {
        *((uint32_t*)i2s_rdma_buf.i2s_xdma_start_addr + loop) = 0x00;
    }
}

void i2s_cb(hosal_i2s_cb_t* p_data_cb) {

    p_data_cb->type;
    p_data_cb->blk_size;
    p_data_cb->seg_size;
    /*printf("p_data_cb->type:%x, p_data_cb->blk_size:%x, p_data_cb->seg_size:%x\r\n",
           p_data_cb->type, p_data_cb->blk_size, p_data_cb->seg_size);*/

    return;
}

int32_t main(void) {
    hosal_i2s_para_set_t i2s_config_para;
    uint32_t cnt = 0, i = 0;

    init_default_pin_mux();

    /* fill the parameters will be used. */
    init_i2s_parameter(&i2s_config_para);

    /* Initial I2S */
    hosal_i2s_init(&i2s_config_para);

    /* Fill the initial data into memory */
    init_i2s_fill_data_to_memory(&i2s_rdma_buf);

    i2s_fill_data_to_memory(&i2s_config_para);
    init_i2s_data_memory_verification();

    hosal_i2s_callback_register(i2s_cb);

    puts("/*****Start I2S Loopback******/\r\n");

    puts("SDI need to connect SDO\r\n");
    
    /* Start I2S */
    hosal_i2s_start(&i2s_config_para);
    i2s_data_memory_verification();

    while (1) {
        i2s_fill_data_to_memory(&i2s_config_para);
        i2s_data_memory_verification();
        if ( (i % 1000000) == 0) {
            puts(".");
        }
            
    }
}

