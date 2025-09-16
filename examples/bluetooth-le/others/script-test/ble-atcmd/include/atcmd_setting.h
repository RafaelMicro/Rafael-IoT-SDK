#ifndef _BLEAPPSETTING_H_
#define _BLEAPPSETTING_H_

#define DEMO_AT_CMD_NONE    0   //Non-connect mode is not available
#define DEMO_AT_CMD_1M      1
#define DEMO_AT_CMD_1S      2
#define DEMO_AT_CMD_1S_1M   3
#define DEMO_AT_CMD_3M      4
#define DEMO_AT_CMD_3S      5

//Select a demo application


#if (BLE_DEMO == DEMO_AT_CMD_1S || BLE_DEMO == DEMO_AT_CMD_1M)
#define LINK_NUM 1
#elif (BLE_DEMO == DEMO_AT_CMD_1S_1M)
#define LINK_NUM 2
#elif (BLE_DEMO == DEMO_AT_CMD_3M || BLE_DEMO == DEMO_AT_CMD_3S)
#define LINK_NUM 3
#endif


#if (BLE_DEMO == DEMO_AT_CMD_NONE)
#define NO_CONNECT
#endif

#endif //_BLEAPPSETTING_H_
