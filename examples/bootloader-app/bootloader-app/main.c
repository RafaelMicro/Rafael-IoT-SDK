#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "fota_define.h"
#include "hosal_uart.h"
#include "mcu.h"
#if defined(CONFIG_BOOTLOADER_DEBUG)
#include "uart_stdio.h"
#endif

#include "LzmaDec.h"
#include "flashctl.h"
#include "rt_crypto.h"
#include "rt_ecc.h"
#include "rt_ecjpake.h"
#include "rt_sha256.h"

#include "version_api.h"

#include "bootloader.h"
#include "slip.h"

#define SECURE_BOOT_INFO_PAGE       0x1000
#define PK_MAGIC_PATTERN            0x72A34B50
#define SIG_MAGIC_PATTERN           0x58BF4E53
#define FLASH_PAGE_PROGRAMMING_SIZE 256
#define SIZE_OF_FLASH_SECTOR_ERASE  4096
#define FLASH_PAGE_MASK             (~(256 - 1))

#define MULTIPLE_OF_32K(Add) ((((Add) & (0x8000 - 1)) == 0) ? 1 : 0)
#define MULTIPLE_OF_64K(Add) ((((Add) & (0x10000 - 1)) == 0) ? 1 : 0)

#define SECURE_VERIFY_PASS 1
#define SECURE_VERIFY_FAIL 0

#define SECURE_BOOT_SUPPORTED   1
#define SECURE_BOOT_NOT_SUPPORT 0

#define CHECK_FOTA_IMAGE_EXIST        1
#define DO_NOT_CHECK_FOTA_IMAGE_EXIST 0

#define IN_BUF_SIZE  (1 << 8)
#define OUT_BUF_SIZE (1 << 9)

uint32_t readBuff[FLASH_PAGE_PROGRAMMING_SIZE >> 2];
uint32_t sec_page_buf[FLASH_PAGE_PROGRAMMING_SIZE >> 2];
uint32_t sig_buf[FLASH_PAGE_PROGRAMMING_SIZE >> 2];
uint32_t check_verify_size;
uint32_t fota_max_size;
uint32_t flash_model_info;
uint32_t fota_signature_address;
uint32_t app_max_size;
uint32_t app_signature_address;

extern sha256_starts_rom_t sha256_init;
extern sha256_update_rom_t sha256_update;
extern sha256_finish_rom_t sha256_finish;

sha256_context sha_cnxt;
uint8_t sha256_digest[32];
uint8_t le_sha256_digest[32];

extern void lib_version_init(void);

typedef void(FUNC_PTR)(void);

static void jump_to_application(uint32_t AppAddress);

static void jump_to_application(uint32_t AppAddress) {
    FUNC_PTR* AppStart;

    if (*(uint32_t*)AppAddress == 0xFFFFFFFF) {
        return;
    }

    SCB->VTOR = (uint32_t)AppAddress;
    __set_MSP(*(uint32_t*)AppAddress);
    AppStart = (FUNC_PTR*)(*(uint32_t*)(AppAddress + 4));
    AppStart();
}

void set_flash_erase(uint32_t flash_addr, uint32_t image_size) {
    uint32_t ErasedSize = 0;

    while (image_size > ErasedSize) {
        if (((image_size - ErasedSize) > 0x10000)
            && (MULTIPLE_OF_64K(flash_addr + ErasedSize))) {
            flash_erase(FLASH_ERASE_64K, flash_addr + ErasedSize);
            ErasedSize += 0x10000;
        } else if (((image_size - ErasedSize) > 0x8000)
                   && (MULTIPLE_OF_32K(flash_addr + ErasedSize))) {
            flash_erase(FLASH_ERASE_32K, flash_addr + ErasedSize);
            ErasedSize += 0x8000;
        } else {
            flash_erase(FLASH_ERASE_SECTOR, flash_addr + ErasedSize);
            ErasedSize += SIZE_OF_FLASH_SECTOR_ERASE;
        }
        while (flash_check_busy())
            ;
    }
}

static int load_image_into_flash(uint32_t image_base, uint32_t flash_addr,
                                 uint32_t image_size) {
    uint8_t Read;
    uint32_t i, j, u32RemainSize = 0;

    /*Here we default read page is 256 bytes.*/
    flash_set_read_pagesize();

    set_flash_erase(flash_addr, image_size);

    /*programming new image by page read/write*/
    for (i = 0; i < image_size; i += FLASH_PAGE_PROGRAMMING_SIZE) {
        flash_read_page((uint32_t)readBuff, image_base + i);
        while (flash_check_busy())
            ;

        flash_write_page((uint32_t)readBuff, (uint32_t)(flash_addr + i));
        while (flash_check_busy())
            ;
    }

    /* if image size not align with page erase size*/
    if (image_size != i) {
        u32RemainSize = i - image_size;

        /*programming new image by single byte read/write*/
        for (j = 0; j < u32RemainSize; j += 1) //write 1 bytes
        {
            Read = flash_read_byte(image_base + (i << 8) + j);
            flash_write_byte(flash_addr + (i << 8) + j, Read);
            while (flash_check_busy())
                ;
        }
    }

    return 0;
}

uint32_t crc32(uint32_t flash_addr, uint32_t data_len) {
    uint8_t RemainLen = (data_len & (0x3));
    uint32_t i;
    uint16_t j, k;
    uint32_t ChkSum = ~0;
    uint32_t Len = (data_len >> 2), Read;
    uint32_t* FlashPtr = (uint32_t*)flash_addr;

    for (i = 0; i < Len; i++) {
        //get 32 bits at one time
        Read = FlashPtr[i];
        //get the CRC of 32 bits
        for (j = 0; j < 32; j += 8) {
            //get the CRC of 8 bits
            ChkSum ^= ((Read >> j) & 0xFF);
            for (k = 0; k < 8; k++) {
                ChkSum = (ChkSum & 1) ? (ChkSum >> 1) ^ 0xedb88320
                                      : ChkSum >> 1;
            }
        }
    }

    /*if data_len not align 4 bytes*/
    if (RemainLen > 0) {
        Read = FlashPtr[i];

        //get the CRC of 32 bits
        for (j = 0; j < (RemainLen << 3); j += 8) {
            //get the CRC of 8 bits
            ChkSum ^= ((Read >> j) & 0xFF);
            for (k = 0; k < 8; k++) {
                ChkSum = (ChkSum & 1) ? (ChkSum >> 1) ^ 0xedb88320
                                      : ChkSum >> 1;
            }
        }
    }
    ChkSum = ~ChkSum;
    return ChkSum;
}

void* MyAlloc(size_t size) {
    void* tset;

    if (size == 0) {
        return NULL;
    }

    tset = malloc(size);

    return tset;
}

void MyFree(void* address) { free(address); }

static void* SzAlloc(ISzAllocPtr p, size_t size) { return MyAlloc(size); }

static void SzFree(ISzAllocPtr p, void* address) { MyFree(address); }

const ISzAlloc g_Alloc = {SzAlloc, SzFree};

static SRes Decode2(CLzmaDec* state, uint32_t OutAddress, uint32_t InAddress,
                    uint32_t unpackSize) {
    int thereIsSize = (unpackSize != (uint32_t)(int32_t)-1);
    static Byte inBuf[IN_BUF_SIZE];
    Byte outBuf[OUT_BUF_SIZE], strBuf[IN_BUF_SIZE];
    size_t inPos = 0, inSize = 0, outPos = 0;
    LzmaDec_Init(state);
    uint32_t processed = 0, processed_out = 0;
    uint32_t flash_size = 256;
    uint32_t write_pos = 0, write_pos_tmp = 0, write_diff = 0;

    for (;;) {
        if (inPos == inSize) {
            inSize = IN_BUF_SIZE;
            inPos = 0;
            DEBUG_PRINT("\r\ninSize = %d, inPos = %d, process = %d\r\n", inSize,
                        inPos, processed);

            memcpy(inBuf, (void*)(InAddress + processed), inSize);
        }
        //DEBUG_PRINT("\r\n\r\n\r\n\r\ninSize = %d, inPos = %d, process = %d\r\n",inSize, inPos, processed);
        {
            SRes res;
            SizeT inProcessed = inSize - inPos;
            SizeT outProcessed = OUT_BUF_SIZE - outPos;
            ELzmaFinishMode finishMode = LZMA_FINISH_ANY;
            ELzmaStatus status;

            if (thereIsSize && outProcessed > unpackSize) {
                outProcessed = (SizeT)unpackSize;
                finishMode = LZMA_FINISH_END;
            }

            DEBUG_PRINT("B inProcessed = %d, outProcessed = %d, inPos = %d, "
                        "processed = %d, processed_out = %d\r\n",
                        inProcessed, outProcessed, inPos, processed,
                        processed_out);

            res = LzmaDec_DecodeToBuf(state, (outBuf), &outProcessed,
                                      (Byte*)(inBuf + inPos), &inProcessed,
                                      finishMode, &status);

            inPos += inProcessed;
            outPos += outProcessed;
            unpackSize -= outProcessed;
            processed += inProcessed;
            processed_out += outProcessed;

            DEBUG_PRINT("A inProcessed = %d, outProcessed = %d, inPos = %d, "
                        "processed = %d, processed_out = %d\r\n",
                        inProcessed, outProcessed, inPos, processed,
                        processed_out);

            write_pos_tmp += outProcessed;

            if (write_pos_tmp > (flash_size - 1)) {
                write_diff = write_pos_tmp - flash_size;
                memcpy(&strBuf[write_pos], outBuf, (outProcessed - write_diff));
                DEBUG_PRINT(
                    "write_pos = %d, (outProcessed-write_diff) = %d\r\n",
                    write_pos, (outProcessed - write_diff));
                flash_write_page((uint32_t)strBuf, OutAddress);
                while (flash_check_busy())
                    ;
                OutAddress += flash_size;
                memset(strBuf, 0x00, sizeof(strBuf));
                write_pos = write_diff;
                write_pos_tmp = write_pos;
                DEBUG_PRINT(
                    "write_pos = %d, write_pos_tmp = %d, write_diff = %d, "
                    "OutAddress = %x\r\n",
                    write_pos, write_pos_tmp, write_diff, OutAddress);

                if (write_pos_tmp > (flash_size - 1)) {
                    memcpy(strBuf, &outBuf[(outProcessed - write_diff)],
                           flash_size);
                    write_diff = write_pos_tmp - flash_size;
                    flash_write_page((uint32_t)strBuf, OutAddress);
                    while (flash_check_busy())
                        ;
                    OutAddress += flash_size;
                    memset(strBuf, 0x00, sizeof(strBuf));
                    write_pos = write_diff;
                    write_pos_tmp = write_pos;
                    DEBUG_PRINT(
                        "2 write_pos = %d, write_pos_tmp = %d, write_diff = "
                        "%d, OutAddress = %x\r\n",
                        write_pos, write_pos_tmp, write_diff, OutAddress);
                }
                memcpy(strBuf, &outBuf[(outProcessed - write_diff)],
                       write_diff);
                DEBUG_PRINT(
                    "3 write_pos = %d, (outProcessed-write_diff) = %d\r\n",
                    write_pos, (outProcessed - write_diff));
            } else {
                memcpy(&strBuf[write_pos], outBuf, outProcessed);
                write_pos = write_pos_tmp;
            }

            if (unpackSize == 0 && write_pos != 0) {
                flash_write_page((uint32_t)strBuf, OutAddress);
                while (flash_check_busy())
                    ;
                DEBUG_PRINT(
                    "end write_pos = %d, write_pos_tmp = %d, write_diff = "
                    "%d, OutAddress = %x\r\n",
                    write_pos, write_pos_tmp, write_diff, OutAddress);
            }

            DEBUG_PRINT("\r\n");
            memset(outBuf, 0x00, sizeof(outBuf));

            outPos = 0;

            if (res != SZ_OK || (thereIsSize && unpackSize == 0)) {
                return res;
            }

            if (inProcessed == 0 && outProcessed == 0) {
                if (thereIsSize || status != LZMA_STATUS_FINISHED_WITH_MARK) {
                    return SZ_ERROR_DATA;
                }
                return res;
            }
        }
    }
}

static SRes Decode(uint32_t OutAddress, uint32_t InAddress) {
    uint32_t unpackSize;
    int i;
    SRes res = 0;

    CLzmaDec state;

    /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
    unsigned char header[LZMA_PROPS_SIZE + 8];

    /* Read and parse header */

    DEBUG_PRINT("\r\n\r\n\r\n[FOTA] Decode\r\n");

    unpackSize = 0;
    for (i = 0; i < sizeof(header); i++) {
        header[i] = flash_read_byte(InAddress + i);
        if ((i > 0) && (i < 5)) {
            DEBUG_PRINT("header[%d] = %x\r\n", i, header[i]);
        }
    }

    for (i = 0; i < 8; i++) {
        unpackSize += (uint32_t)header[LZMA_PROPS_SIZE + i] << (i * 8);
    }

    DEBUG_PRINT("unpackSize = %x\r\n", unpackSize);
    LzmaDec_Construct(&state);
    LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &g_Alloc);

    res = Decode2(&state, OutAddress, (InAddress + sizeof(header)), unpackSize);
    LzmaDec_Free(&state, &g_Alloc);
    return res;
}

uint8_t checksum(uint8_t* buf) {
    uint8_t cksum = 0;
    uint16_t len = 0;

    len = (uint16_t)(buf[1] | buf[2] << 8);
    for (uint16_t i = 0; i < len - 1; i++) {
        cksum += buf[i];
    }
    return cksum;
}

bool check_command(uint8_t* buf) {
    uint16_t len;
    uint8_t cksum;

    len = (uint16_t)(buf[1] | buf[2] << 8);
    cksum = checksum(buf);

    if (cksum != buf[len - 1]) {
        return false;
    }
    return true;
}

int main(void) {
    uint32_t version_info;
    uint8_t secure_boot, fota_check, app_verify_result;
    Signature_P256 signature;
    ECPoint_P256 public_key;
    SRes res = 0;

    char* ptr = NULL;
    fota_information_t* p_fota_info =
        (fota_information_t*)(FOTA_UPDATE_BANK_INFO_ADDRESS);

    /*check flash status*/
    flash_suspend_check();

    /*init software sha256 vector*/
    sha256_vector_init();

    lib_version_init();

    secure_boot = SECURE_BOOT_NOT_SUPPORT;
    fota_check = DO_NOT_CHECK_FOTA_IMAGE_EXIST;
    version_info = get_chip_version();

    // Banner for the bootloader
    extern uint8_t _heap_start;
    extern uint32_t _heap_size;
    DEBUG_PRINT("\r\n  ____              _   _                 _\r\n");
    DEBUG_PRINT(" |  _ \\            | | | |               | |\r\n");
    DEBUG_PRINT(" | |_) | ___   ___ | |_| | ___   __ _  __| | ___ _ __\r\n");
    DEBUG_PRINT(
        " |  _ < / _ \\ / _ \\| __| |/ _ \\ / _` |/ _` |/ _ \\ '__|\r\n");
    DEBUG_PRINT(" | |_) | (_) | (_) | |_| | (_) | (_| | (_| |  __/ |\r\n");
    DEBUG_PRINT(
        " |____/ \\___/ \\___/ \\__|_|\\___/ \\__,_|\\__,_|\\___|_|\r\n");
    DEBUG_PRINT("\r\n");
    DEBUG_PRINT("[BOOT] Bootloader Running...\r\n");

    DEBUG_PRINT("[BOOT] Version: %d.%d.%d\r\n", (version_info >> 16) & 0xFF,
                (version_info >> 8) & 0xFF, version_info & 0xFF);
    DEBUG_PRINT("[BOOT] Heap %u@%p\r\n", (unsigned int)&_heap_size, &_heap_start);

    // Bootloader main loop
    bootloader_uart_init();
    bootloader_run_loop();

    // memset((void *)&_heap_start, 0, (void*)&_heap_size);

#if 1
    do
    {
        if ((version_info & IC_CHIP_REVISION_MASK) < IC_CHIP_REVISION_MPB)
        {
            fota_check = CHECK_FOTA_IMAGE_EXIST;
            DEBUG_PRINT("[BOOT] No Secure Boot.\n");
        }
        else
        {
            /*read secure page sector*/
            flash_read_sec_register((uint32_t)sec_page_buf, SECURE_BOOT_INFO_PAGE);
            while (flash_check_busy());

            if (sec_page_buf[0] == PK_MAGIC_PATTERN)
            {
                secure_boot = SECURE_BOOT_SUPPORTED;

                if (p_fota_info->fotabank_ready == FOTA_IMAGE_READY)
                {
                    ptr = (uint8_t *)p_fota_info->fotabank_startaddr;

                    if ( p_fota_info->fota_image_info & FOTA_IMAGE_INFO_COMPRESSED)
                    {
                        check_verify_size = p_fota_info->signature_len;
                    }
                    else
                    {
                        check_verify_size = *((uint32_t *) (ptr + 0x28));
                    }

                    if (check_verify_size == 0)
                    {
                        DEBUG_PRINT("[SECURE_BOOT] FOTA image validation size error.\n");
                        flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_VERIFY_SIZE_NOT_FOUND);
                        while (flash_check_busy());
                        break;
                    }
                    flash_model_info = flash_get_deviceinfo();
                    fota_max_size = ((1 << ((flash_model_info >> 16) & 0xFF)) - BOOTLOADER_SIZE - MP_SECTOR_SIZE - FLASH_PAGE_PROGRAMMING_SIZE) >> 1;

                    /*check image size can not over flash size*/
                    if (check_verify_size >= fota_max_size)
                    {
                        DEBUG_PRINT("[SECURE_BOOT] FOTA image overflow.\n");
                        flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_IMAGE_SIZE);
                        while (flash_check_busy());
                        break;
                    }

                    fota_signature_address =  ((uint32_t)ptr + check_verify_size + FLASH_PAGE_PROGRAMMING_SIZE - 1) & FLASH_PAGE_MASK;      /*page alignment*/
                    /*read the signature sector of APP image*/
                    flash_read_page((uint32_t)sig_buf, fota_signature_address);
                    while (flash_check_busy());

                    /*check signature pattern*/
                    if (sig_buf[0] != SIG_MAGIC_PATTERN)
                    {
                        DEBUG_PRINT("[SECURE_BOOT] FOTA image cannot find magic header of the signature.\n");
                        flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_SECURE_MAGIC_PATTERN_MISMATCH);
                        while (flash_check_busy());
                        break;
                    }

                    if (sig_buf[1] != check_verify_size)
                    {
                        DEBUG_PRINT("[SECURE_BOOT] Error, Signature length does not match!  %u %u \n", check_verify_size, sig_buf[1]);
                        flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_SECURE_VERIFY_SIZE_MISMATCH);
                        while (flash_check_busy());
                        break;
                    }

                    sha256_init(&sha_cnxt);
                    sha256_update(&sha_cnxt, ptr, check_verify_size);
                    sha256_finish(&sha_cnxt, sha256_digest);

                    /*SHA256 result is big-endian, so we need to change it to little-endian*/
                    buffer_endian_exchange((uint32_t *) le_sha256_digest,  (uint32_t *) sha256_digest, (32 >> 2));

                    memcpy (public_key.x, (uint8_t *) (sec_page_buf + 2), 32);          /*public key, this public key is little endian*/
                    memcpy (public_key.y, (uint8_t *) (sec_page_buf + 10), 32);

                    memcpy (signature.r, (uint8_t *) (sig_buf + 2), 32);             /*signature r,s*/
                    memcpy (signature.s, (uint8_t *) (sig_buf + 10), 32);

                    gfp_ecdsa_p256_verify_init();

                    if (gfp_ecdsa_p256_verify( &signature, (uint32_t *)le_sha256_digest, &public_key) == STATUS_SUCCESS)
                    {
                        /*ecdsa verification check ok.*/
                        DEBUG_PRINT("[SECURE_BOOT] FOTA image correct.\n");
                        fota_check = CHECK_FOTA_IMAGE_EXIST;
                        break;
                    }
                    else
                    {
                        DEBUG_PRINT("[SECURE_BOOT] FOTA image check failed.\n");
                        flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_IMAGE_ECDSA_VERIFY_FAIL);
                        while (flash_check_busy());
                        break;
                    }
                }
            }
            else
            {
                fota_check = CHECK_FOTA_IMAGE_EXIST;
                DEBUG_PRINT("[BOOT] No Secure Boot.\n");
                break;
            }
        }
    } while (0);
#endif 

#if 1
    do
    {
        if (fota_check != CHECK_FOTA_IMAGE_EXIST)
        {
            break;
        }
        if (p_fota_info->fotabank_ready != FOTA_IMAGE_READY) /*check firmware upgrade need to start*/
        {
            fota_check = DO_NOT_CHECK_FOTA_IMAGE_EXIST;
            break;
        }
        if (p_fota_info->fota_result != 0xFF)
        {
            fota_check = DO_NOT_CHECK_FOTA_IMAGE_EXIST;
            break;
        }
        //new firmware validation
        if (p_fota_info->fotabank_crc == crc32((uint32_t)p_fota_info->fotabank_startaddr, p_fota_info->fotabank_datalen))
        {
            if (p_fota_info->target_startaddr >= APP_START_ADDRESS)
            {
                DEBUG_PRINT("[FOTA] fotabank_startaddr %x, target_startaddr %x, data_len %d\n", p_fota_info->fotabank_startaddr, p_fota_info->target_startaddr, p_fota_info->fotabank_datalen);
                if ( p_fota_info->fota_image_info & FOTA_IMAGE_INFO_COMPRESSED )
                {
                    DEBUG_PRINT("[FOTA] FOTA_IMAGE_INFO_COMPRESSED.\n");
                    if ( flash_size() == FLASH_512K )
                    {
                        DEBUG_PRINT("[FOTA] 512.\n");
                        set_flash_erase(APP_START_ADDRESS, (FOTA_UPDATE_BUFFER_FW_ADDRESS_512K - APP_START_ADDRESS));
                    }
                    else if ( flash_size() == FLASH_1024K )
                    {
                        DEBUG_PRINT("[FOTA] 1024.\n");
                        set_flash_erase(APP_START_ADDRESS, (FOTA_UPDATE_BUFFER_FW_ADDRESS_1MB - APP_START_ADDRESS));
                    }
                    else if ( flash_size() == FLASH_2048K )
                    {
                        DEBUG_PRINT("[FOTA] 2048.\n");
                        set_flash_erase(APP_START_ADDRESS, (FOTA_UPDATE_BUFFER_FW_ADDRESS_2MB - APP_START_ADDRESS));
                    }
                    res = Decode(p_fota_info->target_startaddr, p_fota_info->fotabank_startaddr);
                }
                else
                {
                    //new firmware replacement
                    DEBUG_PRINT("[FOTA] NOT FOTA_IMAGE_INFO_COMPRESSED.\n");
                    load_image_into_flash(p_fota_info->fotabank_startaddr, p_fota_info->target_startaddr, p_fota_info->fotabank_datalen);
                }

                if (res == SZ_OK)
                {
                    flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_SUCCESS);
                    while (flash_check_busy());
                }
                else
                {
                    DEBUG_PRINT("[FOTA] decode err %d.\n", res);
                    flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_COMPRESS_DECODE_FAIL);
                    while (flash_check_busy());
                }

                DEBUG_PRINT("[FOTA] FOTA Updated.\n");
            }
            else
            {
                flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_TARGET_ADDR_IS_ILLEGAL);
                while (flash_check_busy());
                DEBUG_PRINT("[FOTA] FOTA target address is illegal.\n");
            }
        }
        else
        {
            flash_write_byte((uint32_t)&p_fota_info->fota_result, FOTA_RESULT_ERR_CHECK_IMAGE_CRC_FAIL);
            while (flash_check_busy());
            DEBUG_PRINT("[FOTA] FOTA check CRC failure.\n");
        }
    } while (0);
#endif

#if 1
    if (secure_boot == SECURE_BOOT_SUPPORTED)
    {
        app_verify_result = SECURE_VERIFY_FAIL;
        do
        {
            /*Here assume APP located  in flash +32K offset */
            ptr = (uint8_t *) APP_START_ADDRESS;
            /*Here assume APP image located in +0x8028 */
            check_verify_size = *((uint32_t *) (APP_START_ADDRESS + 0x28)) ;

            /*Here we don't limit image size */
            flash_model_info = flash_get_deviceinfo();
            app_max_size = ((1 << ((flash_model_info >> 16) & 0xFF)) - BOOTLOADER_SIZE - MP_SECTOR_SIZE - FLASH_PAGE_PROGRAMMING_SIZE) >> 1;

            /*check image size can not over flash size*/
            if ((check_verify_size == 0) || (check_verify_size >= app_max_size))
            {
                DEBUG_PRINT("[SECURE_BOOT] Error, APP has no size information %d %d\n", check_verify_size, app_max_size);
                break;
            }

            app_signature_address = (APP_START_ADDRESS + check_verify_size + FLASH_PAGE_PROGRAMMING_SIZE - 1) & FLASH_PAGE_MASK;      /*page alignment*/

            /*read the signature sector of APP image*/
            flash_read_page((uint32_t)sig_buf, app_signature_address);

            /*wait read one page finish.*/
            while (flash_check_busy());

            /*check signature pattern*/
            if (sig_buf[0] != SIG_MAGIC_PATTERN)
            {
                DEBUG_PRINT("[SECURE_BOOT] Error, can't find the magic header for the signature.\n");
                break;
            }

            /*check size*/
            if (sig_buf[1] != check_verify_size)
            {
                DEBUG_PRINT("[SECURE_BOOT] Error, Signature length does not match!  %u %u \n", check_verify_size, sig_buf[1]);
                break;
            }

            sha256_init(&sha_cnxt);
            sha256_update(&sha_cnxt, ptr, check_verify_size);
            sha256_finish(&sha_cnxt, sha256_digest);

            /*SHA256 result is big-endian, so we need to change it to little-endian*/
            buffer_endian_exchange((uint32_t *) le_sha256_digest,  (uint32_t *) sha256_digest, (32 >> 2));

            memcpy (public_key.x, (uint8_t *) (sec_page_buf + 2), 32);          /*public key, this public key is little endian*/
            memcpy (public_key.y, (uint8_t *) (sec_page_buf + 10), 32);

            memcpy (signature.r, (uint8_t *) (sig_buf + 2), 32);             /*signature r,s*/
            memcpy (signature.s, (uint8_t *) (sig_buf + 10), 32);

            gfp_ecdsa_p256_verify_init();

            if (gfp_ecdsa_p256_verify( &signature, (uint32_t *)le_sha256_digest, &public_key) == STATUS_SUCCESS)
            {
                DEBUG_PRINT("[SECURE_BOOT] APP image is correct.\n");
                app_verify_result = SECURE_VERIFY_PASS;
            }
            else
            {
                DEBUG_PRINT("[SECURE_BOOT] Error, APP image check failed.\n");
            }
        } while (0);

        if (app_verify_result == SECURE_VERIFY_FAIL)
        {
            if (fota_check == CHECK_FOTA_IMAGE_EXIST)
            {
                DEBUG_PRINT("[SECURE_BOOT] Reboot to FOTA update.\n");
                NVIC_SystemReset();
                while (1);
            }
            else
            {
                while (1);
            }
        }
    }
#endif
    DEBUG_PRINT(
        "[BOOT] Bootloader check is complete. Boot into application.\r\n");
    DEBUG_PRINT("[BOOT] jump to application\r\n");

#if defined(CONFIG_BOOTLOADER_DEBUG)
    uart_stdio_deinit();
#endif
    flush_cache();
    jump_to_application(APP_START_ADDRESS);

    while (1) {}
    return 0;
}
