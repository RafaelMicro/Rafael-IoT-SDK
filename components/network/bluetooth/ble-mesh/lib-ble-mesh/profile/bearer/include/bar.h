// ---------------------------------------------------------------------------
// All rights reserved.
//
// ---------------------------------------------------------------------------
#ifndef __BAR_H__
#define __BAR_H__

#ifdef __cplusplus
extern "C" {
#endif
//=============================================================================
//                Include (Better to prevent)
//=============================================================================

#include "bar_def.h"
#include "bar_type.h"
#include "bar_main.h"
#include "bar_data.h"
#include "bar_pb_adv.h"
#include "bar_pb_gatt.h"
#include "bar_tx_fsm.h"
#include "bar_tx.h"
#include "bar_rx.h"
#include "sys_log_bearer.h"

extern uint8_t          g_advertising_host_id;
extern uint8_t          g_search_unprovision_device;

#ifdef __cplusplus
};
#endif
#endif /* __BAR_H__ */
