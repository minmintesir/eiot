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

#ifndef IMFRAME_H
#define IMFRAME_H

//#include <QtGui/QWSInputMethod>

class InputWidgetNum;
class InputWidgetH;
class InputWidgetV;

class IMFrame : public QWSInputMethod
{
    Q_OBJECT
public:
    explicit IMFrame();
	void updateHandler(int type);

private slots:
    void sendContent(const QString &);

private:
    InputWidgetNum *inputWidgetNum;
	InputWidgetV *inputWidgetV;
	InputWidgetH *inputWidgetH;
};

///---------------------------------------------

enum InputTypeUL {
	IS_UPPER = -1,
	IS_LOWER = 1
};

enum InputTypeDefine {
	NUM_INPUT_FOR_REGISTER,
	NUM_INPUT_FOR_USE_HOST,
    ALL_INPUT_H,
    ALL_INPUT_V,
    ALL_INPUT_V_setup
};

class InputType {
public:
	static int getFlag();
	static void setFlag(int flag);
	static int getUppper();
	static void setUppper(int uppper);

private:
	static int flag;
	static int uppper;
};

#endif // IMFRAME_H
