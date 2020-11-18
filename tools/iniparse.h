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


#ifndef INIPARSE_H
#define INIPARSE_H

#include <string>
#include <boost/noncopyable.hpp>
#include <boost/property_tree/ptree.hpp>  
#include <boost/property_tree/ini_parser.hpp> 

class IniParse : public boost::noncopyable
{
public:
	IniParse(std::string fileName);
	~IniParse();
	bool Get(std::string path, bool& value);
	bool Get(std::string path, long& value);
	bool Get(std::string path, std::string& value);

	bool GetBool(std::string path, bool def = false);
	long GetLong(std::string path, long def = 0);
	std::string GetString(std::string path, std::string def = "");

	bool Set(std::string path, bool value);
	bool Set(std::string path, long value);
	bool Set(std::string path, int value);
	bool Set(std::string path, const std::string& value);
	bool SaveFile();

	bool Del(std::string path);

private:
	std::string m_FileName;
	bool m_InitSuccess{ false };
	boost::property_tree::ptree m_PT;

};

#endif //INIPARSE_H
