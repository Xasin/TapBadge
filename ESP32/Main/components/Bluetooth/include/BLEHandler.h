/*
 * BLEHandler.h
 *
 *  Created on: 8 Oct 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BLUETOOTH_BLEHANDLER_H_
#define COMPONENTS_BLUETOOTH_BLEHANDLER_H_

#include "assert.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_system.h"

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"

#include "Service.h"

namespace Peripheral {

using namespace Bluetooth;

class BLE_Handler {
private:
	static BLE_Handler *masterHandler;

	static void GAP_Callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
	static void GATTs_Callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

	esp_bd_addr_t connected_device;

	esp_ble_adv_data_t GAP_param;
	esp_ble_adv_data_t GAP_param_rsp;

	esp_gatt_if_t GATT_if;

	std::vector<Service *> services;
	uint8_t service_counter;

	void process_GAP(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
	void process_GATTs(esp_gatts_cb_event_t event, esp_gatt_if_t iface, esp_ble_gatts_cb_param_t *param);

	void register_service(Service *service);

public:
	BLE_Handler();

	esp_ble_adv_data_t get_GAP_defaults();
	void set_GAP_param(esp_ble_adv_data_t params);

	void set_name(const char *name);
	void add_service(Service * newService);

	void setup_GATTs();

	void start_advertising();
};

} /* namespace Peripheral */

#endif /* COMPONENTS_BLUETOOTH_BLEHANDLER_H_ */
