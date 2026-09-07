#include <stdint.h>
#include "epd_driver.h"
#include "hosal_gpio.h"
#include "hosal_sysctrl.h"
#include "hosal_qspi.h"

#define HW_SPI              0
#define RAFAEL_A203_BOARD   1

#if (RAFAEL_A203_BOARD == 1)
#define EPD_BUSY_PIN        GPIO5
#define EPD_RESET_PIN       GPIO6
#define EPD_DC_PIN          GPIO7

#if (HW_SPI == 1)
HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 28, 9, 29, 2, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);
#else
#define EPD_CS_PIN          GPIO9
#define EPD_SCK_PIN         GPIO28
#define EPD_SDA_PIN         GPIO29

#define EPD_CS_PIN_LOW      hosal_gpio_pin_clear(EPD_CS_PIN)
#define EPD_CS_PIN_HIGH     hosal_gpio_pin_set(EPD_CS_PIN)

#define EPD_CLK_PIN_LOW     hosal_gpio_pin_clear(EPD_SCK_PIN)
#define EPD_CLK_PIN_HIGH    hosal_gpio_pin_set(EPD_SCK_PIN)

#define EPD_SDA_PIN_LOW     hosal_gpio_pin_clear(EPD_SDA_PIN)
#define EPD_SDA_PIN_HIGH    hosal_gpio_pin_set(EPD_SDA_PIN)
#endif

#define EPD_DC_PIN_LOW      hosal_gpio_pin_clear(EPD_DC_PIN)
#define EPD_DC_PIN_HIGH     hosal_gpio_pin_set(EPD_DC_PIN)

#define EPD_RESET_LOW       hosal_gpio_pin_clear(EPD_RESET_PIN)     // RESET Low
#define EPD_RESET_HIGH      hosal_gpio_pin_set(EPD_RESET_PIN)       // RESET High
#else
#define EPD_BUSY_PIN        GPIO5
#define EPD_RESET_PIN       GPIO8
#define EPD_DC_PIN          GPIO9

#if (HW_SPI == 1)
HOSAL_QSPI_DEV_DECL(qspi_dev0, QSPI0, 15, 14, 20, 29, 0, 0, HOSAL_QSPI_BAUDRATE_1M, HOSAL_QSPI_MASTER_MODE);
#else
#define EPD_CS_PIN          GPIO14
#define EPD_SCK_PIN         GPIO15
#define EPD_SDA_PIN         GPIO20

#define EPD_CS_PIN_LOW      hosal_gpio_pin_clear(EPD_CS_PIN)
#define EPD_CS_PIN_HIGH     hosal_gpio_pin_set(EPD_CS_PIN)

#define EPD_CLK_PIN_LOW     hosal_gpio_pin_clear(EPD_SCK_PIN)
#define EPD_CLK_PIN_HIGH    hosal_gpio_pin_set(EPD_SCK_PIN)

#define EPD_SDA_PIN_LOW     hosal_gpio_pin_clear(EPD_SDA_PIN)
#define EPD_SDA_PIN_HIGH    hosal_gpio_pin_set(EPD_SDA_PIN)
#endif

#define EPD_DC_PIN_LOW      hosal_gpio_pin_clear(EPD_DC_PIN)
#define EPD_DC_PIN_HIGH     hosal_gpio_pin_set(EPD_DC_PIN)

#define EPD_RESET_LOW       hosal_gpio_pin_clear(EPD_RESET_PIN)     // RESET Low
#define EPD_RESET_HIGH      hosal_gpio_pin_set(EPD_RESET_PIN)       // RESET High
#endif

#define SOURCE_PIXEL      128
#define GATE_PIXEL        250

void epd_spi_writebyte(uint8_t data)
{
#if (HW_SPI == 1)
    uint8_t   write_buf[1],read_buf[1];
    hosal_qspi_dev_t* spi_epd = hosal_qspi_handle_get(0);

    spi_epd->rx_buf = &read_buf[0];
    spi_epd->tx_buf = &write_buf[0];

    write_buf[0] = data;
    hosal_qspi_transfer_pio(spi_epd, spi_epd->tx_buf,spi_epd->rx_buf,1,1000);
#else
    uint8_t TempData;
    uint8_t scnt;
    TempData = data;
    delay_us(1);
    EPD_CLK_PIN_LOW;
    for (scnt = 0; scnt < 8; scnt++)
    {
        if (TempData & 0x80)
        {
            EPD_SDA_PIN_HIGH;
        }
        else
        {
            EPD_SDA_PIN_LOW;
        }
        delay_us(1);
        EPD_CLK_PIN_HIGH;
        delay_us(1);
        EPD_CLK_PIN_LOW;
        TempData = TempData << 1;
    }
    delay_us(1);
#endif
}

void epd_write_command(uint8_t cmd)
{
#if (HW_SPI == 1)
    EPD_DC_PIN_LOW; // D/C#   0:command  1:data
    epd_spi_writebyte(cmd);
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
#else
    EPD_CS_PIN_HIGH;
    EPD_CS_PIN_LOW;
    EPD_DC_PIN_LOW; // D/C#   0:command  1:data
    epd_spi_writebyte(cmd);
    EPD_CS_PIN_HIGH;
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
#endif
    delay_us(1);
}

void epd_write_data(uint8_t data)
{
#if (HW_SPI == 1)
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
    epd_spi_writebyte(data);
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
#else
    EPD_CS_PIN_HIGH;
    EPD_CS_PIN_LOW;
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
    epd_spi_writebyte(data);
    EPD_CS_PIN_HIGH;
    EPD_DC_PIN_HIGH; // D/C#   0:command  1:data
#endif
    delay_us(1);
}

void epd_read_busy(void)
{
    uint32_t tmp;

    do {
        hosal_gpio_pin_get(EPD_BUSY_PIN, &tmp);
    } while (tmp == 0); // Wait until busy pin is high
    delay_us(100);
}

void epd_refresh(void)
{											
	epd_write_command(0x17);		
	epd_write_data(0xA5);
	epd_read_busy();							
}

void epd_sleep(void)
{
	epd_write_command (0x07);	 	// Deep sleep (DSLP) 
	epd_write_data(0xA5);
}

void epd_display(EPD_LOAD_MODE select, uint8_t *img_data, uint32_t num)
{
    if (select < LOAD_IMAGE_BW)
    {
        /*sent black and white data after 0x10 command*/
        epd_write_command(0x10);    //BW datas
        for(uint32_t i = 0; i < ((SOURCE_PIXEL*GATE_PIXEL) >> 3); i++)
        {            
            switch (select)
            {
            case MANUAL_BLACK:
                epd_write_data(0x00);
                break;
                
            case MANUAL_WHITE:
                epd_write_data(0xFF);
                break;
                
            case MANUAL_RED:
                epd_write_data(0x00);
                break;

            default:
                break;
            }
        } 
        /*sent Red and white data after 0x13 command*/
        epd_write_command(0x13);    //new or Red data 
        for(uint32_t i = 0; i < ((SOURCE_PIXEL*GATE_PIXEL) >> 3); i++)
        {        
            switch (select)
            {
            case MANUAL_BLACK:
                epd_write_data(0x00);
                break;
                
            case MANUAL_WHITE:
                epd_write_data(0x00);
                break;
                
            case MANUAL_RED:
                epd_write_data(0xFF);
                break;

            default:
                break;
            }		 
        }
    }
    else if (select == LOAD_IMAGE_BW)
    {
        /*sent black and white data after 0x10 command*/
        epd_write_command(0x10);    //BW datas
        for(uint32_t i = 0; i < num; i++)
        {
            epd_write_data(img_data[i]);//BW data
        } 
    }
    else if (select == LOAD_IMAGE_RED)
    {
        /*sent Red and white data after 0x13 command*/
        epd_write_command(0x13);    //new or Red data 
        for(uint32_t i = 0; i < num; i++)
        {
            epd_write_data(img_data[i]);//RED data
        }
    }
}

void epd_reset_and_init(void)
{
	EPD_RESET_LOW;		// Module reset
	delay_ms(100);      // At least 10ms delay 
	EPD_RESET_HIGH;
	delay_ms(100);	    // At least 10ms delay 

	epd_read_busy();
}

void epd_io_init(void)
{
    hosal_gpio_input_config_t pin_cfg;

#if (HW_SPI == 1)
    hosal_qspi_status_t       err =   HOSAL_QSPI_SUCCESS;
    hosal_qspi_baudrate_t     baud =  HOSAL_QSPI_BAUDRATE_1M;
    hosal_qspi_mode_t         mode =  HOSAL_QSPI_MASTER_MODE;
    hosal_qspi_bitorder_t     order = HOSAL_QSPI_MSB;
    hosal_qspi_bitsize_t      size =  HOSAL_QSPI_DATASIZE_8;
    hosal_qspi_phase_t        phase = HOSAL_QSPI_PHASE_1EDGE;
    hosal_qspi_polarity_t     pol =   HOSAL_QSPI_POLARITY_LOW;
    hosal_qspi_slave_select_t ss =    HOSAL_QSPI_SELECT_SLAVE_0;
    hosal_qspi_cs_polarity_t  spol =  HOSAL_QSPI_CHIPSEL_ACTIVE_LOW;

    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BAUD_SET,            &baud);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_MODE_SET,            &mode);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_DATAWIDTH_SET,       &size);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_BITORDER_SET,        &order);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_PHASE_SET,           &phase);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_POLARITY_SET,        &pol);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVESELECT_SET,     &ss);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_SLAVE_POLARTITY_SET, &spol);

    hosal_qspi_init(&qspi_dev0);
    NVIC_DisableIRQ(qspi_dev0.irq_num);
#else
    hosal_gpio_cfg_output(EPD_SCK_PIN);
    hosal_gpio_pin_clear(EPD_SCK_PIN);

    hosal_gpio_cfg_output(EPD_SDA_PIN);
    hosal_gpio_pin_clear(EPD_SDA_PIN);

    hosal_pin_set_mode(EPD_CS_PIN, HOSAL_MODE_GPIO);
    hosal_gpio_cfg_output(EPD_CS_PIN);
    hosal_gpio_pin_set(EPD_CS_PIN);
#endif
    //soft-dc
    hosal_gpio_cfg_output(EPD_DC_PIN);
    hosal_gpio_pin_clear(EPD_DC_PIN);

    // init reset pin
    hosal_gpio_cfg_output(EPD_RESET_PIN);
    hosal_gpio_pin_set(EPD_RESET_PIN);

    // init busy pin
    pin_cfg.param = NULL;
    pin_cfg.pin_int_mode = HOSAL_GPIO_PIN_NOINT;
    pin_cfg.usr_cb = NULL;
    hosal_pin_set_pullopt(EPD_BUSY_PIN, HOSAL_PULL_UP_100K);
    hosal_gpio_cfg_input(EPD_BUSY_PIN, pin_cfg); // Configure EPD_BUSY_PIN as input
}
