/*
 * Characteristic.h
 *
 *  Created on: 9 Oct 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BLUETOOTH_CHARACTERISTIC_H_
#define COMPONENTS_BLUETOOTH_CHARACTERISTIC_H_

#include <functional>

#include "esp_gatts_api.h"

namespace Peripheral {

class BLE_Handler;

namespace Bluetooth {

class Service;

class Characteristic {
protected:
	friend BLE_Handler;

	uint16_t attr_handle;

public:
	Service * const service;

	esp_bt_uuid_t id;
	esp_gatt_perm_t 		perm;
	esp_gatt_char_prop_t 	prop;
	esp_attr_control_t 		autoResp;

	esp_attr_value_t 		value;

	uint8_t testData;

	Characteristic(Service * headService);

	void set_uuid16(uint16_t uuid);
	void set_uuid32(uint32_t uuid);

	void can_read(bool val);
	void can_write(bool val);
	void can_notify(bool val);
	void can_indicate(bool val);

	void send_notify(void *data, uint8_t data_len, bool need_confirm);

	void notify();
	void indicate();
};

} /* namespace Bluetooth */
} /* namespace Peripheral */

#endif /* COMPONENTS_BLUETOOTH_CHARACTERISTIC_H_ */
