/*
 * Characteristic.cpp
 *
 *  Created on: 9 Oct 2018
 *      Author: xasin
 */

#include "Characteristic.h"
#include "Service.h"
#include "BLEHandler.h"

namespace Peripheral {
namespace Bluetooth {

Characteristic::Characteristic(Service * service) :
		attr_handle(0),
		service(service),
		id(), perm(), prop(), autoResp(), value() {

	can_read(true);
	can_notify(true);

	autoResp.auto_rsp = 0;

	testData = 69;

	value.attr_len = 1;
	value.attr_max_len = 1;
	value.attr_value = &testData;
}

void Characteristic::set_uuid16(uint16_t uuid) {
	id.len = ESP_UUID_LEN_16;
	id.uuid.uuid16 = uuid;
}
void Characteristic::set_uuid32(uint32_t uuid) {
	id.len = ESP_UUID_LEN_32;
	id.uuid.uuid32 = uuid;
}


void Characteristic::can_read(bool val) {
	if(val) {
		perm |= ESP_GATT_PERM_READ;
		prop |= ESP_GATT_CHAR_PROP_BIT_READ;
	}
	else {
		perm &= ~ESP_GATT_PERM_READ;
		prop &= ~ESP_GATT_CHAR_PROP_BIT_READ;
	}
}
void Characteristic::can_write(bool val) {
	if(val)
		perm |= ESP_GATT_PERM_WRITE;
	else
		perm &= ~ESP_GATT_PERM_WRITE;
}
void Characteristic::can_indicate(bool val) {
	auto b = ESP_GATT_CHAR_PROP_BIT_INDICATE;
	if(val)
		prop |= b;
	else
		prop &= ~b;
}
void Characteristic::can_notify(bool val) {
	auto b = ESP_GATT_CHAR_PROP_BIT_NOTIFY;
	if(val)
		prop |= b;
	else
		prop &= ~b;
}

void Characteristic::send_notify(void *data, uint8_t data_len, bool need_confirm) {
	auto h = service->handler;
	esp_ble_gatts_send_indicate(h->GATT_if, h->connection_id, attr_handle, data_len, reinterpret_cast<uint8_t *>(data), need_confirm);
}

void Characteristic::notify() {
	send_notify(value.attr_value, value.attr_len, false);
}
void Characteristic::indicate() {
	send_notify(value.attr_value, value.attr_len, true);
}

bool Characteristic::is_uuid(esp_bt_uuid_t &tId) {
	if(tId.len != id.len)
		return false;

	return memcmp(&id.uuid, &tId.uuid, id.len) == 0;
}

} /* namespace Bluetooth */
} /* namespace Peripheral */
