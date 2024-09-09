/**************************************************************************//**
 * @file     irm.c
 * @version
 * @brief
 *
 * @copyright
*****************************************************************************/


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "irm.h"

#define TEST_PROTOCOL (_NEC)
#define _NEC (0)
#define _SIRC_12BITS (1)
#define _SIRC_15BITS (2)
#define _RC_6 (3)

IR_STATE_t ir_state;
IR_BUFFER_t buffer;
IR_FIFO_t fifo[IR_BUFFER_MAX_LENGTH];

static irm_proc_cb user_cb = NULL;

void buffer_init(void)
{
    buffer.buf = fifo;
    buffer.bufsize_mask = IR_BUFFER_MAX_LENGTH;
    buffer.wr_idx = 0;
    buffer.rd_idx = 0;
    memset((void *)fifo, 0, sizeof(IR_FIFO_t)*IR_BUFFER_MAX_LENGTH);
}

void ir_carrier_config(uint8_t carrier_high_cnt, uint8_t carrier_low_cnt, uint16_t carrier_base_cnt)
{
    //printf("To configuare IRM Carrier\n");
    IRM->carrier.bit.car_base_cnt = (carrier_base_cnt - 1) & 0xffff;
    IRM->carrier.bit.car_high_cnt = carrier_high_cnt & 0xf;
    IRM->carrier.bit.car_low_cnt = carrier_low_cnt & 0xf;
}

uint32_t irm_open(irm_mode_t *irm_cfg)
{
    IRM->ir_conf.bit.op_mode = irm_cfg->op_mode;
    IRM->ir_conf.bit.out_mode = irm_cfg->ir_out_mode;
    IRM->int_ena.reg = irm_cfg->irm_int_en;
    user_cb = irm_cfg->irm_cb_func;

    NVIC_EnableIRQ(Irm_IRQn);

    ir_state = IDLE;
    buffer_init();
    return STATUS_SUCCESS;
}

void ir_nec_encoder(uint16_t cmd, uint16_t address)
{
    uint8_t i;

    i = buffer.wr_idx % IR_BUFFER_MAX_LENGTH;
    if ( i )
    {
        buffer.wr_idx = buffer.wr_idx + (IR_BUFFER_MAX_LENGTH - i);
    }
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.rd_idx = buffer.wr_idx;
    //printf("IRM cmd:0x%x,address:0x%x\n",cmd,address);

    buffer.total_usage_cnt = 0;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_START_HIGHT;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.total_usage_cnt += NEC_START_HIGHT;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_START_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.total_usage_cnt += NEC_START_LOW;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;

    for (i = 0; i < (16); i++)
    {
        // To encode addr(2bytes)
        if ((address & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ONE_HIGHT;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ONE_LOW;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ZERO_HIGHT;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ZERO_LOW;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        address >>= 1;
    }

    for (i = 0; i < (16); i++)
    {
        // To encode cmd(2bytes)
        if ((cmd & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ONE_HIGHT;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ONE_LOW;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ZERO_HIGHT;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.total_usage_cnt += NEC_ZERO_LOW;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        cmd >>= 1;
    }

    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_END_BUST;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 1;
    buffer.total_usage_cnt += NEC_END_BUST;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
}

void ir_nec_repeat_encoder(void)
{
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_110MS - buffer.total_usage_cnt;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.total_usage_cnt = 0;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_START_HIGHT;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.total_usage_cnt += NEC_START_HIGHT;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_REPEAT_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.total_usage_cnt += NEC_REPEAT_LOW;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = NEC_END_BUST;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 1;
    buffer.total_usage_cnt += NEC_END_BUST;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;

}

void ir_sirc_encoder(uint8_t cmd, uint8_t address, uint8_t address_length)
{
    uint8_t i;

    i = buffer.wr_idx % IR_BUFFER_MAX_LENGTH;
    if ( i )
    {
        buffer.wr_idx = buffer.wr_idx + (IR_BUFFER_MAX_LENGTH - i);
    }
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.rd_idx = buffer.wr_idx;

    buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_START_HIGHT;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_START_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    for (i = 0; i < (7); i++)
    {
        // To encode cmd(2bytes)
        if ((cmd & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;

        }
        cmd >>= 1;
    }
    for (i = 0; i < (address_length - 1); i++)
    {
        // To encode addr(2bytes)
        if ((address & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        address >>= 1;
    }
    if ((address & 1) == 1)
    {
        buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_HIGHT;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ONE_LOW;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
        buffer.buf[buffer.wr_idx].bf.evn_int = 0;
        buffer.buf[buffer.wr_idx].bf.env_last = 1;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    }
    else
    {
        buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_ZERO_HIGHT;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        buffer.buf[buffer.wr_idx].bf.evn_cnt = SIRC_END_LOW;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
        buffer.buf[buffer.wr_idx].bf.evn_int = 0;
        buffer.buf[buffer.wr_idx].bf.env_last = 1;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    }

}

void ir_rc6_encoder(uint16_t cmd, uint8_t address, uint8_t RC6_toggle)
{
    uint8_t i;
    // RC6 START
    i = buffer.wr_idx % IR_BUFFER_MAX_LENGTH;
    if ( i )
    {
        buffer.wr_idx = buffer.wr_idx + (IR_BUFFER_MAX_LENGTH - i);
    }
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.rd_idx = buffer.wr_idx;

    buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_START_HIGHT;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_START_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_HIGHT;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
    buffer.buf[buffer.wr_idx].bf.env_last = 0;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;


    for (i = 0; i < (3); i++)
    {
        // MOD = b000
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_LOW;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_HIGHT;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    }
    //TR
    if ((RC6_toggle & 1) == 1)
    {
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_TR_LOW;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_TR_HIGHT;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    }
    else
    {
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_TR_HIGHT;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_TR_LOW;
        buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
        buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
        buffer.buf[buffer.wr_idx].bf.env_last = 0;
        buffer.wr_idx++;
        buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
    }


    for (i = 0; i < (8); i++)
    {
        // To encode cmd(1bytes)
        if ((cmd & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;

        }
        cmd >>= 1;
    }
    for (i = 0; i < (8); i++)
    {
        // To encode addr(1bytes)
        if ((address & 1) == 1)
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;

            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ONE_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        else
        {
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_LOW;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
            buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_ZERO_HIGHT;
            buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_HIGH;
            buffer.buf[buffer.wr_idx].bf.evn_int = ((buffer.wr_idx % IR_BUFFER_INT_LENGTH) == 0);
            buffer.buf[buffer.wr_idx].bf.env_last = 0;
            buffer.wr_idx++;
            buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;
        }
        address >>= 1;
    }
    //Signal free
    buffer.buf[buffer.wr_idx].bf.evn_cnt = RC6_END_LOW;
    buffer.buf[buffer.wr_idx].bf.evenlope = IR_EVELOPE_LOW;
    buffer.buf[buffer.wr_idx].bf.evn_int = 0;
    buffer.buf[buffer.wr_idx].bf.env_last = 1;
    buffer.wr_idx++;
    buffer.wr_idx %= IR_BUFFER_MAX_LENGTH;

}

void ir_buffer_fill_in(void)
{
    uint8_t i;

    for (i = 0; i < IR_BUFFER_FIRST_FILLIN_LENGTH; i++)
    {
        IRM->fifo_in.reg = buffer.buf[buffer.rd_idx].u32;
        buffer.rd_idx++;
        buffer.rd_idx %= IR_BUFFER_MAX_LENGTH;
    }

}
void ir_buffer_repeat_fill_in(void)
{
    uint8_t i;
    for (i = 0; i < IR_BUFFER_INT_LENGTH; i++)
    {
        if (buffer.rd_idx == buffer.wr_idx)
        {
            break;
        }
        IRM->fifo_in.reg = buffer.buf[buffer.rd_idx].u32;
        buffer.rd_idx++;
        buffer.rd_idx %= IR_BUFFER_MAX_LENGTH;
    }
}

void ir_enable(void)
{
    IRM->ir_cmd.bit.ir_ena = ENABLE;
}

void ir_disable(void)
{
    IRM->ir_cmd.bit.ir_dis = ENABLE;
}


void ir_start(void)
{
    ir_state = TRANSMITTING;

    IRM->ir_cmd.bit.ir_start = ENABLE;
}

void ir_stop(void)
{
    ir_state = IDLE;

    IRM->ir_cmd.bit.ir_start = DISABLE;
}

uint8_t Ir_Fifo_Level(void)
{
    return IRM->status.bit.fifo_lvl;
}

uint8_t Ir_Fifo_Empty(void)
{
    return IRM->status.bit.fifo_empty;
}

uint8_t Ir_Fifo_Full(void)
{
    return IRM->status.bit.fifo_full;
}

/**
 * @ingroup IRM_Driver
 * @brief IRM interrupt
 * @details
 * @return
 */
void IRM_Handler()
{
    volatile uint32_t irm_sr_status;

    irm_sr_status = IRM->INT_STATUS.reg;

    if (irm_sr_status & ENV_START_INT)
    {
        Ir_Buffer_Repeat_Fill_In();
    }
    if (irm_sr_status & ENV_LAST_INT)
    {
        /* This interrupt is triggered only when the envelope configuration in FIFO with the last bit is executed.
        *   After the envelope configuration with the last bit is executed, the IR module will stop and disable automatically.
        *   If one wants to fill FIFO with new envelope configurations , the IR module needs to be enabled again.
        *
        */
        ir_state = DONE;
    }
    if (irm_sr_status & FIFO_UFL_INT)
    {
        // FIFO underflow
    }

    IRM->INT_CLR.reg = irm_sr_status;
    user_cb(irm_sr_status);
}
