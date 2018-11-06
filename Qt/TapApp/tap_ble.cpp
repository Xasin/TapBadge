#include "tap_ble.h"

Tap_BLE::Tap_BLE(QObject *parent) :
	QObject(parent),
	ble_handler(this),
	batteryPercent(0), batteryMV(0)
{
	ble_handler.initiate_find("Tap Badge");

	connect(&ble_handler, &BLE_Handler::properties_updated,
			  this, &Tap_BLE::onProperties_updated);
}

void Tap_BLE::onProperties_updated() {
	auto data = ble_handler.get_data(QBluetoothUuid(quint32(0x2a1a)));

	if(data != nullptr)
		batteryMV = *(reinterpret_cast<uint16_t*>(data->data()));

	qDebug()<<"Battery MV is:"<<batteryMV;
}

BLE_Handler * Tap_BLE::getHandler() {
	return &(this->ble_handler);
}
