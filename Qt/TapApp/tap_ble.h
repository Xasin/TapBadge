#ifndef TAP_BLE_H
#define TAP_BLE_H

#include <QObject>

#include <QtMqtt/QtMqtt>

#include "ble_handler.h"

class Tap_BLE : public QObject
{
	Q_OBJECT

	Q_PROPERTY(BLE_Handler *connHandler READ getHandler)

	Q_PROPERTY(int batteryMV		READ getBatteryMV	NOTIFY deviceDataUpdated)
	Q_PROPERTY(int batteryPercent READ getBatteryPercent NOTIFY deviceDataUpdated)

	Q_PROPERTY(int whoIs				READ getWhoIs WRITE setWhoIs NOTIFY whoIsChanged)

private:
	BLE_Handler connHandler;

	QMqttClient			mqtt_client;
	QMqttSubscription *mqtt_sub_whoIs;
	QTimer				mqtt_reconnet;

	unsigned char batteryPercent;
	unsigned int  batteryMV;

	int deviceWhoIs;
	int whoIs;

	void m_pub(QString topic, const void *data_ptr, int length, bool retain = false);

public:
	explicit Tap_BLE(QObject *parent = nullptr);

	BLE_Handler *getHandler();

	int getBatteryPercent();
	int getBatteryMV();

	int  getWhoIs();
	void setWhoIs(int whoIs);

signals:
	void deviceDataUpdated();
	void whoIsChanged();

public slots:
};

#endif // TAP_BLE_H
