
#include "cfg_mdl_defs.h"
#include "cfg_mdl_type.h"
#include "cfg_mdl_opcode.h"
#include "cfg_mdl_api.h"
#include "cfg_mdl_data.h"
#include "cfg_mdl_cl.h"
#include "cfg_mdl_sr.h"
#include "cfg_mdl_mib.h"
//#include "cfg_mdl_messages.h"

#ifndef __CFG_MDL_H__
#define __CFG_MDL_H__

#ifdef __cplusplus
extern "C" {
#endif

/*! \brief Value to set in order to use the Default TTL */
#define MESH_USE_DEFAULT_TTL                       0xFF



void cfg_mdl_main(cfg_msg_info_t *pCfgMsgInfo, uint8_t *pMsgParam, uint16_t msgParamLen);



#ifdef __cplusplus
};
#endif
#endif /* __CFG_MDL_H__*/

