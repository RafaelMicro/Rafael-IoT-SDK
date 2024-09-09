// ---------------------------------------------------------------------------
// ---------------------------------------------------------------------------
#ifndef __MESH_PROXY_FILTER_H__
#define __MESH_PROXY_FILTER_H__

#ifdef __cplusplus
extern "C" {
#endif

//=============================================================================
//                Include (Better to prevent)
//=============================================================================


//=============================================================================
//                Public Function Declaration
//=============================================================================
#define MESH_GATT_PROXY_FILTER_ADDR_COUNT 16

typedef enum
{
    PROXY_FILTER_TYPE_WHITELIST,
    PROXY_FILTER_TYPE_BLACKLIST,
    PROXY_FILTER_TYPE_RFU_START,
} proxy_filter_type_t;

typedef struct
{
    uint16_t addrs[MESH_GATT_PROXY_FILTER_ADDR_COUNT];
    uint16_t count;
    proxy_filter_type_t type;
} proxy_filter_t;

void proxy_filter_clear(proxy_filter_t *p_filter);
uint32_t proxy_filter_type_set(proxy_filter_t *p_filter, proxy_filter_type_t type);
void proxy_filter_add(proxy_filter_t *p_filter, uint16_t *p_addrs, uint32_t addr_count);
void proxy_filter_remove(proxy_filter_t *p_filter, uint16_t *p_addrs, uint32_t addr_count);
bool proxy_filter_accept(proxy_filter_t *p_filter, uint16_t addr);

#ifdef __cplusplus
};
#endif
#endif /* __MESH_PROXY_FILTER_H__ */
