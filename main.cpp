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

#include "mainwindow.h"
#include <QApplication>
#include <QTextCodec>
#include <QDir>

/******************************************************************************************
* @Function         :main(int argc, char *argv[])
*
* @Purpose          :主函数
* @Description      :
*********************设置支持中文显示，设置字体
*********************设置程序的工作目录
*********************
* @Grammar&Demand   :
*********************<QTextCodec>
*********************<QDir>
* @Param            :
* @Retval           :
* @Others           :
* @e.g.             :
******************************************************************************************/

int main(int argc, char *argv[])
{   //qtvirtualkeyboard
    //Qt5SoftKeyboard
    qputenv("QT_IM_MODULE", QByteArray("qtvirtualkeyboard"));
    QApplication a(argc, argv);

    /* 设置编码格式为utf-8 默认的情况下 无法识别中文路径 所以这里必须要设置为utf-8 */
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QFont font;
    font.setPixelSize(20);
    font.setFamily("fangzheng");
    QApplication::setFont(font);

    /* 设置程序的工作目录为可执行文件所在目录 */
    QDir::setCurrent(QCoreApplication::applicationDirPath());

    MainWindow w;
    w.show();

    return a.exec();
}
