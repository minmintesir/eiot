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

#ifndef INPUTWIDGETH_H
#define INPUTWIDGETH_H

#include <QWidget>

class QWSInputMethod;

namespace Ui {
class InputWidgetH;
}

class InputWidgetH : public QWidget
{
    Q_OBJECT
public:
    explicit InputWidgetH(QWSInputMethod *im);
    ~InputWidgetH();

private:
    Ui::InputWidgetH *ui;
};


#endif // INPUTWIDGETH_H
