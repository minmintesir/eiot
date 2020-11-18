#/******************************************************************************************
#*-----First Created. time: 16-November-2020	4:35  P.m.
#* @File name    :eiot.pro
#* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
#* @Author       :MacianYuan
#* @E-mail       :macian@foxmail.com
#* @Project      :边缘网关控制器（楼宇供暖控制系统）
#*******************************************************************************************
#* @Description  :
#***(20201113)***:绘制基础显示界面
#***(20201116)***:增加boost-system库 ini配置文件功能 增加log4cplus库 系统日志功能
#***(20201117)***:增加第三方串口类
#***(20201118)***:增加数据库操作
#***(20201119)***:增加屏幕虚拟键盘输入功能
#* @Attention    :
#* @Log          :
#*****************16-November-2020(MacianYuan):
#*****************
#* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
#******************************************************************************************/

QT       += core gui network sql xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eiot
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp \

HEADERS += \
        mainwindow.h \


FORMS += \
        mainwindow.ui

include(./tools/tools.pri)
include(./deviceinfo/deviceinfo.pri)
include(./qextserialport/src/qextserialport.pri)
include(./sqlworker/sqlworker.pri)
include(./servicelogic/servicelogic.pri)

INCLUDEPATH += $$PWD/include

LIBS += -L$$PWD/lib
LIBS += -llog4cplus -lboost_system

RESOURCES += \
    images.qrc

DISTFILES +=
