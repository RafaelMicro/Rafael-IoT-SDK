/**************************************************************************//**
 * @file     i2c_slave.c
 * @version
 * @brief    I2C SLAVE driver
 *
 * @copyright
 ******************************************************************************/

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "i2c_slave.h"


#define I2C_SLAVE_STATE_CLOSE            0
#define I2C_STATE_STATE_OPEN             1

#define I2C_SLAVE_STATE_IDLE             2
#define I2C_SLAVE_STATE_READ             3
#define I2C_SLAVE_STATE_WRITE            4


static i2c_slave_proc_cb user_slave_cb = NULL;
static uint8_t i2c_slave_state = I2C_SLAVE_STATE_CLOSE;


uint32_t i2c_slave_open(i2c_slave_mode_t *i2c_slave_client)
{
    /*
     * Because I2C slave driver is interrupt driven, there must have user
     * i2c_slave_cb_func to process master request. This driver just handle
     * i2c slave bus interrupt, NOT i2c receive/send data.
     */
    if ((i2c_slave_client == NULL) || (i2c_slave_client->i2c_slave_cb_func == NULL))
    {
        return STATUS_INVALID_PARAM;
    }

    enter_critical_section();

    if (i2c_slave_state != I2C_SLAVE_STATE_CLOSE)
    {
        leave_critical_section();
        return STATUS_EBUSY;            /*SLAVE DEVICE already opened*/
    }

    i2c_slave_state = I2C_STATE_STATE_OPEN;
    leave_critical_section();

    user_slave_cb = i2c_slave_client->i2c_slave_cb_func;

    I2C_SLAVE->i2c_slave_enable = 0;    /*disable I2C slave first*/

    /*clear all interrupt*/
    I2C_SLAVE->i2c_int_status = I2C_SLAVE_ALL_INT;

    /*set I2C slave address*/
    I2C_SLAVE->i2c_slave_addr = (i2c_slave_client->i2c_slave_addr << 1) & 0xFF;

    I2C_SLAVE->i2c_int_enable =  I2C_SLAVE_ALL_INT;

    if (i2c_slave_client->i2c_bus_timeout_enable)
    {
        I2C_SLAVE->i2c_timeout = (i2c_slave_client->i2c_bus_timeout << 8) | 1;
    }
    else
    {
        I2C_SLAVE->i2c_timeout = 0;
    }

    /*clear Pending Interrupt*/
    NVIC_ClearPendingIRQ(I2C_Slave_IRQn);

    I2C_SLAVE->i2c_slave_enable = 1;

    /*enable NVIC I2C interrrupt */
    NVIC_EnableIRQ(I2C_Slave_IRQn);

    return STATUS_SUCCESS;
}

uint32_t i2c_slave_close(void)
{

    NVIC_DisableIRQ(I2C_Slave_IRQn);
    I2C_SLAVE->i2c_slave_enable = 0;     /*disable I2C slave first*/
    I2C_SLAVE->i2c_int_enable = 0;
    /*clear all interrupt*/
    I2C_SLAVE->i2c_int_status = I2C_SLAVE_ALL_INT;

    enter_critical_section();
    i2c_slave_state = I2C_SLAVE_STATE_CLOSE;
    leave_critical_section();

    return STATUS_SUCCESS;
}

/**
 * @ingroup I2C_Slave_Driver
 * @brief I2C Slave interrupt
 * @details
 * @return
 */
void I2C_Slave_Handler(void)
{
    uint32_t i2c_slave_status;
    uint8_t  temp;

    if (user_slave_cb == NULL)
    {
        /*serious bug... why? */
        while (1);
    }

    i2c_slave_status = I2C_SLAVE->i2c_int_status;

    if (i2c_slave_status & I2C_SLAVE_BUS_TIMEOUT)
    {
        /*i2c slave device return to IDLE mode*/
        i2c_slave_state = I2C_SLAVE_STATE_IDLE;
        user_slave_cb(I2C_SLAVE_STATUS_TIMEOUT);        /*It's error*/
        I2C_SLAVE->i2c_int_status = I2C_SLAVE_BUS_TIMEOUT;
    }
    else
    {
        if (i2c_slave_status & I2C_SLAVE_STATUS_STOP)
        {
            /* i2c master finish i2c command --
             * notice: we don't report this is read or write stop.
             */
            i2c_slave_state = I2C_SLAVE_STATE_IDLE;
            user_slave_cb(I2C_SLAVE_STATUS_STOP);       /*notify I2C command end*/
            I2C_SLAVE->i2c_int_status = I2C_SLAVE_STATUS_STOP;
        }
        else if (i2c_slave_status & I2C_SLAVE_MATCH_ADDR)
        {
            I2C_SLAVE->i2c_int_status = I2C_SLAVE_MATCH_ADDR;       /*write 1 clear*/

            /* check read operation or write operation (I2C Master viewpoint) */
            if (I2C_SLAVE->i2c_int_status & I2C_SLAVE_READ_OP)
            {
                /*NOTICE: it is possible the two start combined condition without no stop condition*/
                i2c_slave_state = I2C_SLAVE_STATE_READ;

                user_slave_cb(I2C_SLAVE_STATUS_READ | I2C_SLAVE_STATUS_ADDR_MATCH);       /*notify I2C master a new request to read data */

                /*user callback should sent data back to I2C master*/
            }
            else
            {
                /*I2C_SLAVE_STATUS bit1 must be 0*/
                i2c_slave_state = I2C_SLAVE_STATE_WRITE;

                /*Dummy read rx to release SCL*/
                temp = I2C_SLAVE->rx_data;

                /*this is debug check --- it should be zero*/
                if ((temp & BIT0) == 1)
                {
                    printf("write flag with BIT0 HIGH? \n");
                    while (1);
                }

                user_slave_cb(I2C_SLAVE_STATUS_WRITE | I2C_SLAVE_STATUS_ADDR_MATCH);    /*notify I2C master a new request to write data */
            }
        }
        else if (i2c_slave_status & I2C_SLAVE_DATA_READY)
        {
            /*clear interrupt*/
            I2C_SLAVE->i2c_int_status = I2C_SLAVE_DATA_READY;       /*write 1 clear*/

            if (I2C_SLAVE->i2c_slave_status & I2C_SLAVE_READ_OP)
            {
                user_slave_cb(I2C_SLAVE_STATUS_READ | I2C_SLAVE_STATUS_DATA_READY);     /*notify I2C master request to read data */
            }
            else
            {
                user_slave_cb(I2C_SLAVE_STATUS_WRITE | I2C_SLAVE_STATUS_DATA_READY);     /*notify I2C master request to read data */
            }

        }
        else
        {
            /*debug check --- fatal Error?*/
            printf("why enter i2c slave fatal Error? \n");
            while (1);
        }
    }
}
