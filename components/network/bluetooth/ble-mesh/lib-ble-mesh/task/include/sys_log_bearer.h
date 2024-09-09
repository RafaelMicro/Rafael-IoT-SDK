// ---------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------
#ifndef __MESH_SYS_LOG_BEARER_H__
#define __MESH_SYS_LOG_BEARER_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================
#include "sys_log.h"

//=============================================================================
//                Public Definitions of const value
//=============================================================================

//=============================================================================
//                Public ENUM
//=============================================================================
typedef enum SYSLOG_TYPE_MESH_BEARER
{
    TYPE_BEARER_HOST_REQ,
    TYPE_BEARER_HOST_IDC,
    TYPE_BEARER_MAX,
} bearer_syslog_t;

typedef enum SYSLOG_SUBTYPE_MESH_BEARER_REQ
{
    SUBTYPE_BEARER_HOST_REQ_START = 0,
    SUBTYPE_BEARER_HOST_REQ_FAIL,
    SUBTYPE_BEARER_HOST_REQ_CFM,
    SUBTYPE_BEARER_HOST_REQ_MAX,
} bearer_req_t;

typedef enum SYSLOG_SUBTYPE_MESH_BEARER_IDC
{
    SUBTYPE_BEARER_HOST_IDC_START = 0,
    SUBTYPE_BEARER_HOST_IDC_MAX,
} bearer_idc_t;
//=============================================================================
//                Public Struct
//=============================================================================

//=============================================================================
//                Public Function Declaration
//=============================================================================
void syslog_bearer_init(void);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_SYS_LOG_BEARER_H__ */
