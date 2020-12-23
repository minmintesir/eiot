/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :watchdog.cpp
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

#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <deviceinfo/version.h>
#include "tools/watchdog.h"
#include "log.h"
#include "iniparse.h"
#include <cstdio>


#define USE_WATCDOG


#define ZY_ASSERT(condition, fmt, ...)  {\
    if(false == (condition)) { \
        char format[4086] = {0}; \
        sprintf(format, "[%s-%d]:%s\n", __FUNCTION__, __LINE__, fmt); \
        printf(format, ##__VA_ARGS__); \
        throw; \
    } \
}

WatchDog *WatchDog::m_Instance = nullptr;
std::mutex WatchDog::m_Mutex;

struct watchdog_info {
    unsigned int options;   //options the card/driver supprots 19
    unsigned int firmware_version;  //firmcard version of the card
    unsigned char identity[32];     //identity of the board 21
};

#define WATCHDOG_IOCTL_BASE     'W'
#define WDIOC_GETSUPPORT        _IOR(WATCHDOG_IOCTL_BASE, 0, struct watchdog_info)
#define WDIOC_SETTIMEOUT        _IOWR(WATCHDOG_IOCTL_BASE, 6, int)
#define WDIOC_GETTIMEOUT        _IOR(WATCHDOG_IOCTL_BASE, 7, int)
#define WDIOS_DISABLECARD       0x0001        /* Turn off the watchdog timer */
#define WDIOS_ENABLECARD        0x0002 /* Turn on the watchdog timer */
#define WDIOC_SETOPTIONS        _IOR(WATCHDOG_IOCTL_BASE, 4, int)
#define WDIOC_KEEPALIVE         _IOR(WATCHDOG_IOCTL_BASE, 5, int)

WatchDog::WatchDog() {
    Init();
}

WatchDog::~WatchDog() {
    Destroy();
}

WatchDog *WatchDog::GetInstance() {
    if (nullptr == m_Instance) {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if (nullptr == m_Instance) {
            m_Instance = new WatchDog();
        }
    }
    return m_Instance;
}

void WatchDog::DestroyInstance() {
    if (nullptr != m_Instance) {
        std::lock_guard<std::mutex> guard(m_Mutex);
        if (nullptr != m_Instance) {
            delete m_Instance;
            m_Instance = nullptr;
        }
    }
}

//TODO:临时禁用，为调试
void WatchDog::Init() {
#ifdef USE_WATCDOG
    IniParse ini(EIOT_CONFIG);
    bool isUseWatchDog = ini.GetBool("WatchDog.use", true);
    if (false == isUseWatchDog) {
        return;
    }
    fd = open("/dev/watchdog", O_RDWR);   //打开看门狗设备
    ZY_ASSERT(fd >= 0, "device[/dev/watchdog] open fail");
    int value = 60;
    ioctl(fd, WDIOC_SETTIMEOUT, &value);
    value = WDIOS_DISABLECARD;
    ioctl(fd, WDIOC_SETOPTIONS, &value);
    value = WDIOS_ENABLECARD;
    ioctl(fd, WDIOC_SETOPTIONS, &value);
 #endif
}

//TODO:临时禁用，为调试
void WatchDog::Destroy() {
#ifdef USE_WATCDOG
    DEBUG_LOG("Watch Dog Closeing");
    if (fd > 0) {
        int value = WDIOS_DISABLECARD;
        ioctl(fd, WDIOC_SETOPTIONS, &value);
        close(fd);
        fd = 0;
        DEBUG_LOG("Watch Dog Closed");
    }
#endif
}
//TODO:临时禁用，为调试
void WatchDog::Feed() {
#ifdef USE_WATCDOG
    if (fd > 0) {
        ioctl(fd, WDIOC_KEEPALIVE, NULL);
    }
#endif
}
