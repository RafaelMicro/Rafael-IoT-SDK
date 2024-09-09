/**************************************************************************//**
 * @file     project_config.h
 * @version
 * @brief   define project config
 *
 * @copyright
 ******************************************************************************/
#ifndef ___PROJECT_CONFIG_H__
#define ___PROJECT_CONFIG_H__

#define MODULE_ENABLE(module)     (module > 0)

/*
 * If system support multi-tasking, some hardware need mutex to protect
 */
#define SUPPORT_MULTITASKING               0
#define SUPPORT_QSPI0_MULTI_CS             0


#endif
