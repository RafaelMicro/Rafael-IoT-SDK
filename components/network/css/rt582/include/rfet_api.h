/**
 * @file rfet_api.h
 * @author
 * @date
 * @brief RF evaluation tool API header
 *
 * Header for RT58x series to apply RF evaluation tool
 *
 *
 * @see http://
 */
#ifndef _RFET_API_H_
#define _RFET_API_H_

/**************************************************************************************************
 *    INCLUDES
 *************************************************************************************************/
#include "rf_common_init.h"

/**************************************************************************************************
 *    CONSTANTS AND DEFINES
 *************************************************************************************************/
#define RFET_EVENT_RET_STATUS_SUCCESS           (0x0)
#define RFET_EVENT_RET_STATUS_FAIL              (0x1)
#define RFET_EVENT_RET_STATUS_FAIL_WRONG_FW     (0x2)


/**************************************************************************************************
*    TYPEDEFS
*************************************************************************************************/
typedef void (*RFET_HDLR) (uint8_t *para, uint16_t para_len);


/**************************************************************************************************
 *    Global Prototypes
 *************************************************************************************************/
extern int rfet_main(void);
extern void rfet_gui_cc_event_gen(uint8_t *evt_para, uint16_t para_len);
extern void rfet_gui_cc_entry_registration(RFET_HDLR rfet_hdlr);

#endif  //_RFET_API_H_

