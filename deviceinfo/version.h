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

#ifndef VERSION_H
#define VERSION_H

#define APP_NAME            "EIOT"
#define APP_VERSION_STR     "1.00"
#define APP_AUTHOR          "macian@foxmail.com"
#define ORG_NAME            "xinjie"

#define EIOT_CONFIG         "/storage/conf/eiot.conf"
#define CLIENT_CONFIG	    "/storage/conf/controller.conf"
#define SSL_CERT            "/storage/conf/sslcert.pem"
#define NET_CONFIG          "/storage/conf/net.conf"
#define NETTYPE_CONFIG      "/storage/conf/nettype.conf"
#define WLAN_CONFIG         "/storage/conf/wlan.conf"
#define WLAN_SCAN_FILE      "/storage/conf/wlan_sacn.conf"

#define DATABASE_USER       ""
#define DATABASE_PASS       ""
#define DATABASE_NAME       "/storage/conf/sql.db"
#define DATABASE_HOST       ""
#define DATABASE_DRIVER     "QSQLITE"

//升级部分数据定义
//下载后的文件名
#define UPGRADE_CONFIG              "/storage/conf/upgrade.conf"
#define UPGRADE_FILE_NAME           "/storage/sd0/upgrade/upgrade.tar.gz"
#define UPGRADE_UNCOMPRESS_PATH     "/storage/sd0/upgrade/dest"
#define UPDATE_SH                   "/storage/sd0/upgrade/dest/upgrade.sh"

//#define OFFLINE             1

#endif // VERSION_H
