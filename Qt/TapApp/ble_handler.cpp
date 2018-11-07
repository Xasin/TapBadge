#include "ble_handler.h"

BLE_Handler::BLE_Handler(QObject *parent) : QObject(parent),
	connection_status(DISCONNECTED),
	connection_interval(30000), resynch_tries(0),
	reconnect_timer(this), disconnect_timer(this),
	target_name("Tap Badge"),
	BLE_discoveryAgent(this), BLE_device(nullptr),
	raw_data_map(), raw_write_map()
{
	reconnect_timer.setSingleShot(true);
	connect(&reconnect_timer, &QTimer::timeout,
			  this, &BLE_Handler::timer_task);

	disconnect_timer.setSingleShot(true);
	connect(&disconnect_timer, &QTimer::timeout,
			  this, &BLE_Handler::timer_task);

	connect(&BLE_discoveryAgent, &QBluetoothDeviceDiscoveryAgent::deviceDiscovered,
			  this, [this](const QBluetoothDeviceInfo &device) {
		qDebug()<<"Found device:"<<device.name()<<" - "<<device.address();
		if(device.name() == target_name) {
			BLE_discoveryAgent.stop();
			initial_connect(device);
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

void BLE_Handler::initial_connect(const QBluetoothDeviceInfo &device) {
	qDebug()<<"Setting up device:"<<device.name();

	if(BLE_device != nullptr)
		delete BLE_device;
	BLE_device = QLowEnergyController::createCentral(device, this);

	connect(BLE_device, &QLowEnergyController::connected,
			  this, [this]() {
		update_status(SYNCHED_CONNECTED);
		resynch_tries = 0;

		BLE_device->discoverServices();
		disconnect_timer.start(2000);

		qDebug()<<"Device connected!";
	});

	connect(BLE_device, &QLowEnergyController::serviceDiscovered,
			  this, [this](const QBluetoothUuid &UUID) {
		auto service = BLE_device->createServiceObject(UUID);

		connect(service, &QLowEnergyService::stateChanged,
				  this, [this, service](QLowEnergyService::ServiceState s) {
			if(s == QLowEnergyService::ServiceDiscovered) {
				for(auto c : service->characteristics()) {
					qDebug()<<"CHARACTERISTIC "<<c.uuid()<<" written!";
					raw_data_map[c.uuid()] = c.value();
					if(raw_write_map.contains(c.uuid())) {
						service->writeCharacteristic(c, raw_write_map[c.uuid()]);
						raw_write_map.remove(c.uuid());
					}
				}
			}
		});

		service->discoverDetails();
	});

	initiate_reconnect();
}
void BLE_Handler::initiate_reconnect() {
	qDebug()<<"Initiating reconnect...";

	if(BLE_device == nullptr)
		return;

	update_status(SYNCHED_CONNECTING);
	BLE_device->connectToDevice();
	reconnect_timer.start(connection_interval);
	emit timerUpdated();
}


void BLE_Handler::timer_task() {
	qDebug()<<"Timer Thread called!";
	switch(connection_status) {
	case DISCONNECTED: break;
	case FINDING:
		update_status(DISCONNECTED);
	break;

	case SYNCHED_CONNECTING:
		update_status(ATTEMPTING_RESYNCH);
	case ATTEMPTING_RESYNCH:
		qDebug()<<"Attempting resynch...";

		if(++resynch_tries >= 3) {
			update_status(DISCONNECTED);
			qDebug()<<"Connection lost!";
		}
		else
			initiate_find();
	break;

	case SYNCHED_PAUSE:
		initiate_reconnect();
	break;
	case SYNCHED_CONNECTED:
		qDebug()<<"Finished update, disconnecting!";

		BLE_device->disconnectFromDevice();
		update_status(SYNCHED_PAUSE);
		emit properties_updated();
	break;

	}
}

void BLE_Handler::initiate_find(QString targetDevice) {
	qDebug()<<"Looking for device ...";

	if(targetDevice != "")
		target_name = targetDevice;

	if(connection_status == SYNCHED_CONNECTED) {
		BLE_device->disconnectFromDevice();
		update_status(FINDING);
	}

	if(connection_status == DISCONNECTED)
		update_status(FINDING);

	BLE_discoveryAgent.setLowEnergyDiscoveryTimeout(connection_interval);
	BLE_discoveryAgent.start(QBluetoothDeviceDiscoveryAgent::LowEnergyMethod);

	reconnect_timer.start(connection_interval);
	emit timerUpdated();
}

BLE_Handler::CONNECTION_STATUS BLE_Handler::getConnectionStatus() {
	return this->connection_status;
}
int BLE_Handler::getRemainingTimer() {
	return this->reconnect_timer.remainingTime();
}

QByteArray * BLE_Handler::get_data(QBluetoothUuid key) {
	if(raw_data_map.contains(key)) {
		return &(raw_data_map[key]);
	}

	return nullptr;
}
void BLE_Handler::write_data(QBluetoothUuid key, QByteArray &data) {
	raw_write_map.insert(key, data);
}
