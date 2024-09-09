


typedef struct  __attribute__((packed)) _hash_k2_result_t_
{
    uint8_t T2[16];
    uint8_t T3[16];
    uint8_t T1;
} hash_k2_result_t;


void mesh_s1(uint8_t *pin, uint32_t len, uint8_t *mac);
void mesh_k1(uint8_t *Salt, uint8_t *pNin, uint32_t len_N, uint8_t *pPin, uint32_t len_P, uint8_t *K1);
void mesh_k2(uint8_t *N, uint8_t *P, uint32_t len_P, hash_k2_result_t *K2);
void mesh_k3(uint8_t *N, uint8_t *pK3_byte);
uint8_t mesh_k4(uint8_t *N);
