#ifndef SUBDEVICESERVICE_H
#define SUBDEVICESERVICE_H
//
#include <QObject>

class SubdeviceService : public QObject
{
    Q_OBJECT
public:
    explicit SubdeviceService(QObject *parent = nullptr);

signals:

};

#endif // SUBDEVICESERVICE_H
