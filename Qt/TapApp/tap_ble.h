#ifndef TAP_BLE_H
#define TAP_BLE_H

#include <QObject>

#include "ble_handler.h"

class Tap_BLE : public QObject
{
	Q_OBJECT

	Q_PROPERTY(BLE_Handler *ble_handler READ getHandler)

private:

	BLE_Handler ble_handler;

	unsigned char batteryPercent;
	unsigned int  batteryMV;

public:
	explicit Tap_BLE(QObject *parent = nullptr);

	BLE_Handler *getHandler();

signals:

public slots:
	void onProperties_updated();
};

#endif // TAP_BLE_H
