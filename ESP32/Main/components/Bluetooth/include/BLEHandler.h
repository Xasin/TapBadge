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

namespace Peripheral {

class BLE_Handler {
private:
	static BLE_Handler *masterHandler;

	static void GAP_Callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
	static void GATTs_Callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param);

	void process_GAP(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param);
	void process_GATTs(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t *param);

public:
	BLE_Handler();
};

} /* namespace Peripheral */

#endif /* COMPONENTS_BLUETOOTH_BLEHANDLER_H_ */
