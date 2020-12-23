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

#ifndef CWLANCONFIGFILE_H
#define CWLANCONFIGFILE_H

#include <QObject>

class CWlanConfigFile
{
public:
    explicit CWlanConfigFile();
    bool isValid(void);
    void readConfigFile(QString fileName);
    void writeConfigFile(QString fileName);
    QString getSsid(void);
    QString getBssid(void);
    QString getPassword(void);
    void setSsid(QString aSsid);
    void setBssid(QString aBssid);
    void setPassword(QString aPassword);
    static QString removeLevleForSsid(QString ssid);
private:
    void clear();
    void processLine(QString line);

    bool validFlag;
    QString ssid;
    QString bssid;
    QString password;
};

#endif // CWLANCONFIGFILE_H
