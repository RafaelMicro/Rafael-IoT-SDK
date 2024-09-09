

#ifndef __MESH_DSM_SUR_H__
#define __MESH_DSM_SUR_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cfg_mdl_type.h"

void dsm_sur_init(void);
void dsm_sur_update(uint8_t remove);

uint32_t dsm_subscribe_add(bool isSIG, req_subscribe_frame_t *pReqSurFram);
uint32_t dsm_subscribe_del(bool isSIG, req_subscribe_frame_t *pReqSurFram);
uint32_t dsm_subscribe_ovr(bool isSIG, req_subscribe_frame_t *pReqSurFram);

uint32_t dsm_subscribe_address_check(uint16_t sub_addr);


uint32_t dsm_subscribe_virtual_add(bool isSIG, req_subscribe_virtual_frame_t *pReqSurVirFram, uint16_t *pVirtualAddress);
uint32_t dsm_subscribe_virtual_del(bool isSIG, req_subscribe_virtual_frame_t *pReqSurVirFram, uint16_t *pVirtualAddress);
uint32_t dsm_subscribe_virtual_ovr(bool isSIG, req_subscribe_virtual_frame_t *pReqSurVirFram, uint16_t *pVirtualAddress);
uint32_t dsm_subscribe_del_all(bool isSIG, uint16_t ElementAddress, uint32_t ModelID);
uint32_t dsm_subscribe_get_sig_list(uint16_t Address, uint16_t ModelId,  uint16_t *pAddr, uint16_t *pCout);
uint32_t dsm_subscribe_get_vend_list(uint16_t Address, uint32_t ModelId, uint16_t *pAddr, uint16_t *pCout);


void dsm_app_mdl_regist(uint32_t ModelID, bool isSIG, uint16_t elementAddr);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_DSM_SUR_H__ */

