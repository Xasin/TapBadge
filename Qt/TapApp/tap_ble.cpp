#include "tap_ble.h"

Tap_BLE::Tap_BLE(QObject *parent) :
	QObject(parent),
	ble_handler(this),
	batteryPercent(0), batteryMV(0),
	deviceWhoIs(0), whoIs(0)
{
	connect(&ble_handler, &BLE_Handler::properties_updated,
			  this, &Tap_BLE::onProperties_updated);
}

BLE_Handler * Tap_BLE::getHandler() {
	return &(this->ble_handler);
}

int Tap_BLE::getBatteryPercent() {
	return this->batteryPercent;
}
int Tap_BLE::getBatteryMV() {
	return int(this->batteryMV);
}

int Tap_BLE::getWhoIs() {
	return whoIs;
}
void Tap_BLE::setWhoIs(int whoIs) {
	if(whoIs < 0 || whoIs > 3)
		return;
	if(whoIs == this->whoIs)
		return;

	this->whoIs = whoIs;
	QByteArray data; data.push_back(char(whoIs));
	ble_handler.write_data(QBluetoothUuid(quint32(0x1)), data);
	emit whoIsChanged();
}

void Tap_BLE::onProperties_updated() {
	auto data = ble_handler.get_data(QBluetoothUuid(quint32(0x2a1a)));
	if(data != nullptr)
		batteryMV = *(reinterpret_cast<uint16_t*>(data->data()));

	data = ble_handler.get_data(QBluetoothUuid(quint32(0x2a19)));
	if(data != nullptr)
		batteryPercent = *(reinterpret_cast<uint8_t*>(data->data()));

	data = ble_handler.get_data(QBluetoothUuid(quint32(1)));
	if(data != nullptr) {
		int newWhois = *(reinterpret_cast<char*>(data->data()));

		if((newWhois != deviceWhoIs)) {
			deviceWhoIs = newWhois;
			if(newWhois != whoIs) {
				whoIs = newWhois;
				emit whoIsChanged();
			}
		}
	}

	emit deviceDataUpdated();
	qDebug()<<"Battery MV is:"<<batteryMV;
}
