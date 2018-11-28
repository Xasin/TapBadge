
#include "spphandler.h"
#include "sppvalue.h"

SPPHandler::SPPHandler(QObject *parent) : QObject(parent),
	targetName(""), connection_status(DISCONNECTED),
	SPP_device_discoverer(this), SPP_target_addr(),
	SPP_socket(QBluetoothServiceInfo::RfcommProtocol, this),
	values()
{
	connect(&SPP_socket, &QBluetoothSocket::readyRead, this,
			  [this]() {
		QByteArray currentData;

		while(SPP_socket.canReadLine()) {
			if((currentData = QByteArray::fromBase64(SPP_socket.readLine())).size() > 2) {
				uint16_t id = *reinterpret_cast<uint16_t *>(currentData.data());
				currentData.remove(0, 2);

				if(values.contains(id)) {
					values[id]->receive_data(currentData);
				}

				qDebug()<<"Received data ID:"<<id<<"rest:"<<currentData;
			}
		}
	});
	connect(&SPP_socket, &QBluetoothSocket::connected, this, [this]()
	{
		update_connection(CONNECTED);
	});
	connect(&SPP_socket, &QBluetoothSocket::disconnected, this, [this]() {
		update_connection(DISCONNECTED);
	});

	connect(&SPP_device_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
			  [this](const QBluetoothDeviceInfo &device) {
		if(device.name() == targetName) {
			SPP_device_discoverer.stop();
			qDebug()<<"Found our device!";

			this->SPP_target_addr = device.address();

			raw_start_connect();
		}
	});
}

void SPPHandler::raw_start_connect() {
	if(SPP_target_addr.isNull())
		return;

	SPP_socket.connectToService(SPP_target_addr, QBluetoothUuid::SerialPort);
	update_connection(RECONNECTING);
}
void SPPHandler::update_connection(CONNECTION_STATUS newStatus) {
	if(newStatus == connection_status)
		return;

	connection_status = newStatus;

	if(connection_status == CONNECTED)
		for(auto c : values) {
			c->on_reconnected();
		}
	emit connectionStatusChanged();
}

bool SPPHandler::write_raw(uint16_t id, const QByteArray &data) {
	if(!SPP_socket.isOpen()) {
		qDebug()<<"SPP socket closed!";
		return false;
	}

	QByteArray outData;
	outData.append(reinterpret_cast<char *>(&id), 2);
	outData.append(data);

	qDebug()<<"SPP writing"<<outData;

	SPP_socket.write(outData.toBase64().append('\n'));

	return true;
}

void SPPHandler::find(const QString &targetName) {
	update_connection(FINDING);

	this->targetName = targetName;
	SPP_device_discoverer.start();
}
