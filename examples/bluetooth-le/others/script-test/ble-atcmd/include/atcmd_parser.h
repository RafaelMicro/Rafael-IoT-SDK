#ifndef _AT_CMD_PARSER_H_
#define _AT_CMD_PARSER_H_

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>
#include "ble_gap.h"
#include "atcmd_ble_param.h"

#define AT_STD_PREFIX_PUNC '+'
#define AT_CMD_SEP_PUNC ';'
#define AT_CMD_PARAM_SEP_PUNC ','
#define AT_CMD_READ_TEST_PUNC '?'
#define AT_CMD_ASSIGN_PUNC '='

#define ERROR_STR "ERROR\r\n"
#define OK_STR "OK\r\n"

#define MAX_CMD_STR_LEN 20        //cmd
#define MAX_PARAM_SIZE 8          //max param_length
#define MAX_EACH_PARAM_STR_LEN 100 //each_param
#define MAX_ALL_CMD_STR_LEN 120    //cmd + all_param

#define MAX_ADV_DATA_SIZE 32
#define ADDR_FORMAT_SIZE 17 //"XX:XX:XX:XX:XX:XX"
#define MAX_NAME_STR_LEN 20

#define min(a, b) ((a) > (b)) ? (b) : (a)
#define SIZE_ARR(a) (sizeof((a))/sizeof((a[0]))) /**< The size of the array.*/

typedef enum atcmd_type
{
    AT_CMD_TYPE_UNKNOW,
    AT_CMD_TYPE_SET_COMMAND,
    AT_CMD_TYPE_READ_COMMAND,
    AT_CMD_TYPE_TEST_COMMAND
} atcmd_type;

typedef enum atcmd_status
{
    AT_CMD_STATUS_OK,
    AT_CMD_STATUS_FAIL,
    AT_CMD_STATUS_QUEUE,
    AT_CMD_STATUS_BUSY,
} atcmd_status;

typedef enum atcmd_param_type
{
    INT,
    ADDR,
    HEX
} atcmd_param_type;

// at_cmd_string
// queue stores this structure
typedef struct atcmd_string_s
{
    char str[MAX_ALL_CMD_STR_LEN];
} atcmd_string_t;

typedef union atcmd_param_block_s
{
    int num;
    char str[MAX_EACH_PARAM_STR_LEN];
    uint8_t hex[MAX_EACH_PARAM_STR_LEN];
    uint8_t addr[BLE_ADDR_LEN];
} atcmd_param_block_t;

typedef struct cmd_info_s cmd_info_t;

typedef struct atcmd_item_s
{
    atcmd_status status;
    ble_err_t err_status;
    char cmd_str[MAX_CMD_STR_LEN];
    atcmd_type cmd_type;
    int param_length;
    atcmd_param_block_t param[MAX_PARAM_SIZE];
    cmd_info_t *cmd_info;
    atcmd_ble_param_t *ble_param;
} atcmd_item_t;

void at_cmd_item_init(atcmd_item_t *this, atcmd_ble_param_t *ble_param);

char *my_strtok(char *s, char d);
void atcmd_string_print(atcmd_string_t *str);
bool parse_cmd_string_to_item(char *cmd_str, atcmd_item_t *item);
bool parse_param_type(atcmd_item_t *item, atcmd_param_type *para_type_list, int para_type_list_len);
bool parse_param_to_int(atcmd_param_block_t *param);
bool parse_param_to_addr(atcmd_param_block_t *param);
bool parse_param_to_hex(atcmd_param_block_t *param);
bool parse_param_to_hex_with_colon(atcmd_param_block_t *param);

void parse_addr_array_to_string(char *str, const uint8_t *addr);
bool parse_addr_string_to_array(uint8_t *addr, const char *str);
bool parse_hex_string_to_array(uint8_t *arr, const char *str);
bool parse_hex_string_to_array_with_colon(uint8_t *arr, const int arr_len, const char *str);
bool parse_hex_array_to_string_with_colon(char *str, const int str_len, const uint8_t *arr, const int arr_len);

#endif //_AT_CMD_PARSER_H_
