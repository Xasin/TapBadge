/*
 * BLEHandler.cpp
 *
 *  Created on: 8 Oct 2018
 *      Author: xasin
 */

#include "BLEHandler.h"

#include "esp_bt.h"

namespace Peripheral {

BLE_Handler *BLE_Handler::masterHandler = nullptr;

void BLE_Handler::GAP_Callback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	assert(masterHandler != nullptr);
	masterHandler->process_GAP(event, param);
}
void BLE_Handler::GATTs_Callback(esp_gatts_cb_event_t event, esp_gatt_if_t gatts_if, esp_ble_gatts_cb_param_t *param) {
	assert(masterHandler != nullptr);
	masterHandler->process_GATTs(event, param);
}

BLE_Handler::BLE_Handler() {
	assert(masterHandler == nullptr);
	masterHandler = this;

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	esp_bt_controller_init(&bt_cfg);

	esp_bt_controller_enable(ESP_BT_MODE_BLE);

	esp_ble_gap_register_callback(BLE_Handler::GAP_Callback);
	esp_ble_gatts_register_callback(BLE_Handler::GATTs_Callback);
}

void BLE_Handler::process_GAP(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
}
void BLE_Handler::process_GATTs(esp_gatts_cb_event_t event, esp_ble_gatts_cb_param_t *param) {
}

} /* namespace Peripheral */
