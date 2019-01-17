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

public:
	enum CONNECTION_STATUS {
		DISCONNECTED,
		FINDING,
		RECONNECTING,
		CONNECTED,
	};
	Q_ENUM(CONNECTION_STATUS)

private:
	CONNECTION_STATUS connection_status;

	int resynch_tries;

	QTimer	reconnect_timer;

	QString	target_name;

	QBluetoothDeviceDiscoveryAgent BLE_discoveryAgent;
	QLowEnergyController				 *BLE_device;
	QLowEnergyService					 *BLE_dataService;
	QLowEnergyCharacteristic		 BLE_dataChar;

	void update_status(CONNECTION_STATUS nStatus);

	void connect_to(const QBluetoothDeviceInfo &device);

private slots:
	void handle_receive(QLowEnergyCharacteristic c, const QByteArray &data);

public:
	QMap<quint16, std::function<void(const QByteArray &data)>> data_cb;

	explicit BLE_Handler(QObject *parent = nullptr);
	Q_INVOKABLE void initiate_find(QString targetDevice = "");

	CONNECTION_STATUS getConnectionStatus();
	int getRemainingTimer();

	void write_data(quint16, const QByteArray &data);

signals:
	void connectionStatusUpdated(CONNECTION_STATUS stat);

	void data_received(int ID, const QByteArray &rawData);

public slots:
};

#endif // BLE_HANDLER_H
