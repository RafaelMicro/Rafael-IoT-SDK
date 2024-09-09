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

/** Show Local Device Address.
 *
 * @return none
 */
void print_localAddr_param(void);


/** Show Target Device Address.
 *
 * @return none
 */
void print_targetAddr_param(void);

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

/** Show Connection Parameters.
 *
 * @return none
 */
void print_app_data_len_param(void);

/** Show Test Mode.
 *
 * @return none
 */
void print_app_test_mode_param(void);

/** Show Test TX Total Length.
 *
 * @return none
 */
void print_app_test_total_len_param(void);

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
