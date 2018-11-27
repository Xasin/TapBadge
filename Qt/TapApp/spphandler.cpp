#include "spphandler.h"

SPPHandler::SPPHandler(QObject *parent) : QObject(parent),
	targetName(""), connection_status(DISCONNECTED),
	SPP_device_discoverer(this), SPP_service_discoverer(this), SPP_target_addr(),
	SPP_socket(QBluetoothServiceInfo::RfcommProtocol, this),
	values()
{
	connect(&SPP_socket, &QBluetoothSocket::readyRead, this,
			  [this]() {
		QByteArray currentData;
		while((currentData = QByteArray::fromBase64(SPP_socket.readLine())).size() > 2) {
			uint16_t id = *reinterpret_cast<uint16_t *>(currentData.data());
			currentData.remove(0, 2);

			qDebug()<<"Received data ID:"<<id<<"rest:"<<currentData;
		}
	});

	connect(&SPP_device_discoverer, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered, this,
			  [this](const QBluetoothDeviceInfo &device) {
		if(device.name() == targetName) {
			SPP_device_discoverer.stop();
			qDebug()<<"Found our device!";

			this->SPP_target_addr = device.address();
			//raw_start_connect();
		}
	});

	connect(&SPP_service_discoverer, &QBluetoothServiceDiscoveryAgent::serviceDiscovered, this,
			  [this](const QBluetoothServiceInfo &service) {
		qDebug()<<"Found a service in:"<<service.device().name();
		if(service.device().name() == targetName) {
			SPP_service_discoverer.stop();

			qDebug()<<"Found service!"<<service.serviceName();
			if(service.serviceName() == "Serial Port Profile")
				SPP_socket.connectToService(service);
		}
	});
}

void SPPHandler::raw_start_connect() {
	SPP_socket.connectToService(SPP_target_addr, 2);
}

bool SPPHandler::write_raw(uint16_t id, const QByteArray &data) {
	if(!SPP_socket.isOpen())
		return false;

	QByteArray outData;
	outData.append(reinterpret_cast<char *>(&id), 2);
	outData.append(data);

	SPP_socket.write(outData.toBase64());

	return true;
}

void SPPHandler::find() {
	SPP_service_discoverer.start(QBluetoothServiceDiscoveryAgent::FullDiscovery);
}
void SPPHandler::find(const QString &targetName) {
	this->targetName = targetName;
	find();
}
