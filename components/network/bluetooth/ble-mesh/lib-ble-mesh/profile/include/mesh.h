// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __MESH_H__
#define __MESH_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================

#include "mesh_def.h"
#include "mesh_type.h"
#include "mesh_err.h"
#include "bitfield.h"
#include "pkt_mesh.h"

// BLE
#include "ble_gap.h"
#include "ble_api.h"
#include "ble_event.h"
#include "ble_profile.h"

// BLE Mesh


#include "bar.h"
#include "bcn.h"
#include "prv.h"
#include "nwk.h"
#include "sec.h"
#include "dsm.h"
#include "device_state_manager.h"
#include "pkt_mesh.h"
#include "lower_layer.h"
#include "upper_layer.h"
#include "transport.h"
#include "reply_cache.h"
#include "hb.h"
#include "cfg_mdl.h"
#include "friend.h"
#include "friendship_type.h"
#include "friend_queue.h"
#include "friend_sublist.h"

#include "lpn_sub_mgmt.h"
#include "lpn.h"

#include "proxy.h"

#include "mesh_task.h"

#include "mib_counter.h"
#include "mesh_api.h"



#include "mmdl_common.h"

#include "mmdl_defs.h"

#include "pib.h"



#ifdef __cplusplus
};
#endif
#endif /* __MESH_H__*/
