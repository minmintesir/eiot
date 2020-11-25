#ifndef PANELSERVICE_H
#define PANELSERVICE_H

#include <QObject>

class PanelService : public QObject
{
    Q_OBJECT
public:
    explicit PanelService(QObject *parent = nullptr);

signals:

public slots:
};

#endif // PANELSERVICE_H