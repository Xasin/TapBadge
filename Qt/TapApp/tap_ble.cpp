#include "tap_ble.h"

Tap_BLE::Tap_BLE(QObject *parent) :
	QObject(parent),
	ble_discoveryAgent(this), disconnectTimer(),
	targetName(""), connectionStatus(SEARCHING)
{
	connect(&ble_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
			  this, [this](const QBluetoothDeviceInfo &device) {
		qDebug()<<"Found device:"<<device.name()<<" UUID:"<<device.deviceUuid();

		if(device.name() == targetName) {
			ble_discoveryAgent.stop();
			this->connectTo(device);
		}
	});

	connect(&disconnectTimer, &QTimer::timeout, this,
			  [this]() {
		ble_device->disconnectFromDevice();
		if(connectionStatus == CONNECTED)
			connectionStatus = IDLE;
		else
			connectionStatus = FAILED;
	});
}

void Tap_BLE::startDeviceSearch(QString targetName) {
	this->targetName = targetName;

	ble_discoveryAgent.setLowEnergyDiscoveryTimeout(10000);
	ble_discoveryAgent.start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

void Tap_BLE::connectTo(const QBluetoothDeviceInfo &device) {
	ble_device = QLowEnergyController::createCentral(device, this);

	connect(ble_device, &QLowEnergyController::connected, this,
			  [this]() {
		qDebug()<<"Managed to connect to device! Discovering services.";

		this->connectionStatus = CONNECTED;
		ble_device->discoverServices();
		disconnectTimer.start(3000);
	});

	connect(ble_device, &QLowEnergyController::serviceDiscovered, this,
			  [this](const QBluetoothUuid &uuid) {
		auto service = ble_device->createServiceObject(uuid);

		connect(service, &QLowEnergyService::stateChanged, this,
				  [this, service](QLowEnergyService::ServiceState s) {
			if(s == QLowEnergyService::ServiceDiscovered)
				onServicePrepCompleted(service);
		});

		service->discoverDetails();
	});
	qDebug()<<"Trying to connect to"<<device.name();
}

void Tap_BLE::startConnecting() {
	ble_device->connectToDevice();
	this->connectionStatus = CONNECTING;

	disconnectTimer.start(5000);
}

void Tap_BLE::onServicePrepCompleted(QLowEnergyService *service) {
	qDebug()<<"Service fully discovered:"<<service->serviceUuid();

	if(service->serviceUuid() == QBluetoothUuid(quint16(0x180F))) {
		batteryPercent = *reinterpret_cast<unsigned char*>(service->characteristic(QBluetoothUuid(quint16(0x2A19))).value().data());
		batteryMV = *reinterpret_cast<unsigned int *>(service->characteristic(QBluetoothUuid(quint32(0x2A1A))).value().data());

		qDebug()<<"Battery is at: "<<batteryPercent<<"% ("<<batteryMV<<"mv)";
	}
}
