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

#ifndef CSCANAPRESULTS_H
#define CSCANAPRESULTS_H

#include <QObject>
#include <QFile>
#include <QStringList>
#include <QMap>


struct SSID_INFO
{
    QString bssid;
    int freq;
    int level;
    QString flags;
    QString ssid;
};

class CScanAPResults
{
public:
    explicit CScanAPResults();

    /**
     * @brief setResultFile 设置WIFI扫描结果的文件名
     * @param fileName
     * @return
     */
    bool setResultFile(QString fileName);

    /**
     * @brief getAPLevel 获取SSID对应的信号值
     * @param ssid SSID
     * @return 信号值,-1000 means AP not exist
     */
    int getAPLevel(QString ssid);

    /**
     * @brief getAPbssid 获取SSID对应的BSSID，若存在多个相同的SSID，则返回信号值最小的那一个
     * @param ssid SSID名称
     * @return BSSID
     */
    QString getAPbssid(QString ssid);
    /**
     * @brief getAPList 获取所有的AP列表
     * @return
     */
    inline QList<SSID_INFO> getAPList(void)
    {
        return apList;
    }
private:
    void clearAPList(void);
    void processFileLine(QString line);

    QList<SSID_INFO> apList;
};

#endif // CSCANAPRESULTS_H
