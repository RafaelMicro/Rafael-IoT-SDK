
#ifndef MMDL_CLIENT_H
#define MMDL_CLIENT_H
#ifdef __cplusplus
extern "C"
{
#endif

#include "stdint.h"
#include "mesh_api.h"
#include "pib.h"
#include "mmdl_opcodes.h"
#include "mmdl_defs.h"

void mmdl_client_send(uint16_t dst_addr, uint16_t src_addr, uint32_t opcode, uint16_t key_index, uint16_t param_len, uint8_t *p_param);

typedef struct  mmdl_transmit_info_s
{
    uint16_t    dst_addr;
    uint16_t    src_addr;
    uint16_t    appkey_index;
} mmdl_transmit_info_t;

#ifdef __cplusplus
}
#endif

#endif /* MMDL_CLIENT_H */
