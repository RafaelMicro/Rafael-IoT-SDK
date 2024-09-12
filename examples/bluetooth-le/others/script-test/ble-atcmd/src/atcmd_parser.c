#include "atcmd_parser.h"

// PRIVATE VARIABLE
char string_for_tok[256];

// PRIVATE FUNCTION DECLARE
static bool parse_param(atcmd_item_t *item, char *param_str);
static void mem_copy_substr(char *dest, const char *scr, int pos, int len);
static bool str_to_int(int *dest, char *str);

// PUBLIC FUNCTION IMPLEMENT
char *my_strtok(char *s, char d)
{
    static char *input = NULL;
    int i;
    char *result;
    if (s != NULL)
    {
        input = s;
    }
    if (input == NULL)
    {
        return NULL;
    }
    result = string_for_tok;
    for (i = 0; input[i] != '\0'; i++)
    {
        if (input[i] != d)
        {
            result[i] = input[i];
        }
        else
        {
            result[i] = '\0';
            input = input + i + 1;
            return result;
        }
    }
    result[i] = '\0';
    input = NULL;
    return result;
}
void at_cmd_item_init(atcmd_item_t *this, atcmd_ble_param_t *ble_param)
{
    this->status = AT_CMD_STATUS_QUEUE;
    this->ble_param = ble_param;
}
void atcmd_string_print(atcmd_string_t *str)
{
    printf("%s", str->str);
}
bool parse_cmd_string_to_item(char *cmd_str, atcmd_item_t *item)
{
    char *pch;
    char *param_str;
    int length = strlen(cmd_str);
    bool check = true;

    item->status = AT_CMD_STATUS_QUEUE;
    item->err_status = BLE_ERR_OK;

    if (cmd_str[length - 1] == AT_CMD_READ_TEST_PUNC)
    {
        if (cmd_str[length - 2] == AT_CMD_ASSIGN_PUNC)
        {
            // ATXXX=? ex.AT+CMGL=?
            mem_copy_substr(item->cmd_str, cmd_str, 0, length - 2);
            item->cmd_type = AT_CMD_TYPE_TEST_COMMAND;
        }
        else
        {
            // ATXXX? ex.AT+CPBS?
            mem_copy_substr(item->cmd_str, cmd_str, 0, length - 1);
            item->cmd_type = AT_CMD_TYPE_READ_COMMAND;
        }
    }
    else
    {
        pch = strchr(cmd_str, AT_CMD_ASSIGN_PUNC);
        if (pch == NULL)
        {
            // ATXXX ex.AT+CNUM
            mem_copy_substr(item->cmd_str, cmd_str, 0, length);
            item->cmd_type = AT_CMD_TYPE_SET_COMMAND;
            item->param_length = 0;
        }
        else
        {
            // ATXXX=<a>,<b> ex.AT+ADVTYPE=0
            mem_copy_substr(item->cmd_str, cmd_str, 0, pch - cmd_str);
            item->cmd_type = AT_CMD_TYPE_SET_COMMAND;
            // store param
            param_str = pch + 1;
            check = parse_param(item, param_str);
            if (!check)
            {
                return false;
            }
        }
    }
    return true;
}
bool parse_param_type(atcmd_item_t *item, atcmd_param_type *para_type_list, int para_type_list_len)
{
    bool check = true;

    //check length
    if (item->param_length != para_type_list_len)
    {
        return false;
    }

    //parse param type
    for (int i = 0; i < para_type_list_len; i++)
    {
        switch (para_type_list[i])
        {
        case INT:
            check = parse_param_to_int(&item->param[i]);
            break;
        case ADDR:
            check = parse_param_to_addr(&item->param[i]);
            break;
        case HEX:
            check = parse_param_to_hex_with_colon(&item->param[i]);
            break;
        default:
            check = false;
            break;
        }
        if (!check)
        {
            return check;
        }
    }
    return check;
}
bool parse_param_to_int(atcmd_param_block_t *param)
{
    return str_to_int(&(param->num), param->str);
}
bool parse_param_to_addr(atcmd_param_block_t *param)
{
    char str[MAX_EACH_PARAM_STR_LEN];
    strcpy(str, param->str);
    return parse_addr_string_to_array(param->addr, param->str);
}
bool parse_param_to_hex(atcmd_param_block_t *param)
{
    int str_len;
    char str[MAX_EACH_PARAM_STR_LEN];
    strcpy(str, param->str);
    if (!parse_hex_string_to_array(param->hex, str))
    {
        return false;
    }

    str_len = strlen(str);
    param->hex[str_len / 2] = 0;

    return true;
}
bool parse_param_to_hex_with_colon(atcmd_param_block_t *param)
{
    //use for adv data & scan rsp
    char str[MAX_EACH_PARAM_STR_LEN];
    strcpy(str, param->str);
    return parse_hex_string_to_array_with_colon(param->hex, sizeof(param->hex), str);
}
void parse_addr_array_to_string(char *str, const uint8_t *addr)
{
    int i;
    sprintf(str, "%02hx", addr[0]);

    for (i = 1; i < BLE_ADDR_LEN; i++)
    {
        sprintf(str, "%s:%02hx", str, addr[i]);
    }
    str[ADDR_FORMAT_SIZE] = '\0';
}
bool parse_addr_string_to_array(uint8_t *addr, const char *str)
{
    int i;
    uint16_t addr_a;

    //check format
    if (strlen(str) != ADDR_FORMAT_SIZE)
    {
        return false;
    }
    for (i = 0; i < ADDR_FORMAT_SIZE; i++)
    {
        if (i % 3 == 2)
        {
            if (str[i] != ':')
            {
                return false;
            }
        }
        else
        {
            if (!isxdigit(str[i]))
            {
                return false;
            }
        }
    }

    //get address value
    for (i = 0; i < BLE_ADDR_LEN; i++)
    {
        sscanf(str, "%02hx", (unsigned short *)&addr_a);
        str += 3;
        addr[i] = addr_a;
    }
    return true;
}
bool parse_hex_string_to_array(uint8_t *arr, const char *str)
{
    int i;
    int len;
    int str_len;
    uint16_t data_a;

    //check format
    str_len = strlen(str);
    if (str_len % 2 != 0)
    {
        return false;
    }
    for (i = 0; i < str_len; i++)
    {
        if (!isxdigit(str[i]))
        {
            return false;
        }
    }

    len = str_len / 2;
    //get hex arr
    for (i = 0; i < len; i++)
    {
        sscanf(str, "%02hx", (unsigned short *)&data_a);
        str += 2;
        arr[i] = data_a;
    }
    return true;
}
bool parse_hex_string_to_array_with_colon(uint8_t *arr, const int arr_len, const char *str)
{
    /*
    turn hex str to uint8_t array
    example: str="12:34" to arr={0x12,0x34}
    if array length over string length, it will pedding with '0x00'.
    from the other way, if string length over array length, it will return false
    */
    int i;
    int len;
    int str_len;
    uint16_t data_a;

    // check format
    str_len = strlen(str);
    if (str_len % 3 != 2)
    {
        return false;
    }
    len = str_len / 3 + 1;
    if (arr_len < len)
    {
        return false;
    }
    for (i = 0; i < str_len; i++)
    {
        if (i % 3 == 2)
        {
            if (str[i] != ':')
            {
                return false;
            }
        }
        else
        {
            if (!isxdigit(str[i]))
            {
                return false;
            }
        }
    }

    // get value
    len = str_len / 3 + 1;
    for (i = 0; i < len; i++)
    {
        sscanf(str, "%02hx", (unsigned short *)&data_a);
        str += 3;
        arr[i] = data_a;
    }
    while (i < arr_len)
    {
        arr[i] = 0;
        i++;
    }
    return true;
}
bool parse_hex_array_to_string_with_colon(char *str, const int str_len, const uint8_t *arr, const int arr_len)
{
    /*
    turn uint8_t array to hex str
    example: arr={0x12,0x34} to str="12:34"
    if array length over string length, it will return false
    */
    int i;
    // if array length is zero, return str = "00"
    if (arr_len == 0)
    {
        if (str_len < 2)
        {
            return false;
        }

        sprintf(str, "00");
        return true;
    }

    // except string length turned from array length is not enough
    if (str_len < (arr_len * 3 - 1))
    {
        return false;
    }

    // get string
    sprintf(str, "%02hx", arr[0]);
    for (i = 1; i < arr_len; i++)
    {
        sprintf(str, "%s:%02hx", str, arr[i]);
    }
    return true;
}

// PRIVATE FUNCTION IMPLEMENT
static bool parse_param(atcmd_item_t *item, char *param_str)
{
    int i = 0;

    //split command param by ','
    char *param = my_strtok(param_str, AT_CMD_PARAM_SEP_PUNC);
    while (param != NULL)
    {
        if (i == MAX_PARAM_SIZE)
        {
            break;
        }

        if (strlen(param) > (sizeof(item->param[i].str) - 1))
        {
            printf("ERROR : this at command length is too long\r\n");
            return false;
        }
        strcpy(item->param[i].str, param);
        i++;

        param = my_strtok(NULL, AT_CMD_PARAM_SEP_PUNC);
    }
    item->param_length = i;

    return true;
}
static void mem_copy_substr(char *dest, const char *src, int pos, int len)
{
    memcpy(dest, src + pos, len + 1);
    dest[len] = '\0';
}
static bool str_to_int(int *dest, char *str)
{
    int res = 0;
    int i;

    //string size check
    if (strlen(str) > 10)
    {
        return false;
    }

    for (i = 0; i < strlen(str); i++)
    {
        //format check
        if (!isdigit(str[i]))
        {
            return false;
        }
        res *= 10;
        res += (str[i] - '0');
    }
    *dest = res;
    return true;
}

