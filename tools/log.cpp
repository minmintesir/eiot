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


#include "tools/log.h"

#include <log4cplus/ndc.h>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/layout.h>
#include <log4cplus/loggingmacros.h>

using namespace log4cplus;
using namespace log4cplus::helpers;

bool LoggerControl::init(std::string fileName, int size, int num) {
    SharedAppenderPtr _append(new RollingFileAppender(fileName, size, num));
    _append->setName("QT");
    std::string pattern("%D{%m/%d/%y %H:%M:%S} : %m [%F:%L]%n");
    _append->setLayout(std::auto_ptr<Layout>(new PatternLayout(pattern)));
    Logger::getRoot().addAppender(_append);
    return true;
}

Logger LoggerControl::getRoot() {
    static Logger root = Logger::getRoot();
    return root;
}

LoggerControl& LoggerControl::getInstance() {
    static LoggerControl info;
    return info;
}
