#ifndef SPPHANDLER_H
#define SPPHANDLER_H

#include <QObject>

#include <QtBluetooth/QtBluetooth>

class SPPValue;

class SPPHandler : public QObject
{
	Q_OBJECT
public:
	enum CONNECTION_STATUS {
		DISCONNECTED,
		FINDING,
		RECONNECTING,
		CONNECTED,
	};
	Q_ENUM(CONNECTION_STATUS)

private:
	QString targetName;

	CONNECTION_STATUS connection_status;

	QBluetoothDeviceDiscoveryAgent  SPP_device_discoverer;
	QBluetoothAddress SPP_target_addr;

	QBluetoothSocket SPP_socket;

	void raw_start_connect();
	void update_connection(CONNECTION_STATUS newStatus);

protected:
	friend SPPValue;

	QMap<uint16_t, SPPValue *> values;
	bool write_raw(uint16_t id, const QByteArray &data);

public:
	SPPHandler(QObject *parent = nullptr);

	void find(const QString &targetName);

signals:
	void connectionStatusChanged();
};

#endif // SPPHANDLER_H
