/**
 * @file rfb_sf.c
 * @author
 * @date
 * @brief porting ruci related function for RFB special function
 *
 * More detailed description can go here
 *
 *
 * @see http://
 */
/**************************************************************************************************
*    INCLUDES
*************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include "mcu.h"
#include "ruci.h"
#include "rf_mcu.h"
#include "rf_tx_comp.h"
#include "sadc.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define SIZE_HEADER             (sizeof(ruci_head_t))
#define SIZE_SUBHEADER          (1)
#define SIZE_RET_VALUE          (1)

/* SF host event status */
#define RFB_SF_CMD_FAIL         (0)
#define RFB_SF_CMD_SUCCESS      (1)

/* SF event status */
#define SF_EVENT_SUCCESS        (0x0)
#define SF_EVENT_INVALID        (0x1)
#define SF_EVENT_RX_BUSY        (0x2)
#define SF_EVENT_TX_BUSY        (0x3)
#define SF_EVENT_SYS_BUSY       (0x4)
#define SF_EVENT_NOT_SUPPORT    (0x5)

/**************************************************************************************************
 *    GLOBAL VARIABLES
 *************************************************************************************************/


/**************************************************************************************************
 *    LOCAL FUNCTIONS
 *************************************************************************************************/
uint16_t rfb_sf_sfr_read(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_sfr_read_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_sfr_read_t *)(p_input_buf);
    p_cmd_out = (ruci_para_sfr_read_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if (p_cmd_in->addr > 0xF)
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }

    cmd_out_len += 1;

#if 1
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;
    *((&p_cmd_out->sub_header) + 2) = RfMcu_SpiSfrGet(p_cmd_in->addr);
    return (cmd_out_len);
#else
    /* the SFR read should be refine in RT57x */
    return 0;
#endif

}

uint16_t rfb_sf_sfr_write(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_sfr_write_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_sfr_write_t *)(p_input_buf);
    p_cmd_out = (ruci_para_sfr_write_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if (p_cmd_in->addr > 0xF)
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }

#if 1
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;
    RfMcu_SpiSfrSet(p_cmd_in->addr, p_cmd_in->data);
    return (cmd_out_len);
#else
    /* the SFR read should be refine in RT57x */
    return 0;
#endif

}

uint16_t rfb_sf_io_read(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_io_read_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_io_read_t *)(p_input_buf);
    p_cmd_out = (ruci_para_io_read_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if ((p_cmd_in->data_length > 2117) || (p_cmd_in->data_length > max_uart_len) || (p_cmd_in->q_idx > 1))
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }
    cmd_out_len += p_cmd_in->data_length;
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;

    RfMcu_IoGet(p_cmd_in->q_idx, ((&p_cmd_out->sub_header) + 2), p_cmd_in->data_length);

    return (cmd_out_len);
}

uint16_t rfb_sf_io_write(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_io_write_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_io_write_t *)(p_input_buf);
    p_cmd_out = (ruci_para_io_write_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if ((p_cmd_in->data_length > 2110) || (p_cmd_in->data_length > max_uart_len) || (p_cmd_in->q_idx > 7))
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;

    RfMcu_IoSet(p_cmd_in->q_idx, p_cmd_in->data, p_cmd_in->data_length);

    return (cmd_out_len);
}

uint16_t rfb_sf_mem_read(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_mem_read_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_mem_read_t *)(p_input_buf);
    p_cmd_out = (ruci_para_mem_read_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if ((p_cmd_in->data_length > 2048) || (p_cmd_in->data_length > max_uart_len))
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }

    cmd_out_len += p_cmd_in->data_length;
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;

    /* wake up IC before read memory */
    do
    {
        RfMcu_HostWakeUpMcu();
    } while (RfMcu_PowerStateCheck() != 0x03);

    RfMcu_MemoryGet(p_cmd_in->addr, ((&p_cmd_out->sub_header) + 2), p_cmd_in->data_length);

    return (cmd_out_len);

}

uint16_t rfb_sf_mem_write(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_mem_write_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_mem_write_t *)(p_input_buf);
    p_cmd_out = (ruci_para_mem_write_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    if ((p_cmd_in->data_length > 2048) || (p_cmd_in->data_length > max_uart_len))
    {
        *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_FAIL;
        return (cmd_out_len);
    }
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;

    /* wake up IC before write memory */
    do
    {
        RfMcu_HostWakeUpMcu();
    } while (RfMcu_PowerStateCheck() != 0x03);

    RfMcu_MemorySet(p_cmd_in->addr, p_cmd_in->data, p_cmd_in->data_length);

    return (cmd_out_len);
}

uint16_t  rfb_sf_host_reg_read(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_reg_read_t *p_cmd_in, *p_cmd_out;

    p_cmd_in = (ruci_para_reg_read_t *)(p_input_buf);
    p_cmd_out = (ruci_para_reg_read_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    cmd_out_len += 4;
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;
    *((uint32_t *)((&p_cmd_out->sub_header) + 2)) = inp32(p_cmd_in->addr);

    return (cmd_out_len);
}

uint16_t  rfb_sf_host_reg_write(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    uint16_t cmd_out_len = SIZE_HEADER + SIZE_SUBHEADER + SIZE_RET_VALUE;
    ruci_para_reg_write_t *p_cmd_in, *p_cmd_out;
    uint16_t delay_cnt;

    p_cmd_in = (ruci_para_reg_write_t *)(p_input_buf);
    p_cmd_out = (ruci_para_reg_write_t *)(p_output_buf);
    p_cmd_out->ruci_header.u8 = p_cmd_in->ruci_header.u8;
    p_cmd_out->sub_header = p_cmd_in->sub_header;
    *((&p_cmd_out->sub_header) + 1) = RFB_SF_CMD_SUCCESS;
    outp32(p_cmd_in->addr, p_cmd_in->data);

    // Delay 100us for avoiding sleep mode issue
    for (delay_cnt = 0; delay_cnt < 32000; delay_cnt++);

    return (cmd_out_len);
}

uint16_t  rfb_sf_host_sys_tx_pwr_comp_cfg(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    ruci_para_tx_pwr_comp_cfg_t *p_cmd_in;
    ruci_para_sf_cnf_event_t *p_cmd_out;
    uint8_t retStatus;

    p_cmd_in = (ruci_para_tx_pwr_comp_cfg_t *)(p_input_buf);
    p_cmd_out = (ruci_para_sf_cnf_event_t *)(p_output_buf);
#if (RF_TX_POWER_COMP)
    Tx_Power_Compensation_Deinit();
    retStatus = SF_EVENT_SUCCESS;
    if (p_cmd_in->interval)
    {
        Tx_Power_Compensation_Init(p_cmd_in->interval);
    }
#else
    retStatus = SF_EVENT_NOT_SUPPORT;
#endif

    SET_RUCI_PARA_SF_CNF_EVENT(p_cmd_out, p_cmd_in->ruci_header.u8, p_cmd_in->sub_header, retStatus);
    return (RUCI_LEN_SF_CNF_EVENT);
}

uint16_t rfb_sf_host_hdlr(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    switch (p_input_buf[SIZE_HEADER])
    {
    case (RUCI_CODE_SFR_READ):
        /* SFR read */
        return rfb_sf_sfr_read(p_input_buf, p_output_buf);
    case (RUCI_CODE_SFR_WRITE):
        /* SFR write */
        return rfb_sf_sfr_write(p_input_buf, p_output_buf);
    case (RUCI_CODE_IO_READ):
        /* IO read */
        return rfb_sf_io_read(p_input_buf, p_output_buf, max_uart_len);
    case (RUCI_CODE_IO_WRITE):
        /* IO write */
        return rfb_sf_io_write(p_input_buf, p_output_buf, max_uart_len);
    case (RUCI_CODE_MEM_READ):
        /* Memory read */
        return rfb_sf_mem_read(p_input_buf, p_output_buf, max_uart_len);
    case (RUCI_CODE_MEM_WRITE):
        /* Memory write */
        return rfb_sf_mem_write(p_input_buf, p_output_buf, max_uart_len);
    case (RUCI_CODE_REG_READ):
        /* Host register read */
        return rfb_sf_host_reg_read(p_input_buf, p_output_buf);
    case (RUCI_CODE_REG_WRITE):
        /* Host register write */
        return rfb_sf_host_reg_write(p_input_buf, p_output_buf);
    default:
        return 0;
    }
}

uint16_t rfb_sf_host_sys_hdlr(uint8_t *p_input_buf, uint8_t *p_output_buf)
{
    switch (p_input_buf[SIZE_HEADER])
    {
    case (RUCI_CODE_TX_PWR_COMP_CFG):
        /* TX power compensation configuration */
        return rfb_sf_host_sys_tx_pwr_comp_cfg(p_input_buf, p_output_buf);
    default:
        return 0;
    }
}

/**************************************************************************************************
 *    GLOBAL FUNCTIONS
 *************************************************************************************************/
uint16_t rfb_sf_hdlr(uint8_t *p_input_buf, uint8_t *p_output_buf, uint16_t max_uart_len)
{
    p_ruci_head_t p_ruci_head;

    p_ruci_head = (p_ruci_head_t)(p_input_buf);
    switch (p_ruci_head->u8)
    {
    case (RUCI_SF_HOST_CMD_HEADER):
        /* Special funtion host command parser */
        return rfb_sf_host_hdlr(p_input_buf, p_output_buf, max_uart_len);
    case (RUCI_SF_HOST_SYS_CMD_HEADER):
        /* Special function host system command parser */
        return rfb_sf_host_sys_hdlr(p_input_buf, p_output_buf);
    default:
        return 0;
    }
}
