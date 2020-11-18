/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :mainwindow.cpp
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
#include "ui_mainwindow.h"
#include <QTableView>
#include <QHeaderView>
#include <QStandardItemModel>

/******************************************************************************************
* @Function         :MainWindow(QWidget *parent):QMainWindow(parent)
*
* @Purpose          :主页面构造函数
* @Description      :
*********************系统初始化:串口初始化,系统时间显示和刷新,接收数据处理,
*********************界面ui显示:
*********************
* @Grammar&Demand   :<QWSServer>
*********************
*********************
* @Param            :
* @Retval           :
* @Others           :
* @e.g.             :
******************************************************************************************/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    /* 创建数据模型 */
    QStandardItemModel* model = new QStandardItemModel();
    /* 设置表格标题行(输入数据为QStringList类型) */
    model->setHorizontalHeaderLabels({"户主", "告警名称", "状态", "等级"});

    /* 自适应所有列，让它布满空间 */
   ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

   /* 加载共10行数据，并每行有6列数据 */
   for (int i = 0; i < 4; i++) {
       /* 加载第一列(设备)数据 */
       model->setItem(i, 0, new QStandardItem(QString("user%1").arg(i)));
       /* 加载第二列(告警名称)数据 */
       model->setItem(i, 1, new QStandardItem("name"));
       /* 加载第三列(状态)数据 */
       model->setItem(i, 2, new QStandardItem("state"));
       /* 加载第四列(等级)数据 */
       model->setItem(i, 3, new QStandardItem("class"));
   }
    ui->tableView->verticalHeader()->hide();
   /* 设置表格视图数据 */
   ui->tableView->setModel(model);

   /* 显示 */
   ui->tableView->show();

   initButtonToGroup();
   //将私有的按键组ID通过信号发送
   connect(ui->buttonGroup, SIGNAL(buttonClicked(QAbstractButton *)),
           this, SLOT(slot_buttonClicked(QAbstractButton *)));
   ui->tableView->setStyleSheet(
   "QTableView{"
   "border:1px solid rgb(96,236,249);"
   "background-color:rgb(13,43,86);"
   "color:white;}"
   "QHeaderView::section {"
   "color: white;"
   "background-color: rgb(0,120,255);"
   "border: 1px solid rgb(96,236,249);"
   "border-radius:0px; }"
   );

   ui->digital_tabwidget->setStyleSheet(
   "QTabWidget:pane{border: 1px solid rgb(96,236,249);}"
   "QTabWidget>QTabBar::tab:selected {"
   "color:white;"
   "background-color:rgb(0,120,255);}"
   "QTabWidget>QTabBar::tab:!selected {"
   "color:white;"
   "background-color:rgb(13,43,86);}"
   );
   ui->analog_tabwidget->setStyleSheet(
   "QTabWidget:pane{border: 1px solid rgb(96,236,249);}"
   "QTabWidget>QTabBar::tab:selected {"
   "color:white;"
   "background-color:rgb(0,120,255);}"
   "QTabWidget>QTabBar::tab:!selected {"
   "color:white;"
   "background-color:rgb(13,43,86);}"
   );
   ui->server_tabwidget->setStyleSheet(
   "QTabWidget:pane{border: 1px solid rgb(96,236,249);}"
   "QTabWidget>QTabBar::tab:selected {"
   "color:white;"
   "background-color:rgb(0,120,255);}"
   "QTabWidget>QTabBar::tab:!selected {"
   "color:white;"
   "background-color:rgb(13,43,86);}"
   );


}
//初始化 按键组
void MainWindow::initButtonToGroup()
{
    ui->buttonGroup->setId(ui->toolButton_home, 0);
    ui->buttonGroup->setId(ui->toolButton_device, 1);
    ui->buttonGroup->setId(ui->toolButton_alarm, 2);
    ui->buttonGroup->setId(ui->toolButton_record, 3);
    ui->buttonGroup->setId(ui->toolButton_setup, 4);
    ui->buttonGroup->setId(ui->toolButton_about, 5);
}

//将私有的按键组ID通过信号发送
void MainWindow::slot_buttonClicked(QAbstractButton *btn)
{
    int id = ui->buttonGroup->id(btn);
    ui->stackedWidget->setCurrentIndex(id);
    QToolButton *obj = qobject_cast<QToolButton*>(btn);
    obj->setChecked(true);
}


MainWindow::~MainWindow()
{
    delete ui;
}
