
#ifndef __MESH_DSM_PUL_H__
#define __MESH_DSM_PUL_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "cfg_mdl_type.h"

void dsm_pul_init(void);

uint32_t dsm_publish_set(bool isSIG, req_publish_frame_t *pPublishInfo);
uint32_t dsm_publish_virtual_set(bool isSIG, req_publish_virtual_frame_t *pPublishInfo, uint16_t *pVirtualAddress);

uint32_t dsm_publish_get(uint16_t ElementAddress, uint32_t Model, rsp_publish_frame_t *pPublishInfo);
void dsm_disale_publish_by_del_app_key(uint16_t AppKeyIndex);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_DSM_PUL_H__ */

