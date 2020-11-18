/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :globaldata.cpp
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


//#include "deviceinfo/globaldata.h"

//GlobalData *GlobalData::m_Instance = nullptr;
//std::mutex GlobalData::m_Mutex;

//GlobalData *GlobalData::GetInstance() {
//    if (nullptr == m_Instance) {
//        std::lock_guard<std::mutex> gurad(m_Mutex);
//        if (nullptr == m_Instance) {
//            m_Instance = new GlobalData;
//        }
//    }
//    return m_Instance;
//}

//void GlobalData::Destory() {
//    if (nullptr != m_Instance) {
//        std::lock_guard<std::mutex> gurad(m_Mutex);
//        if (nullptr != m_Instance) {
//            delete m_Instance;
//            m_Instance = nullptr;
//        }
//    }
//}

//QObject *GlobalData::GetControlData(ControlDataType type) {
//    QObject *ptr = nullptr;
//    auto iter = m_GlobalObject.find(type);
//    if (m_GlobalObject.end() != iter) {
//        ptr = iter->second;
//    }
//    return ptr;
//}

//GlobalData::GlobalData() {
////    SqlWorker* sqlWork = new SqlWorker();
////    ZY_ASSERT(NULL != sqlWork, "create new SqlMainWorker failed");
////    m_GlobalObject.insert(std::make_pair(SQL_CONTROL, sqlWork));

//}

//GlobalData::~GlobalData() {
//    for (auto v : m_GlobalObject) {
//        if (nullptr != v.second) {
//            delete v.second;
//        }
//    }
//}

//IniParse osla(EIOT_CONFIG);
////QTranslator *GlobalData::appTranslator = NULL  ;
//QString GlobalData::currentLocale = osla.GetString("Global.Language").c_str();

//quint64 GlobalData::id = 0;
//int  GlobalData::user_id= 0;
//int  GlobalData::group_id= 0;
//char* GlobalData::card_serial = NULL;
//int GlobalData::card_type= 0;
//int GlobalData::action_type= 0;
//int GlobalData::action_result= 0;
//int GlobalData::create_time= 0;
//int GlobalData::instrument_id= 0;

//User GlobalData::User;

