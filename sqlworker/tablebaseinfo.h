/******************************************************************************************
*-----First Created. time: 16-November-2020	4:35  P.m.
* @File name    :TableBaseIndo.h
* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
* @Author       :MacianYuan
* @E-mail       :macian@foxmail.com
* @Project      :边缘网关控制器（楼宇供暖控制系统）
*******************************************************************************************
* @Description  :
***(20201118)***:创建 用户信息表 用户资格表 面板信息表 热量表信息表 调节阀信息表
* @Attention    :
* @Log          :
*****************16-November-2020(MacianYuan):
*****************
* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
******************************************************************************************/

#ifndef TABLEBASEINFO_H
#define TABLEBASEINFO_H

struct TableDefine {
    char *name;
    char *def;
};

TableDefine tableDefineArr[] = {
        //用户信息表
        //       用户ID(125)  楼号(11号楼栋+1单元 1101)   房间号(27楼层+4号房间 2704)
        //       室内面板SN(云端提供)  M-BUS适配器地址(485地址)  热量表地址(M-BUS地址)  调节阀地址(M-BUS地址)  更新时间
        {
                "TUserMessage",            "create table TUserMessage(UserID INTEGER PRIMARY KEY,BuildingNumber TEXT,RoomNumber TEXT,"
                                            "PanelSN TEXT,AdapterAddr TEXT,HeatMeterAddr TEXT,ControlValveAddr TEXT,"
                                            "UpdateTime INT default (strftime('%s','now')))"
        },

        //用户资格表
        //       用户状态(未激活100 正常101 欠费102 停用109...)    更新时间
        {
                "TUserRight",       "create table TUserRight(UserID INTEGER PRIMARY KEY,Right SMALLINT,"
                                            "UpdateTime INT default (strftime('%s','now')))"
        },

        //面板信息表
        //      面板SN  用户ID  室内温度  设置温度  设置模式  更新时间
        {
                "TPanelMessage ",    "create table TPanelMessage(PanelSN TEXT PRIMARY KEY,UserID INT,"
                                            "RoomTemperature INT,SetTemperature INT,SetMode INT，UpdateTime INT default (strftime('%s','now')))"
        },

        //热量表信息表
        //       M-BUS地址  用户ID  供水温度  回水温度  进回水温差  瞬时流量 瞬时热量(功率) 累计热量 更新时间
        {
                "TMeterMessage",    "create table TMeterMessage(HeatMeterAddr TEXT PRIMARY KEY,UserID INT,"
                                            "InletFlowTemperature INT, OutletFlowTemperature INT,TemperatureDiff INT,"
                                            "CurrentFlow INT,CurrentPower INT,TotalFlowVolume INT,"
                                            "UpdateTime INT default (strftime('%s','now')))"
        },

        //调节阀信息表
        //        M-BUS地址  用户ID  状态  当前角度  设置角度  更新时间
        {
                "TValveMessage",    "create table TValveMessage(ControlValveAddr TEXT PRIMARY KEY,UserID INT,"
                                            "Status INT, CurrentAngle INT,SetAngle INT,"
                                            "UpdateTime INT default (strftime('%s','now')))"
        },

        {       nullptr,            nullptr}
};

#endif //TABLEBASEINFO_H
