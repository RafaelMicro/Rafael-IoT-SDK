

#ifndef __CFG_MDL_DATA_H__
#define __CFG_MDL_DATA_H__

#ifdef __cplusplus
extern "C" {
#endif



#define CFG_MDL_1_BYTE_OPCODE_NUM       5
#define CFG_MDL_2_BYTE_OPCODE_NUM       66
#define CFG_MDL_3_BYTE_OPCODE_NUM       0

#define CFG_MDL_1_BYTE_CL_OPCODE_NUM    3
#define CFG_MDL_2_BYTE_CL_OPCODE_NUM    44
#define CFG_MDL_3_BYTE_CL_OPCODE_NUM    0
#define CFG_MDL_1_BYTE_SR_OPCODE_NUM    2
#define CFG_MDL_2_BYTE_SR_OPCODE_NUM    22
#define CFG_MDL_3_BYTE_SR_OPCODE_NUM    0





/*! Mesh Configuration Server operation request action handler */
typedef void (*meshCfgMdlSrOpReqAct_t)(cfg_msg_info_t *pCfgMsgInfoPtr, uint8_t *pMsgParam, uint16_t msgParamLen);



typedef struct _opcode_action_t_
{
    uint32_t                    opcode;
    meshCfgMdlSrOpReqAct_t      act_func_ptr;
} opcode_action_t;


extern opcode_action_t CfgMdlActTbl_1byte_opcode[];

extern opcode_action_t CfgMdlActTbl_2byte_opcode[];



#ifdef __cplusplus
};
#endif
#endif /* __CFG_MDL_DATA_H__*/

