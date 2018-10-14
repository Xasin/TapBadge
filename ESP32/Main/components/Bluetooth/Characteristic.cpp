/*
 * Characteristic.cpp
 *
 *  Created on: 9 Oct 2018
 *      Author: xasin
 */

#include "Characteristic.h"
#include "Service.h"

namespace Peripheral {
namespace Bluetooth {

Characteristic::Characteristic(Service * service) :
		attr_handle(0),
		service(service),
		id(), perm(), prop(), autoResp(), value() {

	can_read(true);
	autoResp.auto_rsp = 1;

	testData = 69;

	value.attr_len = 1;
	value.attr_max_len = 1;
	value.attr_value = &testData;
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

void Characteristic::set_uuid16(uint16_t uuid) {
	id.len = ESP_UUID_LEN_16;
	id.uuid.uuid16 = uuid;
}
void Characteristic::set_uuid32(uint32_t uuid) {
	id.len = ESP_UUID_LEN_32;
	id.uuid.uuid32 = uuid;
}

} /* namespace Bluetooth */
} /* namespace Peripheral */
