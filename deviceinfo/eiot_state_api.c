/**
 * @file aiot_state_api.c
 * @brief 状态码模块实现
 * @date 2019-12-27
 *
 * @copyright Copyright (C) 2015-2018 Alibaba Group Holding Limited
 *
 */


#include "eiot_state_api.h"

eiot_state_logcb_t g_logcb_handler = NULL;

int32_t eiot_state_set_logcb(eiot_state_logcb_t handler)
{
    g_logcb_handler = handler;
    return 0;
}

