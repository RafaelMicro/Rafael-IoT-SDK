/**************************************************************************//**
 * @file     i2c_master.c
 * @version
 * @brief    I2C Master driver
 *
 * @copyright
 ******************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "gpio.h"
#include "i2c_master.h"
#include "sysctrl.h"

/*
 * HERE We assume there are multiple i2c devices attached with one I2C master.
 * It is possible for multitask to access the I2C master at same time.
 * So we need critical section to protect.
 *
 */
#define I2C_STATE_UNINIT           0
#define I2C_STATE_IDLE             1
#define I2C_STATE_WRITE            2
#define I2C_STATE_WRITE_DATA       3
#define I2C_STATE_READ             4
#define I2C_STATE_READ_DATA        5
#define I2C_STATE_READ_DATA_STOP   6

/*Please do NOT change the order of these define.*/
#define I2C_STOP_MASK             16     /*BIT5 as stop MASK*/
#define I2C_STATE_NOACK           16
#define I2C_STATE_STOP            17
#define I2C_STATE_ESTOP           18

#define I2C_STATE_HOST_INIT       32     /*this mode only in i2c_init, avoid multi-task i2c_init at same time*/

#define  I2C_MASTER_NUMBER_MAX          2

#define  HIGH_SPEED_DIV        (20-1)
#define  MEDIUM_SPEED_DIV      (40-1)
#define  NORMAL_SPEED_DIV      (80-1)
#define  SUPER_HIGH_SPEED_DIV  (10-1)

#define  I2C_MODE_READ          1
#define  I2C_MODE_WRITE         0

#define  I2C_MAX_DATASIZE      (1024)       /*max data packet size for one transfer*/

/**
 * @brief i2c_master_handle_t save i2c master information
 */
typedef struct
{
    I2C_MASTER_T    *i2c_hw_dev_master;     /*!< based hardware address */
    uint16_t        i2c_state;              /*!< current i2c master state, is critical data */
    uint16_t        i2c_substate;           /*!< current i2c master substate, is critical data */
    uint32_t        remain_data_length;     /*!< the maximum data length is 1K */
    uint32_t        next_read_data_length;  /*!< the data need to read */
    uint8_t         *pdata;                 /*!< save write/read data */
    i2cm_proc_cb    notify_cb;              /*!< user callback */
    IRQn_Type       irq_num;                /*!< IRQ Number */

} i2c_master_handle_t;

static i2c_master_handle_t  m_i2c_master_handle[I2C_MASTER_NUMBER_MAX] =
{
    {
        I2C_MASTER0,
        I2C_STATE_UNINIT,       /*i2c_state   */
        0,                      /*i2c_substate*/
        0,
        0,
        NULL,
        NULL,
        I2C_Master0_IRQn
    },
    {
        I2C_MASTER1,
        I2C_STATE_UNINIT,
        0,
        0,
        0,
        NULL,
        NULL,
        I2C_Master1_IRQn
    }
};

/**
 * @brief rt58x API
 */
uint32_t i2c_preinit(uint32_t SCL_pin, uint32_t SDA_pin)
{
    uint32_t  times = 0;
    uint32_t master_channel, master_mode;

    /*check SCL and SDA pin setting*/
    if ((SCL_pin == 22) && (SDA_pin == 23))
    {
        master_channel = 0;
        master_mode = MODE_I2C;
    }
    else
    {
        return STATUS_INVALID_PARAM;
    }

    pin_set_mode(SCL_pin, MODE_GPIO);     /*set SCL output*/
    pin_set_mode(SDA_pin, MODE_GPIO);     /*set SDA  output*/

    gpio_pin_set(SCL_pin);               /*SCL high*/
    gpio_pin_set(SDA_pin);               /*SDA high*/

    gpio_cfg_output(SCL_pin);
    gpio_cfg_output(SDA_pin);

    /*set  SCL,  SDA  I2C mode.*/
    pin_set_mode(SCL_pin, master_mode);
    pin_set_mode(SDA_pin, master_mode);

    /*set SCL, SDA as open drain mode.*/
    enable_pin_opendrain(SCL_pin);
    enable_pin_opendrain(SDA_pin);

    pin_set_pullopt(SCL_pin, PULLUP_100K);
    pin_set_pullopt(SDA_pin, PULLUP_100K);

    i2c_preinit(master_channel);

    return STATUS_SUCCESS;
}

uint32_t i2c_write(const i2c_slave_data_t *slave, uint8_t *data, uint32_t len, i2cm_proc_cb_t endproc_cb)
{
    uint32_t status;
    i2c_slave_data_ex_t slave_cfg;

    assert_param(slave);           /*slave data should not be NULL*/
    assert_param(data);            /*data should not be NULL*/
    assert_param(endproc_cb);      /*endproc_cb hook should not be NULL*/

    slave_cfg.bFlag_16bits = slave->bFlag_16bits;
    slave_cfg.dev_addr = slave->dev_addr;
    slave_cfg.endproc_cb = (i2cm_proc_cb)endproc_cb;
    slave_cfg.reg_addr = slave->reg_addr;

    status = i2c_master_write(0, &slave_cfg, data, len);

    return status;
}

uint32_t i2c_read(const i2c_slave_data_t *slave,
                  uint8_t *data,
                  uint32_t len,
                  i2cm_proc_cb_t endproc_cb)
{
    uint32_t status;
    i2c_slave_data_ex_t slave_cfg;

    assert_param(slave);           /*slave data should not be NULL*/
    assert_param(data);            /*data should not be NULL*/
    assert_param(endproc_cb);      /*endproc_cb hook should not be NULL*/

    slave_cfg.bFlag_16bits = slave->bFlag_16bits;
    slave_cfg.dev_addr = slave->dev_addr;
    slave_cfg.endproc_cb = (i2cm_proc_cb)endproc_cb;
    slave_cfg.reg_addr = slave->reg_addr;

    status = i2c_master_read(0, &slave_cfg, data, len);

    return status;
}
/**
 * @brief End of rt58x API
 */



/*
* We write this function to avoid multi-tasking access for i2c host
* Because this function is "local" function only, it's master_channel
* must be valid. But we still check it.
*/
static uint32_t i2c_check_state(uint32_t master_channel, uint16_t next_state)
{
    i2c_master_handle_t   *i2c_handle;

    if (master_channel >= I2C_MASTER_NUMBER_MAX)
    {
        return STATUS_INVALID_PARAM;
    }

    i2c_handle = &m_i2c_master_handle[master_channel];

    enter_critical_section();

    if ((i2c_handle->i2c_state) != I2C_STATE_IDLE)
    {
        leave_critical_section();
        if ((i2c_handle->i2c_state) != I2C_STATE_UNINIT)
        {
            return STATUS_EBUSY;    /*I2C host in read/write state... busy..*/
        }
        else
        {
            return STATUS_NO_INIT;
        }
    }
    i2c_handle->i2c_state = next_state;        /*so other task can not read/write now.*/

    leave_critical_section();

    return STATUS_SUCCESS;
}

uint32_t i2c_preinit(uint32_t master_channel)
{
    i2c_master_handle_t   *i2c_handle;
    I2C_MASTER_T          *i2c_master;

    if (master_channel >= I2C_MASTER_NUMBER_MAX)
    {
        return STATUS_INVALID_PARAM;
    }

    i2c_handle = &m_i2c_master_handle[master_channel];
    i2c_master = i2c_handle->i2c_hw_dev_master;

    i2c_master->slck_gen = NORMAL_SPEED_DIV;       /*Bit15: Select APB clock (0)*/
    i2c_master->control  = I2CM_CONTROL_ENABLE;

    i2c_master->control  = I2CM_CONTROL_ENABLE | I2CM_CONTROL_FIFO_CLEAR ;

    while (i2c_master->control & I2CM_CONTROL_FIFO_CLEAR);

    i2c_master->control  = I2CM_CONTROL_ENABLE | I2CM_CONTROL_BUS_CLEAR;

    while (i2c_master->control & I2CM_CONTROL_BUS_CLEAR);

    i2c_master->control = 0;

    NVIC_ClearPendingIRQ(i2c_handle->irq_num);

    return STATUS_SUCCESS;
}

uint32_t I2c_Master_Init(uint32_t master_channel, uint32_t i2c_speed)
{
    i2c_master_handle_t   *i2c_handle;
    I2C_MASTER_T          *i2c_master;
    uint32_t              div;

    if (master_channel >= I2C_MASTER_NUMBER_MAX)
    {
        return STATUS_INVALID_PARAM;
    }

    i2c_handle = &m_i2c_master_handle[master_channel];

    enter_critical_section();
    /*We can set i2c host only when i2c_state in idle/uninit mode*/
    if ( (i2c_handle->i2c_state) > I2C_STATE_IDLE)
    {
        leave_critical_section();
        return STATUS_INVALID_REQUEST;
    }

    /*i2c host in UNINIT or IDLE mode now*/
    /*this is temp state, only avoid multi-task call i2c_init*/
    i2c_handle->i2c_state = I2C_STATE_HOST_INIT;

    leave_critical_section();

    if (i2c_speed == I2C_CLOCK_400K)
    {
        div = HIGH_SPEED_DIV;
    }
    else if (i2c_speed == I2C_CLOCK_200K)
    {
        div = MEDIUM_SPEED_DIV;
    }
    else if (i2c_speed == I2C_CLOCK_800K)
    {
        div = SUPER_HIGH_SPEED_DIV;
    }
    else if (i2c_speed == I2C_CLOCK_1M)
    {
        div = 7;
    }
    else
    {
        /*I2C default is 100K*/
        div = NORMAL_SPEED_DIV;
    }

    i2c_master = i2c_handle->i2c_hw_dev_master;

    i2c_master->control = 0;

    //peripheral 16M
    if ( SYSCTRL->sys_clk_ctrl.bit.per_clk_sel == PERCLK_SEL_16M )
    {
        i2c_master->slck_gen = div / 2;
    }
    //peripheral 1M
    else if ( SYSCTRL->sys_clk_ctrl.bit.per_clk_sel == PERCLK_SEL_RCO1M )
    {
        i2c_master->slck_gen = 3;
    }
    //peripheral 32M
    else
    {
        i2c_master->slck_gen = div;
    }

    /*we will use interrupt mode*/
    /*until now, we don't want any interrupt generated.*/
    /*set all interrupt mask, so no interrupt can happen*/
    i2c_master->int_enable = 0;

    /*clear I2C interrupt status register */
    i2c_master->int_clear = I2CM_INT_MASK_ALL;

    /*interrupt priority set */
    /*enable NVIC I2C interrrupt */
    NVIC_EnableIRQ(i2c_handle->irq_num);

    Enter_Critical_Section();
    i2c_handle->i2c_state = I2C_STATE_IDLE;         /*OK, right now we can use I2C host to transfer data*/
    Leave_Critical_Section();

    return STATUS_SUCCESS;
}

uint32_t I2c_Master_Write(uint32_t master_channel,
                          const i2c_slave_data_ex_t *slave,
                          uint8_t *data,
                          uint32_t len)
{
    uint32_t status, avalible_fifos;
    i2c_master_handle_t   *i2c_handle;
    I2C_MASTER_T          *i2c_master;

    if (master_channel >= I2C_MASTER_NUMBER_MAX)
    {
        return STATUS_INVALID_PARAM;
    }

    assert_param(slave);           /*slave data should not be NULL*/
    assert_param(data);            /*data should not be NULL*/

    if (len > I2C_MAX_DATASIZE)
    {
        return STATUS_INVALID_PARAM;    /*packet data is > 1K bytes...*/
    }

    if (slave->endproc_cb == NULL)
    {
        return STATUS_INVALID_PARAM;    /*No I2C complete callback function.*/
    }

    status = I2c_Check_State(master_channel, I2C_STATE_WRITE);

    if (status)
    {
        return status;
    }

    i2c_handle = &m_i2c_master_handle[master_channel];
    i2c_master = i2c_handle->i2c_hw_dev_master;

    i2c_handle->pdata = data;
    i2c_handle->remain_data_length = len;
    i2c_handle->notify_cb = slave->endproc_cb;

    i2c_handle->i2c_substate = I2C_STATE_WRITE_DATA;

    i2c_master = i2c_handle->i2c_hw_dev_master;

    /*Here Command FIFO should be empty.*/
    if (!( (i2c_master->int_raw_status) & I2CM_INT_TX_EMPTY))
    {
#ifdef  DEBUG
        printf("Warning Check why I2C CMDFIFO is not empty \n");
#endif
    }

    //Set Address.
    i2c_master->tar = slave->dev_addr;

    /* In fact, available fifo is 16 bytes..
     * It always has at least one byte for address register,
     * so we decrease it first.
     */
    avalible_fifos = I2CM_FIFO_NUM - 1;

    //Write data to I2C device
    if (slave->bFlag_16bits)
    {
        avalible_fifos--;
        i2c_master->buf = (slave->reg_addr >> 8);
    }

    i2c_master->buf = (slave->reg_addr & 0xFF);

    if (avalible_fifos > len)
    {
        avalible_fifos = len;
        i2c_handle->remain_data_length = 0;
    }
    else
    {
        i2c_handle->remain_data_length = len - avalible_fifos;
    }

    while (avalible_fifos > 0)
    {
        i2c_master->buf = *(i2c_handle->pdata);
        i2c_handle->pdata++;
        avalible_fifos--;
    }

    /*enable interrupt of I2CM_INT_TX_EMPTY/I2CM_INT_ADDR_NACK/I2CM_INT_WRITE_NACK*/
    i2c_master->int_enable = (I2CM_INT_TX_OVER | I2CM_INT_TX_EMPTY | I2CM_INT_ADDR_NACK | I2CM_INT_WRITE_NACK | I2CM_INT_LOSTARB);

    if (i2c_handle->remain_data_length)
    {
        //Enable I2C controller.
        /*enable interrupt of I2CM_INT_TX_EMPTY/I2CM_INT_ADDR_NACK/I2CM_INT_WRITE_NACK*/
        i2c_master->control = I2CM_CONTROL_ENABLE;
    }
    else
    {
        //all data already in fifo.
        i2c_handle->i2c_substate = I2C_STATE_STOP;
        i2c_master->control = I2CM_CONTROL_ENABLE | I2CM_CONTROL_STOP_EN;
    }

    return STATUS_SUCCESS;
}

uint32_t I2c_Master_Read(uint32_t master_channel,
                         const i2c_slave_data_ex_t  *slave,
                         uint8_t  *data,
                         uint32_t len)
{
    uint32_t status;
    uint32_t avalible_fifos;

    i2c_master_handle_t   *i2c_handle;
    I2C_MASTER_T          *i2c_master;

    if (master_channel >= I2C_MASTER_NUMBER_MAX)
    {
        return STATUS_INVALID_PARAM;
    }

    assert_param(slave);           /*slave data should not be NULL*/
    assert_param(data);            /*data should not be NULL*/

    if ((len > I2C_MAX_DATASIZE) || (len == 0))
    {
        return STATUS_INVALID_PARAM;    /*packet data is > 1K bytes. or zero bytes?*/
    }

    if (slave->endproc_cb == NULL)
    {
        return STATUS_INVALID_PARAM;    /*I2C complete function must exist*/
    }

    status = i2c_check_state(master_channel, I2C_STATE_READ);

    if (status)
    {
        return status;
    }

    i2c_handle = &m_i2c_master_handle[master_channel];
    i2c_master = i2c_handle->i2c_hw_dev_master;

    i2c_handle->pdata = data;

    i2c_handle->remain_data_length = len;

    i2c_handle->notify_cb = slave->endproc_cb;

    i2c_handle->i2c_substate = I2C_STATE_READ_DATA;

    /*Here Command FIFO should be empty.*/
    if (!(i2c_master->int_raw_status & I2CM_INT_TX_EMPTY))
    {
#ifdef  DEBUG
        printf("Warning Check why I2C CMDFIFO is not empty \n");
#endif
    }

    //Set Address.
    i2c_master->tar = slave->dev_addr ;

    /* In fact, available fifo is 16 bytes..
     * It always has at least one byte for address register,
     * so we decrease it first.
     */
    avalible_fifos = I2CM_FIFO_NUM - 1;

    //Write register data to I2C device
    if (slave->bFlag_16bits)
    {
        avalible_fifos--;
        i2c_master->buf = (slave->reg_addr >> 8);
    }

    i2c_master->buf = (slave->reg_addr & 0xFF);

    if (avalible_fifos > len)
    {
        avalible_fifos = len;
    }

    i2c_handle->next_read_data_length = avalible_fifos;
    i2c_handle->remain_data_length -= avalible_fifos;

    //read data
    while (avalible_fifos > 0)
    {
        i2c_master->buf = 0x100;
        avalible_fifos--;
    }

    /*enable interrupt of I2CM_INT_RX_FIINISH/I2CM_INT_ADDR_NACK/I2CM_INT_WRITE_NACK*/
    i2c_master->int_enable = (I2CM_INT_RX_FINISH | I2CM_INT_ADDR_NACK | I2CM_INT_WRITE_NACK);

    if (i2c_handle->remain_data_length)
    {
        //Enable I2C controller.
        i2c_master->control = I2CM_CONTROL_ENABLE | I2CM_CONTROL_RESTART;
    }
    else
    {
        //all data already in fifo.
        //i2c_substate = I2C_STATE_READ_DATA_STOP;
        i2c_master->control = I2CM_CONTROL_ENABLE | I2CM_CONTROL_STOP_EN | I2CM_CONTROL_RESTART;
    }

    return STATUS_SUCCESS;
}

/**
 * @ingroup I2C_Master_Driver
 * @brief I2C Master handler
 * @details
 * @return
 */
static void I2C_Master_Handler(i2c_master_handle_t *handle)
{
    uint32_t   status;
    uint32_t   ret_status;
    uint32_t   avalible_fifos;
    volatile   uint32_t delay;

    I2C_MASTER_T    *i2c_master;

    i2c_master = handle->i2c_hw_dev_master;

    /*read interrupt status and clear.*/
    status = i2c_master->int_status;
    i2c_master->int_clear = status;

    /*Disable all interrupt*/
    i2c_master->int_enable = 0;

    if (status & I2CM_INT_IDLE)
    {
        handle->i2c_state = I2C_STATE_IDLE;
        ret_status = I2C_STATUS_OK;

        if (handle->i2c_substate == I2C_STATE_READ_DATA_STOP)
        {
            /*this is read end*/
            while (handle->next_read_data_length > 0)
            {
                *(handle->pdata) = i2c_master->buf;       /*read data*/
                handle->pdata++;
                handle->next_read_data_length--;
            }

            /*debug check*/
            if (handle->remain_data_length != 0)
            {
                /*debug check */
#ifdef  DEBUG
                printf("\t check why remain_data_length!=0 \n");
#endif
            }
        }
        else if ((handle->i2c_substate) == I2C_STATE_STOP)
        {
            /*this is write end.*/
        }
        else
        {
            /*this is error NACK end*/
            ret_status = I2C_STATUS_ERR_NOACK;
        }

        /*check Stop_EN should be clear. this check is clear very soon.*/
        while ((i2c_master->control) & I2CM_CONTROL_STOP_EN);

        i2c_master->control = 0;      /*if we are in idle we will ce I2C*/

        handle->notify_cb(ret_status);     /*let application hook to know error*/
        return;
    }


    if (status & (I2CM_INT_ADDR_NACK | I2CM_INT_WRITE_NACK))
    {
        /*error. no address device or write NACK (device is busy)... */
        handle->remain_data_length = 0;         /*stop to send/receive any data. because error*/
        handle->i2c_substate = I2C_STATE_ESTOP;

        /*stop will auto see when controller see NACK error*/
        /*but current stop happened?*/

        /*clear FIFO*/
        i2c_master->control = (I2CM_CONTROL_ENABLE | I2CM_CONTROL_FIFO_CLEAR);

        while ((i2c_master->control) & I2CM_CONTROL_FIFO_CLEAR);
        /*wait this bit clear.*/

        /* i2c_idle ... this is interrupt ISR..
         *  so it does NOT worry about critical section
         */
        /*
         * Because code running in flash, sometimes it need times to load code to
         * execute...for this case, stop has been sent so I2C bus become idle.
         * But if code already in cache, then it will need 10us to send stop condition
         * for this case we using iterrupt to wait idle state.
         *
         */

        if ((i2c_master->int_raw_status) & I2CM_INT_IDLE)
        {
            i2c_master->control = 0;      /*if we are in idle we will close I2C*/
            handle->i2c_state = I2C_STATE_IDLE;
            ret_status = I2C_STATUS_ERR_NOACK;
            handle->notify_cb(ret_status);     /*let application hook to know error*/
        }
        else
        {
            handle->i2c_substate = I2C_STATE_ESTOP;
            i2c_master->int_enable = I2CM_INT_IDLE;
        }
        return;
    }


    if (status & I2CM_INT_TX_EMPTY)
    {
        /*this must be write state.*/

        /*write command*/
        avalible_fifos = I2CM_FIFO_NUM;

        if (avalible_fifos > handle->remain_data_length)
        {
            avalible_fifos = handle->remain_data_length;
            handle->remain_data_length = 0;
        }
        else
        {
            handle->remain_data_length -= avalible_fifos;
        }

        while (avalible_fifos > 0)
        {
            i2c_master->BUF = *(handle->pdata);
            handle->pdata++;
            avalible_fifos--;
        }

        if (handle->remain_data_length)
        {
            //Enable I2C controller.

            i2c_master->INT_ENABLE =  (I2CM_INT_TX_EMPTY | I2CM_INT_WRITE_NACK);
            i2c_master->CONTROL = I2CM_CONTROL_ENABLE;
        }
        else
        {
            //all data already in fifo.
            handle->i2c_substate = I2C_STATE_STOP;

            i2c_master->INT_ENABLE =  (I2CM_INT_IDLE | I2CM_INT_WRITE_NACK);
            i2c_master->CONTROL = I2CM_CONTROL_ENABLE | I2CM_CONTROL_STOP_EN;
        }
    }

    if (status & I2CM_INT_RX_FINISH)
    {
        /*this must be read state*/

        while (handle->next_read_data_length > 0)
        {
            *(handle->pdata) = i2c_master->BUF;       /*read data*/
            handle->pdata++;
            handle->next_read_data_length--;
        }

        /*read next */
        avalible_fifos = I2CM_FIFO_NUM;

        if (avalible_fifos > handle->remain_data_length)
        {
            avalible_fifos = handle->remain_data_length;
        }

        handle->next_read_data_length = avalible_fifos;
        handle->remain_data_length -= avalible_fifos;

        //read data
        while (avalible_fifos > 0)
        {
            i2c_master->BUF = 0x100;
            avalible_fifos--;
        }

        if (handle->remain_data_length)
        {
            //Enable I2C controller.
            /*enable interrupt of I2CM_INT_RX_FINISH/I2CM_INT_ADDR_NACK/I2CM_INT_WRITE_NACK*/
            i2c_master->INT_ENABLE = (I2CM_INT_RX_FINISH | I2CM_INT_ADDR_NACK | I2CM_INT_WRITE_NACK);
            i2c_master->CONTROL = I2CM_CONTROL_ENABLE | I2CM_CONTROL_RESTART;
        }
        else
        {
            //all data already in fifo.
            handle->i2c_substate = I2C_STATE_READ_DATA_STOP;
            /*enable interrupt of I2CM_INT_TX_EMPTY/I2CM_INT_ADDR_NACK/I2CM_INT_WRITE_NACK*/
            i2c_master->INT_ENABLE = (I2CM_INT_IDLE | I2CM_INT_ADDR_NACK | I2CM_INT_WRITE_NACK);
            i2c_master->CONTROL = I2CM_CONTROL_ENABLE | I2CM_CONTROL_STOP_EN | I2CM_CONTROL_RESTART;
        }
    }
    return;
}

/**
 * @ingroup I2C_Master_Driver
 * @brief I2C Master interrupt
 * @details
 * @return
 */
void I2C_Master0_Handler(void)
{
    I2C_Master_Handler(&m_i2c_master_handle[0]);
}

/**
 * @ingroup I2C_Master_Driver
 * @brief I2C Master interrupt
 * @details
 * @return
 */
void I2C_Master1_Handler(void)
{
    I2C_Master_Handler(&m_i2c_master_handle[1]);
}
