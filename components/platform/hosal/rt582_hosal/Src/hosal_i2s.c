/**
 * \file            hosal_i2x.c
 * \brief           Hosal I2C driver file
 *          
 */
/*
 * Copyright (c) year FirstName LASTNAME
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 * Author:          Kc.tseng
 */

#include "mcu.h"

#include "hosal_i2s.h"

static hosal_i2s_cb_fn hosal_i2s_reg_handler = NULL;

void hosal_i2s_cb_handler(i2s_cb_t* p_cb) {
    hosal_i2s_cb_t hosal_cb = {
        .type = (hosal_i2s_cb_type_t)p_cb->type,
        .blk_size = p_cb->blk_size,
        .seg_size = p_cb->seg_size,
    };
    if (hosal_i2s_reg_handler != NULL)
        hosal_i2s_reg_handler(&hosal_cb);
}

void hosal_i2s_callback_register(void* i2s_usr_callback) {
    if (i2s_usr_callback != NULL) {
        hosal_i2s_reg_handler = i2s_usr_callback;
        i2s_callback_register(hosal_i2s_cb_handler);
    }
}

uint32_t hosal_i2s_get_wdma_access_pos(void) {
    return i2s_get_wdma_access_pos();
};

uint32_t hosal_i2s_init(hosal_i2s_para_set_t* hosal_i2s_para) {
    i2s_para_set_t i2s_para = {
        .rdma_config = (i2s_rdma_ctrl_ptr_t*)hosal_i2s_para->rdma_config,
        .wdma_config = (i2s_wdma_ctrl_ptr_t*)hosal_i2s_para->wdma_config,
        .sr = (i2s_sample_rate_t)hosal_i2s_para->sr,
        .ch = (i2s_ch_format_t)hosal_i2s_para->ch,
        .trx_mode = (i2s_trx_mode_t)hosal_i2s_para->trx_mode,
        .fmt = (i2s_fmt_t)hosal_i2s_para->fmt,
        .width = (i2s_cfg_txd_wid_t)hosal_i2s_para->width,
        .bck_ratio = (i2s_cfg_bck_ratio_t)hosal_i2s_para->bck_ratio,
        .mck_div = (i2s_mclk_div_t)hosal_i2s_para->mck_div,
        .bck_osr = (i2s_cfg_bck_osr_t)hosal_i2s_para->bck_osr,
        .imck_rate = (i2s_cfg_imck_rate_t)hosal_i2s_para->imck_rate,
    };
    return i2s_init(&i2s_para);
}

uint32_t hosal_i2s_uninit(void) { return i2s_uninit(); }

uint32_t hosal_i2s_start(hosal_i2s_para_set_t* hosal_i2s_para) {
    i2s_para_set_t i2s_para = {
        .rdma_config = (i2s_rdma_ctrl_ptr_t*)hosal_i2s_para->rdma_config,
        .wdma_config = (i2s_wdma_ctrl_ptr_t*)hosal_i2s_para->wdma_config,
        .sr = (i2s_sample_rate_t)hosal_i2s_para->sr,
        .ch = (i2s_ch_format_t)hosal_i2s_para->ch,
        .trx_mode = (i2s_trx_mode_t)hosal_i2s_para->trx_mode,
        .fmt = (i2s_fmt_t)hosal_i2s_para->fmt,
        .width = (i2s_cfg_txd_wid_t)hosal_i2s_para->width,
        .bck_ratio = (i2s_cfg_bck_ratio_t)hosal_i2s_para->bck_ratio,
        .mck_div = (i2s_mclk_div_t)hosal_i2s_para->mck_div,
        .bck_osr = (i2s_cfg_bck_osr_t)hosal_i2s_para->bck_osr,
        .imck_rate = (i2s_cfg_imck_rate_t)hosal_i2s_para->imck_rate,
    };
    return i2s_start(&i2s_para);
}

uint32_t hosal_i2s_stop(void) { return i2s_stop(); }
