#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <QObject>

class BLE_Handler : public QObject
{
    Q_OBJECT
public:
    explicit BLE_Handler(QObject *parent = nullptr);

signals:

public slots:
};

#endif // BLE_HANDLER_H