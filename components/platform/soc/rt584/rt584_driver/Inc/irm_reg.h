/**************************************************************************//**
 * @file     irm_reg.h
 * @version  V1.00
 * @brief    IRM register definition header file
 *
 * @copyright
 *****************************************************************************/
/** @defgroup IRM_Register IRM
 *  @ingroup  peripheral_group
 *  @{
 *  @brief  IRM_Register header information
*/
#ifndef IRM_REG_H
#define IRM_REG_H

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

//0x00
typedef union irm_conf_s
{
    struct irm_conf_b
    {
        uint32_t reserved     : 1;
        uint32_t op_mode      : 1;
        uint32_t out_mode     : 2;
        uint32_t no_car       : 1;
        uint32_t car_ini      : 1;
        uint32_t reserved2    : 26;
    } bit;
    uint32_t reg;
} irm_conf_t;

//0x04
typedef union irm_carrier_s
{
    struct irm_carrier_b
    {
        uint32_t car_base_cnt     : 16;
        uint32_t car_low_cnt      : 4;
        uint32_t car_high_cnt     : 4;
        uint32_t reserved         : 8;
    } bit;
    uint32_t reg;
} irm_carrier_t;

//0x08
typedef union irm_fifo_in_s
{
    struct irm_fifo_in_b
    {
        uint32_t env_cnt      : 16;
        uint32_t env_mark     : 1;
        uint32_t env_init     : 1;
        uint32_t env_last     : 1;
        uint32_t reserved     : 13;
    } bit;
    uint32_t reg;
} irm_fifo_in_t;

//0x0c
typedef union irm_status_s
{
    struct irm_status_b
    {
        uint32_t fifo_lvl       : 5;
        uint32_t fifo_full      : 1;
        uint32_t fifo_empty     : 1;
        uint32_t reserved       : 25;
    } bit;
    uint32_t reg;
} irm_status_t;

//0x10
typedef union irm_cmd_s
{
    struct irm_cmd_b
    {
        uint32_t ir_ena       : 1;
        uint32_t ir_dis       : 1;
        uint32_t ir_start     : 1;
        uint32_t ir_rst       : 1;
        uint32_t reserved     : 28;
    } bit;
    uint32_t reg;
} irm_cmd_t;

//0x14
typedef union irm_int_status_s
{
    struct irm_int_status_b
    {
        uint32_t env_start_int      : 1;
        uint32_t env_last_int       : 1;
        uint32_t env_ufl_int        : 1;
        uint32_t reserved           : 29;
    } bit;
    uint32_t reg;
} irm_int_status_t;

//0x18
typedef union irm_int_ena_s
{
    struct irm_int_ena_b
    {
        uint32_t env_start_ena      : 1;
        uint32_t env_last_ena       : 1;
        uint32_t env_ufl_ena        : 1;
        uint32_t reserved           : 29;
    } bit;
    uint32_t reg;
} irm_int_ena_t;

//0x1C
typedef union irm_int_clr_s
{
    struct irm_int_clr_b
    {
        uint32_t env_start_clr      : 1;
        uint32_t env_last_clr       : 1;
        uint32_t env_ufl_clr        : 1;
        uint32_t reserved           : 29;
    } bit;
    uint32_t reg;
} irm_int_clr_t;

typedef struct
{
    __IO irm_conf_t         ir_conf;        /*0x00*/
    __IO irm_carrier_t      carrier ;       /*0x04*/
    __IO irm_fifo_in_t      fifo_in;        /*0x08*/
    __I irm_status_t        status ;        /*0x0c*/
    __IO irm_cmd_t          ir_cmd;         /*0x10*/
    __I irm_int_status_t    int_status;     /*0x14*/
    __IO irm_int_ena_t      int_ena;        /*0x18*/
    __IO irm_int_clr_t      int_clr;        /*0x1c*/

} IRM_T;

/*@}*/ /* end of peripheral_group IRM_Register */

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif

#endif      /* end of __RT584_IRM_REG_H__ */