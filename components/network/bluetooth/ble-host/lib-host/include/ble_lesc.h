/** @file       ble_lesc.h
 *
 * @brief BLE LESC header file.
 *
*/

/**
 * @defgroup ble_lesc BLE LESC
 * @ingroup BLE_group
 * @{
 * @brief Define BLE LSSC definitions, structures, and functions.
 * @}
 */

#ifndef _BLE_LESC_H_
#define _BLE_LESC_H_


/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include "ble_api.h"
#include "ble_privacy.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**************************************************************************************************
 *    DEFINITIONS
 *************************************************************************************************/
typedef struct lesc_numeric_comparision_generation_format
{
    uint8_t   public_key_x_1[32];
    uint8_t   public_key_x_2[32];
    uint8_t   rand_1[16];
    uint8_t   rand_2[16];
} lesc_numeric_comparision_generation_format;


typedef struct lesc_cfm_value_generation_format
{
    uint8_t   public_key_x_1[32];
    uint8_t   public_key_x_2[32];
    uint8_t   rand[16];
    uint8_t   z;
} lesc_cfm_value_generation_format;

typedef struct lesc_check_value_format
{
    uint8_t   mac_key[16];
    uint8_t   rand1[16];
    uint8_t   rand2[16];
    uint8_t   r[16];
    uint8_t   iocap[3];
    uint8_t   addr1[7];
    uint8_t   addr2[7];
} lesc_check_value_format;

typedef struct lesc_key_generation_format
{
    uint8_t   dh_key[32];
    uint8_t   rand1[16];
    uint8_t   rand2[16];
    uint8_t   addr1[7];
    uint8_t   addr2[7];
} lesc_key_generation_format;


#define SIZE_SMP_PUBLIC_KEY_X                                  32
#define SIZE_SMP_PUBLIC_KEY_Y                                  32

/**************************************************************************************************
 *    GLOBAL PROTOTYPES
 *************************************************************************************************/

uint32_t lesc_numeric_comp_generation_g2(lesc_numeric_comparision_generation_format *p_num_comp_gen);

void lesc_confirm_value_generation_f4(lesc_cfm_value_generation_format *p_cfm_value_gen, uint8_t *encrypt_out);

void lesc_key_generation_f5(lesc_key_generation_format *p_key_gen, uint8_t *encrypt_out);

void lesc_check_value_generation_f6(lesc_check_value_format *p_check_value, uint8_t *encrypt_out);

void lesc_own_dh_key_generation(uint8_t host_id, uint8_t *p_dh_key);

/** @brief Initial LESC related parameter.
 *
 * @ingroup ble_lesc
 *
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_lesc_init(void);


/** @brief Generate the lesc key pair.
 *
 * @ingroup ble_lesc
 *
 *
 * @return @ref BLE_ERR_OK is success or an @ref ble_err_t "error".
 */
ble_err_t ble_lesc_keypair_generate(uint8_t *p_random);

ble_err_t ble_lesc_dhkey_generate(uint8_t *p_public_key, uint8_t *p_private_key, uint8_t *p_dhkey);

ble_err_t ble_lesc_public_key_get(uint8_t *p_public_key);

ble_err_t ble_lesc_private_key_get(uint8_t *p_private_key);

void sec_data_rev(uint8_t *pBuf, uint16_t len);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* _BLE_LESC_H_ */
