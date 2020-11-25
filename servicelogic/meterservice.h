#ifndef METERSERVICE_H
#define METERSERVICE_H

#include <QObject>


struct Meter_Message_ {
  char* heatmeteraddr;
  int userid;
  int inletflowtemperature;
  int outletflowtemperature;
  int temperaturediff;
  int currentflow;
  int currentpower;
  int totalflowvolume;
};

class MeterService : public QObject
{
    Q_OBJECT
public:
    explicit MeterService(QObject *parent = nullptr);
    Meter_Message_ Meter_Message;

signals:

public slots:
};

#endif // METERSERVICE_H
