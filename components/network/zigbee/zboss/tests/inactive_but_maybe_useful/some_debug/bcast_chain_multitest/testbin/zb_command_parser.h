/* ZBOSS Zigbee software protocol stack
 *
 * Copyright (c) 2012-2020 DSR Corporation, Denver CO, USA.
 * www.dsr-zboss.com
 * www.dsr-corporation.com
 * All rights reserved.
 *
 * This is unpublished proprietary source code of DSR Corporation
 * The copyright notice does not evidence any actual or intended
 * publication of such source code.
 *
 * ZBOSS is a registered trademark of Data Storage Research LLC d/b/a DSR
 * Corporation
 *
 * Commercial Usage
 * Licensees holding valid DSR Commercial licenses may use
 * this file in accordance with the DSR Commercial License
 * Agreement provided with the Software or, alternatively, in accordance
 * with the terms contained in a written agreement between you and
 * DSR.
 */
/*  PURPOSE:
*/
#ifndef ZB_COMMAND_PARSER_H
#define ZB_COMMAND_PARSER_H 1

typedef zb_bool_t (param_handler_function_t) (char *key, char *value);

/**
 * Reads tokens: only alphanumeric charachers and '_-' are allowed 
 * @param ptr - points to string pointer, will be moved
 * @param dst - buffer to store parsed token
 * @param max_len - size of buffer pointed to by dst
 **/
zb_bool_t parse_command_token(char **ptr, char *dst, int max_len);

/**
 * Handles parameter list of the key = value
 * They may or may not be separeted by commas.
 * @returns true if everything is parsed and successfully handled
 */
zb_bool_t handle_command_parameter_list(char *ptr, param_handler_function_t func);

#endif /* ZB_COMMAND_PARSER_H */
