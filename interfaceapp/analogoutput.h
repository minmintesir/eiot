#ifndef ANALOGOUTPUT_H
#define ANALOGOUTPUT_H

#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#include <QObject>
#include <QTimer>
#include <QThread>

#define IMX6_ANALOGOUTPUT     "/dev/ad5667r"

//第一路模拟量输出
#define ANALOGOUTPUT_CH1       0
//第二路模拟量输出
#define ANALOGOUTPUT_CH2       1

class AnalogOutput : public QObject
{
    Q_OBJECT
public:
    explicit AnalogOutput(QObject *parent = nullptr);
    int analogoutput1_change(unsigned char status);

private:
    int analogoutput_write(unsigned char channel,unsigned char quantity);
    int analogoutput_init();
    int m_fd;

signals:

};

#endif // ANALOGOUTPUT_H
