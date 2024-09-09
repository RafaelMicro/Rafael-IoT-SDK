#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "mcu.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hosal_qspi.h"
#include "cli.h"
#include "log.h"

#define FLASH_SECTOR_SIZE 256

volatile bool dma_finish_state = false;

void read_id(void);
void write_enable(void);
void erase_sector(uint32_t address);
void get_flash_status0(uint8_t *status);
void get_flash_status1(uint8_t *status);
void program_sector(uint32_t flash_address, uint8_t *data_buf, int mode);
void read_sector(uint32_t flash_address, uint8_t *data_buf, int mode);
void write_status(uint8_t status0, uint8_t status1);
void wait_wip_finish(void);
void program_sector_dma(uint32_t flash_address, uint8_t *data_buf, int mode, uint16_t length);
void read_sector_dma(uint32_t flash_address, uint8_t *data_buf, int mode, uint16_t length);
void pattern_init(void);

HOSAL_QSPI_DEV_DECL(qspi_dev0, 
    QSPI0, QSPI0_SCLK, QSPI0_CS0, QSPI0_DATA0, QSPI0_DATA1, QSPI0_DATA2, QSPI0_DATA3, 
    HOSAL_QSPI_BAUDRATE_32M, HOSAL_QSPI_MASTER_MODE);

uint8_t write_buf[FLASH_SECTOR_SIZE];
uint8_t read_buf[FLASH_SECTOR_SIZE];
uint8_t write_buf2[FLASH_SECTOR_SIZE];
uint8_t read_buf2[FLASH_SECTOR_SIZE];

void qspi0_dma_finish_cb(void *arg)
{
    hosal_qspi_dev_t *ptr = arg;
    memcpy(&qspi_dev0, ptr, sizeof(hosal_qspi_dev_t));
    dma_finish_state = true;
}

int main(void) {
    uint32_t i = 0, temp = 0;
    uint8_t status0 = 0, status1 = 0;

    hosal_qspi_phase_t cpha = HOSAL_QSPI_PHASE_2EDGE;
    hosal_qspi_polarity_t cpol = HOSAL_QSPI_POLARITY_HIGH;

    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_PHASE_SET, &cpha);
    hosal_qspi_ioctl(&qspi_dev0, HOSAL_QSPI_POLARITY_SET, &cpol);

    hosal_qspi_callback_register(&qspi_dev0, HOSAL_QSPI_TRANSFER_DMA, qspi0_dma_finish_cb, &qspi_dev0);
    hosal_qspi_init(&qspi_dev0);
    NVIC_SetPriority(qspi_dev0.irq_num, 0);
    NVIC_EnableIRQ(qspi_dev0.irq_num);

    printf("Hello, QSPI TEST\r\n");
    printf("This QSPI flash is an external flash outside RT58X\r\n");
    printf("GPIO14 is QSPI DATA2\r\n");
    printf("GPIO15 is QSPI DATA3\r\n");
    printf("GPIO6 is QSPI SCLK\r\n");
    printf("GPIO7 is QSPI CSN0\r\n");
    printf("GPIO8 is QSPI DATA0\r\n");
    printf("GPIO9 is QSPI DATA1\r\n");
    printf("Please read flash document for more instructions command\r\n\r\n");

    printf("Get flash id: ");
    read_id();

    get_flash_status0(&status0);
    get_flash_status1(&status1);        /*Check QE bit*/

    /*before Quad program, you must check QE bit in status bit is 1*/
    if ((status1 & 0x2) == 0) {
        /*QE is 0, we should set it*/
        status1 |= 0x2;      /*set QE bit*/
        write_status(status0, status1);
    }
    erase_sector(0);

    pattern_init();

    printf("PIO write/read QSPI by NORMAL mode.\r\n");
    program_sector(0x600, write_buf2, QSPI_NORMAL_SPI);
    /*program data in CPU polling mode. default read 256 bytes.*/
    read_sector(0x600, read_buf2, QSPI_NORMAL_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("PIO write/read QSPI by QAUD mode.\r\n");
    /*program data in CPU polling mode. default write 256 bytes. in 4 bits write*/
    program_sector(0x500, write_buf2, QSPI_QUAD_SPI);
    /*program data in CPU polling mode. default read 256 bytes. in 4 bit read*/
    read_sector(0x500, read_buf2, QSPI_QUAD_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("PIO write/read QSPI by QAUD mode.\r\n");
    /*program data in CPU polling mode. default write 256 bytes. in 4 bits write*/
    program_sector(0x400, write_buf2, QSPI_QUAD_SPI);
    /*program data in CPU polling mode. default read 256 bytes. in 4 bit read*/
    read_sector(0x400, read_buf2, QSPI_QUAD_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("PIO read QSPI by NORMAL mode.\r\n");
    read_sector(0x400, read_buf2, QSPI_NORMAL_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("PIO read QSPI by DUAL mode.\r\n");
    read_sector(0x400, read_buf2, QSPI_DUAL_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("PIO read QSPI by QUAD mode.\r\n");
    read_sector(0x400, read_buf2, QSPI_QUAD_SPI);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf2[i] != read_buf2[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf2, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    /*DMA write, normal SPI mode*/
    /*
     *   Please notice: QFLASH each sector is 256 bytes. so you can not program
     * a sector more than 256 bytes. If your data is more than 256 bytes,
     * you should separate the data to several write.
     */
    printf("DMA write/read QSPI by NORMAL mode.\r\n");
    program_sector_dma(0x300, write_buf, QSPI_NORMAL_SPI, FLASH_SECTOR_SIZE);
    /*DMA read, normal SPI mode*/
    read_sector_dma(0x300, read_buf, QSPI_NORMAL_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("DMA write/read QSPI by DUAL mode.\r\n");
    program_sector_dma(0x300, write_buf, QSPI_DUAL_SPI, FLASH_SECTOR_SIZE);
    /*DMA read, normal SPI mode*/
    read_sector_dma(0x300, read_buf, QSPI_DUAL_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("DMA write/read QSPI by QUAD mode\r\n");
    /*4bit write dma*/
    program_sector_dma(0x100, write_buf, QSPI_QUAD_SPI, FLASH_SECTOR_SIZE);
    /*DMA read, quad SPI mode*/
    read_sector_dma(0x100, read_buf, QSPI_QUAD_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("DMA read QSPI by NORMAL mode\r\n");
    read_sector_dma(0x100, read_buf, QSPI_NORMAL_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("DMA read QSPI by DUAL mode\r\n");
    read_sector_dma(0x100, read_buf, QSPI_DUAL_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("DMA read QSPI by QUAD mode\r\n");
    read_sector_dma(0x100, read_buf, QSPI_QUAD_SPI, FLASH_SECTOR_SIZE);
    /*compare data*/
    for (i = 0; i < FLASH_SECTOR_SIZE; i++) {
        if (write_buf[i] != read_buf[i]) {
            printf("error i:%ld %02x %02x\r\n", i, write_buf[i], read_buf[i] );
            while (1);
        }
    }
    memset(read_buf, 0, FLASH_SECTOR_SIZE);
    printf("data match\r\n");

    printf("QSPI Validation Success\r\n");
    while (1) {}
    return 0;
}

void read_id(void)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4], rxbuf[3];
    uint32_t val;

    command[0] = 0x9F;

    req.cmd_buf = command;
    req.cmd_length = 1;

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = rxbuf; 
    req.read_length = 3;

    hosal_qspi_flash_command(&qspi_dev0, &req);

    printf("%02x%02x%02x\r\n", 
        req.read_buf[0], 
        req.read_buf[1], 
        req.read_buf[2]);  
}

void write_enable(void)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4];

    command[0] = 0x06;

    req.cmd_buf = command;
    req.cmd_length = 1;

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = NULL; 
    req.read_length = 0;
    req.transfer_mode = QSPI_NORMAL_SPI;

    hosal_qspi_flash_command(&qspi_dev0, &req);
}

void sr_write_enable(void)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4];

    command[0] = 0x50;

    req.cmd_buf = command;
    req.cmd_length = 1;

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = NULL; 
    req.read_length = 0;
    req.transfer_mode = QSPI_NORMAL_SPI;

    hosal_qspi_flash_command(&qspi_dev0, &req);
}

void erase_sector(uint32_t address)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4];

    write_enable();

    command[0] = 0x20;              /*sector erase*/
    command[1] = (address >> 16) & 0xFF;
    command[2] = (address >> 8) & 0xFF;
    command[3] = 0x00;              /*page must be 4K alignment, so it must be zero*/

    req.cmd_buf = command;
    req.cmd_length = 4;

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = NULL; 
    req.read_length = 0;
    req.transfer_mode = QSPI_NORMAL_SPI;

    hosal_qspi_flash_command(&qspi_dev0, &req);

    wait_wip_finish();
}

void get_flash_status0(uint8_t *status)
{
    hosal_qspi_status_t err = HOSAL_QSPI_SUCCESS;
    hosal_qspi_flash_command_t req;
    uint8_t command[4], read_buf[4];

    command[0] = 0x05;              /*winbond flash can not use 0x05 to get S1 ?*/

    req.cmd_buf = command;
    req.cmd_length = 1;             /*only 1 bytes command*/

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = read_buf;
    req.read_length = 1;
    req.transfer_mode = QSPI_NORMAL_SPI;

    err = hosal_qspi_flash_command(&qspi_dev0, &req);

    *status = read_buf[0];
}

void get_flash_status1(uint8_t *status)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4], read_buf[4];

    command[0] = 0x35;              /*winbond flash can not use 0x05 to get S1 ?*/

    req.cmd_buf = command;
    req.cmd_length = 1;             /*only 1 bytes command*/ 

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = read_buf;
    req.read_length = 1;
    req.transfer_mode = QSPI_NORMAL_SPI;

    hosal_qspi_flash_command(&qspi_dev0, &req);

    *status = read_buf[0];
}

void wait_wip_finish(void)
{
    uint8_t status = 0;

    while (1) {
        get_flash_status0(&status);

        if ((status & 1) == 0) {
            break;    /*WIP is 0 */
        }
    }
}

void program_sector(uint32_t flash_address, uint8_t *data_buf, int mode)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4], read_buf[4];

    write_enable();

    /*program page --- in QFlash, each page is 256 bytes!!*/

    /*Here, we will assume, each time write is write page size
      and the flash address is page-size alignment,*/
    switch (mode) {
    case QSPI_QUAD_SPI:
        command[0] = 0x32;              /*Quad read */
        req.transfer_mode = QSPI_QUAD_SPI;    /*QUAD mode*/
        break;
    case QSPI_NORMAL_SPI:
    case QSPI_DUAL_SPI:
    /* Notice: Flash does NOT support dual write command,
     * So we change this dual to single mode.
     */
    default:    /*use one bit */
        command[0] = 0x02;              /*default single read*/
        req.transfer_mode = QSPI_NORMAL_SPI;
        break;
    }

    command[1] = (flash_address >> 16) & 0xFF;     /*Here we assume data address in 256 bytes alignment*/
    command[2] = (flash_address >> 8) & 0xFF;      /*so flash_address last 8 bytes is zero!*/
    command[3] = 0x00;

    req.cmd_buf = command;
    req.cmd_length = 4;        /*command is 4 bytes command*/
    req.write_buf = data_buf;
    req.write_length = FLASH_SECTOR_SIZE;
    req.read_buf = NULL;
    req.read_length = 0;

    hosal_qspi_flash_command(&qspi_dev0, &req);

    /*Here we use busy polling*/
    wait_wip_finish();
}

void read_sector(uint32_t flash_address, uint8_t *data_buf, int mode)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[5], read_buf[4];

    switch (mode) {
    case QSPI_NORMAL_SPI:
        command[0] = 0x0B;              /*single read */
        req.transfer_mode = QSPI_NORMAL_SPI;    /*single one bit mode*/
        break;
    case QSPI_DUAL_SPI:
        command[0] = 0x3B;              /*Dual real */
        req.transfer_mode = QSPI_DUAL_SPI;    /*dual mode*/
        break;
    case QSPI_QUAD_SPI:
        command[0] = 0x6B;              /*Quad read */
        req.transfer_mode = QSPI_QUAD_SPI;    /*QUAD mode*/
        break;
    default:    /*use one bit */
        command[0] = 0x0B;              /*default single read*/
        req.transfer_mode = QSPI_NORMAL_SPI;
        break;
    }

    /*Here we assume data is page size alignment.*/

    command[1] = (flash_address >> 16) & 0xFF;
    command[2] = (flash_address >> 8) & 0xFF;
    command[3] = 0x00;
    command[4] = 0x00;      /*this byte is dummy byte*/

    req.cmd_buf = command;
    req.cmd_length = 5;        /*command is 5 bytes command*/

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = data_buf;
    req.read_length = FLASH_SECTOR_SIZE;

    hosal_qspi_flash_command(&qspi_dev0, &req);
}

void write_status(uint8_t status0, uint8_t status1)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[4];

    write_enable();
    // sr_write_enable();
    
    /*write status*/
    command[0] = 0x01;
    command[1] = status0;
    command[2] = status1;

    req.cmd_buf = command;
    req.cmd_length = 3;        /*3 bytes command*/
    req.transfer_mode = QSPI_NORMAL_SPI;

    req.write_buf = NULL;
    req.write_length = 0;

    req.read_buf = NULL;
    req.read_length = 0;

    hosal_qspi_flash_command(&qspi_dev0, &req);

    wait_wip_finish();
}

void program_sector_dma(uint32_t flash_address, uint8_t *data_buf, int mode, uint16_t length)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[8];

    write_enable();

    /*program page using dma*/
    /* TODO:  we don't check length here. but the length should be <= page size (default page size is 256)
     *
     */

    /*Here, we will assume, each time write is write page size
      and the flash address is page-size alignment,*/
    switch (mode) {
    case QSPI_QUAD_SPI:
        command[0] = 0x32;              /*Quad read */
        req.transfer_mode = QSPI_QUAD_SPI;    /*QUAD mode*/
        break;
    case QSPI_NORMAL_SPI:
    case QSPI_DUAL_SPI:
    /* Notice: Flash does NOT support dual write command,
     * So we change this dual to single mode.
     */
    default:    /*use one bit */
        command[0] = 0x02;              /*default single read*/
        req.transfer_mode = QSPI_NORMAL_SPI;
        break;
    }

    command[1] = (flash_address >> 16) & 0xFF;
    command[2] = (flash_address >> 8) & 0xFF;
    command[3] = 0x00;

    req.cmd_buf = command;
    req.cmd_length = 4;        /*command is 4 bytes command*/

    req.write_buf = data_buf;
    req.write_length = length;

    /*qspi_write_dma don't care read_buf.. so it can ignore read_buf and read_length*/
    req.read_buf = NULL;
    req.read_length = 0;

    /*this is just an example to show we should wait dma finish...*/
    dma_finish_state = false;
    /*
     * the following function is using dma to send data to spi device.
     * the function is asynchronous function. So we need a callback function
     * to notify the job finish.
     */
    hosal_qspi_write_dma(&qspi_dev0, &req);

    while (dma_finish_state == false) {}

    /*Here we use busy polling*/
    wait_wip_finish();
}

void read_sector_dma(uint32_t flash_address, uint8_t *data_buf, int mode, uint16_t length)
{
    hosal_qspi_flash_command_t req;
    uint8_t command[8];

    /*QE bit already enable*/
    switch (mode) {
    case QSPI_NORMAL_SPI:
        command[0] = 0x0B;                      /*single read */
        req.transfer_mode = QSPI_NORMAL_SPI;    /*single one bit mode*/
        break;

    case QSPI_DUAL_SPI:
        command[0] = 0x3B;                      /*Dual real */
        req.transfer_mode = QSPI_DUAL_SPI;      /*DUAL mode*/
        break;

    case QSPI_QUAD_SPI:
        command[0] = 0x6B;                      /*Quad read */
        req.transfer_mode = QSPI_QUAD_SPI;      /*QUAD mode*/
        break;
    default:    /*use one bit */
        command[0] = 0x0B;                      /*default single read*/
        req.transfer_mode = QSPI_NORMAL_SPI;
        break;
    }

    command[1] = (flash_address >> 16) & 0xFF;
    command[2] = (flash_address >> 8) & 0xFF;
    command[3] = 0x00;
    command[4] = 0x00;      /*dummy clocks*/

    req.cmd_buf = command;
    req.cmd_length = 5;        /*5 bytes command*/

    /* If we want to use dma to read, data size must be 16*N */
    req.read_buf = data_buf;
    req.read_length = length;

    /*qspi_read_dma will ignore write_buf and write_length.*/
    req.write_buf = NULL;
    req.write_length = 0;

    dma_finish_state = false;

    /*
     * the following function is using dma to send data to spi device.
     * the function is asynchronous function. So we need a callback function
     * to notify the job finish.
     */
    hosal_qspi_read_dma(&qspi_dev0, &req);

    while (dma_finish_state == false) {}
}

void pattern_init(void)
{
    uint32_t i = 0, temp = 0;

    write_buf[0] = 0x2D;
    write_buf[1] = 0xAB;

    write_buf2[0] = 0xDD;
    write_buf2[1] = 0x7F;

    for (i = 2; i < FLASH_SECTOR_SIZE; i++) {
        temp = (write_buf[i - 2] * 17 + write_buf[i - 1] * 53 + 173);
        write_buf[i] = temp % 251;

        temp = (write_buf2[i - 2] * 17 + write_buf2[i - 1] * 53 + 173);
        write_buf2[i] = temp % 251;
    }    
}