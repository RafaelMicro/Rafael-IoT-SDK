
#ifndef EFD_INIT_H_
#define EFD_INIT_H_

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <efd_def.h>

EfErrCode efd_port_init(efd_env const** default_env, size_t* default_env_size);
EfErrCode efd_env_init(efd_env const* default_env, size_t default_env_size);
EfErrCode efd_iap_init(void);
EfErrCode efd_log_init(void);

#endif /* EFD_INIT_H_ */
