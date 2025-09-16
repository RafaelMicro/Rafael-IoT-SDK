#include "atcmd_helper.h"
#include "ble_app.h"

bool jump_to_main(void)
{
    bool check = app_request_set(QUEUE_TYPE_APP_REQ, false);
    return check;
}

bool jump_to_main_isr(void)
{
    bool check = app_request_set(QUEUE_TYPE_APP_REQ, true);
    return check;
}
