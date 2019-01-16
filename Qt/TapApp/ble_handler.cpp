#include "ble_handler.h"

BLE_Handler::BLE_Handler(QObject *parent) : QObject(parent),
	connection_status(DISCONNECTED),
	resynch_tries(0),
	reconnect_timer(this),
	target_name("Tap Badge"),
	BLE_discoveryAgent(this), BLE_device(nullptr)
{
	connect(&BLE_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
			  this, [this](const QBluetoothDeviceInfo &device) {
		qDebug()<<"Found device:"<<device.name()<<" - "<<device.address();
		if(device.name() == target_name) {
			BLE_discoveryAgent.stop();
			connect_to(device);
		}
	});
}

void BLE_Handler::update_status(CONNECTION_STATUS nStatus) {
	if(nStatus == this->connection_status)
		return;

	qDebug()<<"New status set:"<<nStatus;

	this->connection_status = nStatus;
	emit connectionStatusUpdated();
}

void BLE_Handler::connect_to(const QBluetoothDeviceInfo &device) {
	qDebug()<<"Setting up device:"<<device.name();

	if(BLE_device != nullptr)
		delete BLE_device;
	BLE_device = QLowEnergyController::createCentral(device, this);

	connect(BLE_device, &QLowEnergyController::connected,
			  this, [this]() {
		update_status(CONNECTED);
		resynch_tries = 0;

		BLE_device->discoverServices();

		qDebug()<<"Device connected, discovering services...";
	});

	connect(BLE_device, &QLowEnergyController::serviceDiscovered,
			  this, [this](const QBluetoothUuid &UUID) {
		qDebug()<<"Found service:"<<UUID;

		if(UUID != QBluetoothUuid(quint32(0x58535050)))
			return;

		if(BLE_dataService != nullptr)
			delete BLE_dataService;

		qDebug()<<"Wiring up the service!";

		BLE_dataService = BLE_device->createServiceObject(UUID);

		connect(BLE_dataService, &QLowEnergyService::stateChanged,
				  this, [this](QLowEnergyService::ServiceState s) {

			if(s == QLowEnergyService::ServiceDiscovered) {
				BLE_dataChar = BLE_dataService->characteristic(QBluetoothUuid(quint32(0x58535050)));

				connect(BLE_dataService, &QLowEnergyService::characteristicChanged,
						  this, &BLE_Handler::handle_receive);

				BLE_dataService->writeDescriptor(BLE_dataChar.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration), QByteArray::fromHex("0100"));
			}
		});

		BLE_dataService->discoverDetails();
	});

	BLE_device->connectToDevice();
}

void BLE_Handler::handle_receive(QLowEnergyCharacteristic c, const QByteArray &data) {
	qDebug()<<"Receiving data from"<<c.uuid()<<"DATA:"<<data;
	if(c != BLE_dataChar)
		return;

	if(data.length() < 2)
		return;

	quint16 ID = *reinterpret_cast<const quint16*>(data.data());
	QByteArray cutData = QByteArray::fromRawData(data.data() + 2, data.length() - 2);

	qDebug()<<"BLE Received signal from"<<ID<<"DATA: "<<cutData;

	if(data_cb.count(ID) > 0)
		data_cb.find(ID).value()(cutData);

	emit data_received(ID, cutData);
}

void BLE_Handler::initiate_find(QString targetDevice) {
	qDebug()<<"Looking for device ...";

	if(targetDevice != "")
		target_name = targetDevice;

	if(connection_status != DISCONNECTED)
		BLE_device->disconnectFromDevice();

	update_status(FINDING);

	BLE_discoveryAgent.setLowEnergyDiscoveryTimeout(10000);
	BLE_discoveryAgent.start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);
}

BLE_Handler::CONNECTION_STATUS BLE_Handler::getConnectionStatus() {
	return this->connection_status;
}
int BLE_Handler::getRemainingTimer() {
	return this->reconnect_timer.remainingTime();
}

void BLE_Handler::write_data(quint16 ID, const QByteArray &data) {
	if(connection_status != CONNECTED)
		return;
	if(BLE_dataService == nullptr)
		return;

	QByteArray outData = QByteArray::fromRawData(reinterpret_cast<const char *>(&ID), 2);
	outData.append(data);

	BLE_dataService->writeCharacteristic(BLE_dataChar, outData);
}
