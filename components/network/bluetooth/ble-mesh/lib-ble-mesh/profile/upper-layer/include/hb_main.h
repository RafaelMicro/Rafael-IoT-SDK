#ifndef __HB_MAIN_H__
#define __HB_MAIN_H__

#ifdef __cplusplus
extern "C" {
#endif


#include "Cfg_mdl_type.h"

uint32_t hb_publ_set(req_hb_publ_frame_t *pReqHbpublFrmPtr);
uint32_t hb_publ_get(rsp_hb_publ_frame_t *pRspHbpublFrmPtr);

void hb_sub_get(rsp_hb_sur_frame_t *pRspHbSurFrmPtr);
void hb_sub_set(req_hb_sur_frame_t *pReqHbSurfrmPtr, rsp_hb_sur_frame_t *pRspHbSurFrmPtr);


#ifdef __cplusplus
};
#endif
#endif /* __HB_MAIN_H__ */

