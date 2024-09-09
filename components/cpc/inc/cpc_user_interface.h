#ifndef __CPC_USER_INTERFACE_H
#define __CPC_USER_INTERFACE_H

#include "cpc_system_common.h"

void cpc_uart_init(void);
void cpc_hci_init(void);
void cpc_sys_signal(void);
void cpc_upgrade_init(void);
void zigbee_app_init(void);
void zigbee_cli_init(void);

/* User implement functions*/
void cpc_system_reset(cpc_system_reboot_mode_t reboot_mode);

#endif // __CPC_USER_INTERFACE_H