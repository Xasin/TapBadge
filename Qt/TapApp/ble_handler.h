#ifndef BLE_HANDLER_H
#define BLE_HANDLER_H

#include <QObject>
#include <QMap>
#include <QByteArray>

#include <QTimer>

#include <QtBluetooth/QtBluetooth>
#include <QtBluetooth/QBluetoothDeviceInfo>

class BLE_Handler : public QObject
{
	Q_OBJECT

	Q_PROPERTY(CONNECTION_STATUS connection READ getConnectionStatus NOTIFY connectionStatusUpdated)
	Q_PROPERTY(int remainingReconnectTime READ getRemainingTimer NOTIFY timerUpdated)

public:
	enum CONNECTION_STATUS {
		DISCONNECTED,
		FINDING,
		ATTEMPTING_RESYNCH,
		SYNCHED_PAUSE,
		SYNCHED_CONNECTING,
		SYNCHED_CONNECTED,
	};
private:
	CONNECTION_STATUS connection_status;
	Q_ENUM(CONNECTION_STATUS)

	int connection_interval;
	int resynch_tries;

	QTimer	reconnect_timer;
	QTimer	disconnect_timer;

	QString	target_name;

	QBluetoothDeviceDiscoveryAgent BLE_discoveryAgent;
	QLowEnergyController				 *BLE_device;

	QMap<QBluetoothUuid, QByteArray> raw_data_map;
	QMap<QBluetoothUuid, QByteArray> raw_write_map;

	void update_status(CONNECTION_STATUS nStatus);

	void initial_connect(const QBluetoothDeviceInfo &device);
	void initiate_reconnect();

private slots:
	void timer_task();

public:
	explicit BLE_Handler(QObject *parent = nullptr);
	void initiate_find(QString targetDevice = "");

	CONNECTION_STATUS getConnectionStatus();
	int getRemainingTimer();

	QByteArray *get_data(QBluetoothUuid key);
	void write_data(QBluetoothUuid key, QByteArray &data);

signals:
	void connectionStatusUpdated();
	void timerUpdated();

	void properties_updated();

public slots:
};

#endif // BLE_HANDLER_H
