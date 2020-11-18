///******************************************************************************************
//*-----First Created. time: 16-November-2020	4:35  P.m.
//* @File name    :main.cpp
//* @Platform     :imx6ull * openwrt19.07.2 * linux 4.1.15 * Qt5.5.1 * arm-openwrt-linux-gnueabi-
//* @Author       :MacianYuan
//* @E-mail       :macian@foxmail.com
//* @Project      :边缘网关控制器（楼宇供暖控制系统）
//*******************************************************************************************
//* @Description  :
//* @Attention    :
//* @Log          :
//*****************16-November-2020(MacianYuan):
//*****************
//* @Copyright© @MacianYuan Anhui Xinjie Technology Co.,Ltd.  2010-2020  All rights reserved.
//******************************************************************************************/

//#include <stdio.h>
//#include <stdlib.h>
//#include <fcntl.h>
//#include <sys/soundcard.h>
//#include <linux/watchdog.h>

//#include "imx6ull.h"

//extern sysfs_write(const char* fname, const char* dat, int len);
////#define BACKLIGHT_NAME_VALUE "/sys/class/pwm/ehrpwm.1\:0/duty_percent"
//#define BACKLIGHT_NAME_VALUE "/sys/class/backlight/backlight/brightness"
//int backlight(int brightness)
//{
//    if (brightness < 0) {
//        brightness = 0;
//    } else if (brightness > 100) {
//        brightness = 100;
//    }
//    brightness = (int)(brightness * 1.0 / 100 * 8);
//    char temp[16] = {0};
//    sprintf(temp, "%d", brightness);
//    sysfs_write(BACKLIGHT_NAME_VALUE, temp, strlen(temp));
//    return 0;
//}

//#define BEEP_GPIONAME_DIR "/sys/class/gpio/gpio58/direction"
//#define BEEP_GPIONAME_VALUE "/sys/class/gpio/gpio58/value"

//int beep(int freq, int ms)
//{
//    sysfs_write(BEEP_GPIONAME_DIR, "out", 3);
//    sysfs_write(BEEP_GPIONAME_VALUE, "1", 1);//H
//    usleep(ms * 1000);
//    sysfs_write(BEEP_GPIONAME_VALUE, "0", 1);//L

//    return 0;
//}

//int com1_rts(int on)
//{
//    int fd;

//    if (on != 0) {
//        on = 1;
//    }

//    fd = open(TQ2440_GPIO, O_RDONLY);
//    if (fd < 0) {
//        perror("open gpio device");
//        return -1;
//    }
//    ioctl(fd, on, COM1_RTS);
//    close(fd);
//    return 0;
//}


//int led1(int on)
//{
//    #define LED1_GPIONAME_VALUE "/sys/class/leds/led1/brightness"
//    const char* value = (0 == on ? "0" : "1");
//    sysfs_write(LED1_GPIONAME_VALUE, value, 1);
//    return 0;
//}

//int led2(int on)
//{
//    #define LED2_GPIONAME_VALUE "/sys/class/leds/led2/brightness"
//    const char* value = (0 == on ? "0" : "1");
//    sysfs_write(LED2_GPIONAME_VALUE, value, 1);
//    return 0;
//}

//int led3(int on)
//{
//    #define LED3_GPIONAME_VALUE "/sys/class/leds/led3/brightness"
//    const char* value = (0 == on ? "0" : "1");
//    sysfs_write(LED3_GPIONAME_VALUE, value, 1);
//    return 0;
//}

//int led4(int on)
//{
//    #define LED4_GPIONAME_VALUE "/sys/class/leds/led4/brightness"
//    const char* value = (0 == on ? "0" : "1");
//    sysfs_write(LED4_GPIONAME_VALUE, value, 1);
//    return 0;
//}

//#define POWER_GPIONAME_DIR   "/sys/class/gpio/gpio112/direction"
//#define POWER_GPIONAME_VALUE  "/sys/class/gpio/gpio112/value"
//int relay(int on)
//{
//    sysfs_write(POWER_GPIONAME_DIR, "out", 3);
//    char temp[32] = { 0 };
//    sprintf(temp, "%d", on);
//    sysfs_write(POWER_GPIONAME_VALUE, temp, strlen(temp));//H
//}


//int wifi(int on)
//{
//    int fd;

//    if (on != 0) {
//        on = 1;
//    }

//    fd = open(TQ2440_GPIO, O_RDONLY);
//    if (fd < 0) {
//        perror("open gpio device");
//        return -1;
//    }
//    ioctl(fd, on, WIFI_ACTIVE);
//    close(fd);
//    return 0;
//}

//int volume(int volume_write)
//{
//    int ret = 0;
//    int HEAD_PHONE_MAX = 127;
//    int PCM_MAX = 192;
//    int headPhoneVol = 0;
//    int pcmVol = 0;

//    if (volume_write <= 0) {
//        headPhoneVol = 0;
//        pcmVol = 0;
//    } else if (volume_write > 100) {
//        headPhoneVol = HEAD_PHONE_MAX;
//        pcmVol = PCM_MAX;
//    } else {
//        headPhoneVol = (int)((volume_write * 1.0 / 100) * (HEAD_PHONE_MAX - HEAD_PHONE_MAX * 0.8)) + HEAD_PHONE_MAX * 0.8;
//        pcmVol = (int)((volume_write * 1.0 / 100) * (PCM_MAX - PCM_MAX * 0.8)) + PCM_MAX * 0.8;
//    }

//    char temp[128] = "amixer -c 0 sset '%s',0 %d,%d";
//    char cmd1[128] = { 0 };
//    char cmd2[128] = { 0 };
//    sprintf(cmd1, temp, "Headphone", headPhoneVol, headPhoneVol);
//    sprintf(cmd2, temp, "PCM", pcmVol, pcmVol);
//    system(cmd1);
//    system(cmd2);

//    return ret;
//}

//int watchdog_start(int interval)
//{
//    int fd;
//    fd = open(TQ2440_WATCHDOG, O_RDONLY);
//    if (fd < 0) {
//        perror("open watchdog device");
//        return -1;
//    }
//    if (interval > 0) {
//        if (ioctl(fd, WDIOC_SETTIMEOUT, &interval) != 0) {
//            perror("set watchdog interval");
//            return -1;
//        }
//    }
//    return fd;
//}

//int watchdog_keepalive(int fd)
//{
//    if (fd < 0) {
//        return -1;
//    }
//    if (ioctl(fd, WDIOC_KEEPALIVE, NULL) != 0) {
//        perror("keep watchdog alive");
//        return -1;
//    }
//    return 0;
//}

//int watchdog_bootstatus(int fd, int* status)
//{
//    if (fd < 0) {
//        return -1;
//    }
//    if (ioctl(fd, WDIOC_GETBOOTSTATUS, status) != 0) {
//        perror("get watchdog boot status");
//        return -1;
//    }
//    return 0;
//}

//int watchdog_stop(int fd)
//{
//    if (fd < 0) {
//        return -1;
//    }
//    write(fd, "V", 1, 0);
//    close(fd);
//    return 0;
//}

//#define WORK_GPIONAME_DIR   "/sys/class/gpio/gpio111/direction"
//#define WORK_GPIONAME_VALUE  "/sys/class/gpio/gpio111/value"
//int checkworking()
//{
//    //sysfs_write(WORK_GPIONAME_DIR, "in", 2);
//    int fd = open(WORK_GPIONAME_VALUE, O_RDONLY);
//    if (0 > fd) {
//        printf("open [%s] failed\n", WORK_GPIONAME_VALUE);
//        return -1;
//    }
//    char value[2] = { 0 };
//    read(fd, value, 1);
//    close(fd);
//    return atol(value);
//}

//#define PILOTMAP_GPIONAME_DIR   "/sys/class/gpio/gpio113/direction"
//#define PILOTMAP_GPIONAME_VALUE "/sys/class/gpio/gpio113/value"
//int pilotlamp(int on)
//{
//    const char* ON = "1";
//    const char* OFF = "0";
//    const char* status = (on > 0 ? ON : OFF);
//    sysfs_write(PILOTMAP_GPIONAME_DIR,  "out", 3);
//    sysfs_write(PILOTMAP_GPIONAME_VALUE,  status, 1);
//}


//#define TEMPERATURE_I2C_VALUE "/sys/devices/platform/ocp/44e0b000.i2c/i2c-0/0-0040/hwmon/hwmon0/temp1_input"
///**
// * @brief 获得温度值
// * @param value 保存读到的温度值
// * @return 0:获取失败;1：获取成功
// */
//int temperature(float *value)
//{
//    float temp = 0;

//    do {
//        FILE* fp = fopen(TEMPERATURE_I2C_VALUE, "r");
//        if (NULL == fp) {
//            return 0;
//        }
//        char line[128] = { 0 };
//        if (fread(line, 1, 128, fp) == 0)
//        {
//            fclose(fp);
//            return 0;
//        }
//        int tv = atoi(line);
//        temp = tv * 1.0 / 1000;
//        fclose(fp);
//    } while (0);

//    *value = temp;
//    return 1;
//}

//#define HUMIDITY_I2C_VALUE "/sys/devices/platform/ocp/44e0b000.i2c/i2c-0/0-0040/hwmon/hwmon0/humidity1_input"
///**
// * @brief 获取湿度
// * @param value 保存读到的湿度值
// * @return 0:获取失败;1：获取成功
// */
//int humidity(float *value)
//{
//    float temp = 0;

//    do {
//        FILE* fp = fopen(HUMIDITY_I2C_VALUE, "r");
//        if (NULL == fp) {
//            return 0;
//        }
//        char line[128] = { 0 };
//        if (fread(line, 1, 128, fp) == 0)
//        {
//            fclose(fp);
//            return 0;
//        }
//        int tv = atoi(line);
//        temp = tv * 1.0 / 1000;
//        fclose(fp);
//    } while (0);

//    *value = temp;
//    return 1;
//}

//#define RESET_FACTORY_CONFIG "/sys/class/gpio/gpio109/value"
//int needReset()
//{
//    int status = -1;
//    do {
//        FILE* fp = fopen(RESET_FACTORY_CONFIG, "r");
//        if (NULL == fp) {
//            break;
//        }
//        char line[128] = { 0 };
//        fread(line, 1, 128, fp);
//        status = atoi(line); /// 如果为0，说明需要回复出厂设置
//        fclose(fp);
//    } while (0);

//    return status;
//}

