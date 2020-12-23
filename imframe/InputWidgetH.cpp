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

#include "imframe/InputWidgetH.h"
#include "imframe/imframe.h"
#include "ui_InputWidgetH.h"

#include <QtCore/QSignalMapper>
#include <QtGui/QWSInputMethod>
#include <QtGui/QMessageBox>

InputWidgetH::InputWidgetH(QWSInputMethod *im)
        : QWidget(0, Qt::Tool | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint), ui(new Ui::InputWidgetH) {
    ui->setupUi(this);
    QSignalMapper *mapper = new QSignalMapper(this);
    connect(mapper, SIGNAL(mapped(const QString &)), im, SLOT(sendContent(const QString &)));

    QList<QPushButton *> btn = this->findChildren<QPushButton *>();
    foreach (QPushButton *b, btn) {
        if (b->objectName() == "bthBackspace") {
            mapper->setMapping(b, "bs1");
            connect(b, SIGNAL(clicked()), mapper, SLOT(map()));
        } else if (b->objectName() == "bthda") {
            mapper->setMapping(b, "bthda");
            connect(b, SIGNAL(clicked()), mapper, SLOT(map()));
        } else {
            if (InputType::getUppper() == -1) {
                mapper->setMapping(b, b->text().toUpper());
                b->setText(b->text().toUpper());
                connect(b, SIGNAL(clicked()), mapper, SLOT(map()));
            } else if (InputType::getUppper() == 1) {
                mapper->setMapping(b, b->text().toLower());
                b->setText(b->text().toLower());
                connect(b, SIGNAL(clicked()), mapper, SLOT(map()));
            }
            qApp->processEvents();
        }
    }
}

InputWidgetH::~InputWidgetH() {
    delete ui;
}
