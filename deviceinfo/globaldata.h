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

#ifndef GLOBALDATA_H
#define GLOBALDATA_H

#include <mutex>
#include <map>
#include <QtCore/QObject>
#include <QtCore/QTranslator>
#include "sqlworker/sqlworker.h"
#include <tools/iniparse.h>
#include <deviceinfo/version.h>


//enum UserRightType {
//    SQL_CONTROL,
//    OSLA_CONTROL,
//    CLIENT_CONTROL
//};



//struct Panel_Message {
//  char* panelsn;
//  int userid;
//  int roomtemperature;
//  int settemperature;
//  int setmode;
//};



//struct Valve_Message {
//  char* heatmeteraddr;
//  int userid;
//  int inletflowtemperature;
//  int outletflowtemperature;
//  int temperaturediff;
//  int currentflow;
//  int currentpower;
//  int totalflowvolume;
//};

//class GlobalData
//{
//public:


//private:
//    GlobalData();
//    ~GlobalData();

//private:
//    static std::mutex m_Mutex;
//    static GlobalData* m_Instance;
////    std::map<ControlDataType, QObject*> m_GlobalObject;

//};

#endif //GLOBALDATA_H
