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

#include <QStringList>
#include <QFile>
#include "cwlanconfigfile.h"


CWlanConfigFile::CWlanConfigFile() :
    validFlag(false)
{

}

bool CWlanConfigFile::isValid()
{
    return validFlag;
}

void CWlanConfigFile::readConfigFile(QString fileName)
{
    clear();

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly))
    {
        return;
    }

    while (!file.atEnd())
    {
        QString line = file.readLine();
        processLine(line);
    }

    validFlag = true;
}

void CWlanConfigFile::writeConfigFile(QString fileName)
{
    if (!validFlag)
    {
        return;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::WriteOnly))
    {
        return;
    }

    file.write("ctrl_interface=/var/run/wpa_supplicant\n");
    file.write("network={\n");
    file.write("    key_mgmt=WPA-PSK\n");
    file.write("    scan_ssid=1\n");
    file.write("    proto=RSN\n");
    file.write("    ssid=\"");
    file.write(ssid.toLatin1());
    file.write("\"\n");
    file.write("    psk=\"");
    file.write(password.toLatin1());
    file.write("\"\n");
    if (!bssid.isEmpty())
    {
        file.write("    bssid=");
        file.write(bssid.toLatin1());
        file.write("\n");
    }
    file.write("}\n");
}

QString CWlanConfigFile::getSsid()
{
    return ssid;
}

QString CWlanConfigFile::getBssid()
{
    return bssid;
}

QString CWlanConfigFile::getPassword()
{
    return password;
}

void CWlanConfigFile::setBssid(QString aBssid)
{
    validFlag = true;
    bssid = aBssid;
}

void CWlanConfigFile::setPassword(QString aPassword)
{
    validFlag = true;
    password = aPassword;
}

QString CWlanConfigFile::removeLevleForSsid(QString ssid)
{
    QStringList strList = ssid.split("]");
    if (strList.count() == 2)
    {
        return strList[1];
    }

    return ssid;
}

void CWlanConfigFile::setSsid(QString aSsid)
{
    validFlag = true;
    ssid = aSsid;
}

void CWlanConfigFile::clear()
{
    validFlag = false;
    ssid.clear();
    bssid.clear();
    password.clear();
}

void CWlanConfigFile::processLine(QString line)
{
    QStringList strList = line.split("=");
    if (strList.count() == 2)
    {
        if (strList[0].trimmed().toLower() == "ssid")
        {
            ssid = strList[1].trimmed().remove('"');
        }
        else if (strList[0].trimmed().toLower() == "psk")
        {
            password = strList[1].trimmed().remove('"');
        }
        else if (strList[0].trimmed().toLower() == "bssid")
        {
            bssid = strList[1].trimmed().remove('"');
        }
    }
}
