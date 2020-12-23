/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :main.cpp
* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
* @Author       :MacianYuan
* @E-mail       :macian@foxmail.com
* @Project      :边缘网关控制器（楼宇供暖控制系统）
*******************************************************************************************
* @Description  :
* @Attention    :
* @Log          :
*****************16-November-2020(MacianYuan):
*****************
* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
******************************************************************************************/


#ifndef OSLA_NEW_WATCHDOG_H
#define OSLA_NEW_WATCHDOG_H


#include <mutex>

class WatchDog {
public:
    static WatchDog* GetInstance();
    static void DestroyInstance();

private:
    WatchDog();
    ~WatchDog();
    void Init();
    void Destroy();

public:
    void Feed();

private:
    int fd {0};

    static WatchDog* m_Instance;
    static std::mutex m_Mutex;
};


#endif //OSLA_NEW_WATCHDOG_H
