/**
 * @file product_config.h
 * @author Joshua Jean (joshua.jean@rafaelmicro.com)
 * @brief This file includes all product needs defines
 * @version 0.1
 * @date 2022-08-18
 *
 * @copyright Copyright (c) 2022
 *
 */


#ifndef __PRODUCT_CONFIG_H__
#define __PRODUCT_CONFIG_H__

#ifdef __cplusplus
extern "C"
{
#endif
#define     RAFAEL_RT58X_SDK_VERSION                    {"1.3.0"}
#define     RAFAEL_COMPANY_ID                           0x0864
#define     RAFAEL_PRODUCT_ID                           0x1658
#define     RAFAEL_VENDOR_ID                            0x1688
#define     RAFAEL_PRODUCT_SW_MAJOR_VER                 0
#define     RAFAEL_PRODUCT_SW_SUBMAJOR_VER              0
#define     RAFAEL_PRODUCT_SW_MINOR_VER                 1
#define     RAFAEL_PRODUCT_SW_SUBMINOR_VER              0
#define     RAFAEL_PRODUCT_SW_BUILDCODE                 20220818
#define     RAFAEL_PRODUCT_HW_VER                       {"HW-01"}
#define     RAFAEL_PRODUCT_MODEL_ID                     {"RT-MESH-1"}
#define     RAFAEL_PRODUCT_MODEL_NAME                   {"RT MESH Server Demo"}

/* define the product supported features */
#define     RAFAEL_PRODUCT_FEATURE_RELAY                1
#define     RAFAEL_PRODUCT_FEATURE_PROXY                1
#define     RAFAEL_PRODUCT_FEATURE_LOW_POWER            0
#define     RAFAEL_PRODUCT_FEATURE_FRIEND               1

/* define provision related information for device provisioning */
#define     RAFAEL_PRODUCT_PROVISION_ALGORITHM          1 // FIPS P-256 Elliptic Curve
#define     RAFAEL_PRODUCT_PROVISION_OOB_PUBLIC_KEY     0 // No OOB public Key
#define     RAFAEL_PRODUCT_PROVISION_STATIC_OOB         0 // No Static OOB information
#define     RAFAEL_PRODUCT_PROVISION_OUPUT_OOB_SIZE     0 // Output OOB size 0
#define     RAFAEL_PRODUCT_PROVISION_OUPUT_OOB_ACTION   0 // Output OOB action 
#define     RAFAEL_PRODUCT_PROVISION_INPUT_OOB_SIZE     0 // Input OOB size 
#define     RAFAEL_PRODUCT_PROVISION_INPUT_OOB_ACTION   0 // Input OOB action

#ifdef __cplusplus
};
#endif

#endif /* __PRODUCT_CONFIG_H__*/
