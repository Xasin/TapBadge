#ifndef TAP_BLE_H
#define TAP_BLE_H

#include <QObject>

#include <QtBluetooth/QtBluetooth>
#include <QtBluetooth/QBluetoothDeviceInfo>

class Tap_BLE : public QObject
{
	Q_OBJECT
private:
	QBluetoothDeviceDiscoveryAgent	ble_discoveryAgent;
	QLowEnergyController					*ble_device;

	QTimer disconnectTimer;

	QString targetName;

	enum {
		SEARCHING,
		CONNECTING,
		CONNECTED,
		IDLE,
		FAILED,
	} connectionStatus;

	unsigned char batteryPercent;
	unsigned int  batteryMV;

public:
	explicit Tap_BLE(QObject *parent = nullptr);

	Q_INVOKABLE void startDeviceSearch(QString targetName);
	Q_INVOKABLE void connectTo(const QBluetoothDeviceInfo &device);

signals:

public slots:
	void startConnecting();

	void onServicePrepCompleted(QLowEnergyService *startedService);
};

#endif // TAP_BLE_H
