//
// ---------------------------------------------------------------------------
#ifndef __BAR_DEF_H__
#define __BAR_DEF_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#define BARER_ADV_HEADR_SIZE    2

/* define the interface HOST ID */

enum interface_def
{
    IF_GATT_PROVISION =     0x00,
    IF_GATT_MIN       =     0x01,
    IF_GATT_MAX       =     0x7E,
    IF_ADV            =     0x7F,
    IF_ALL            =     0XFF,
};

#ifdef __cplusplus
};
#endif
#endif /* __BAR_DEF_H__ */
