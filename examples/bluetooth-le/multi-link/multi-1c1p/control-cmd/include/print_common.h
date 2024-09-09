/**************************************************************************//**
* @file       print_common.h
* @brief      Provide the declarations that for BLE AT Commands print related.
*
*****************************************************************************/

#ifndef _print_common_H_
#define _print_common_H_

/** Show Power Measurement Tool Version.
 *
 * @return none
 */
void print_app_tool_version(void);

/** Show Target Device Address.
 *
 * @return none
 */
void print_targetAddr_param(void);

/** Show Advertisement Parameters.
 *
 * @return none
 */
void print_app_adv_param(void);

/** Show Scan Parameters.
 *
 * @return none
 */
void print_app_scan_param(void);

/** Show Connection Parameters.
 *
 * @return none
 */
void print_app_conn_param(void);

/** Show RF PHY Settings.
 *
 * @return none
 */
void print_app_phy_param(void);

/** Show TX Power Settings.
 *
 * @return none
 */
void print_app_txpower_param(void);

/** Show All Application Parameters.
 *
 * @return none
 */
void print_app_param(void);


/** Show Help Message.
 *
 * @return none
 */
void print_ctrl_cmd_help(void);


#endif // _print_common_H_
