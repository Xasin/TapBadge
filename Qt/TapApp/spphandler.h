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
	QBluetoothDeviceDiscoveryAgent SPP_discoverer;

	QList<SPPValue *> values;

public:
	SPPHandler();
};

#endif // SPPHANDLER_H
