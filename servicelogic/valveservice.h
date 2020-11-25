#ifndef VALVESERVICE_H
#define VALVESERVICE_H

#include <QObject>

class ValveService : public QObject
{
    Q_OBJECT
public:
    explicit ValveService(QObject *parent = nullptr);

signals:

public slots:
};

#endif // VALVESERVICE_H