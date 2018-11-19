/*
 * SPPHandler.cpp
 *
 *  Created on: 17 Nov 2018
 *      Author: xasin
 */

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "SPPServer.h"


namespace Bluetooth {

SPP_Server* SPP_Server::headServer = nullptr;

void SPP_Server::static_GAP_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
	assert(headServer != nullptr);

	headServer->GAP_callback(event, param);
}
void SPP_Server::static_SPP_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
	assert(headServer != nullptr);

	headServer->SPP_callback(event, param);
}

void SPP_Server::write_packet(uint16_t id, const void *data, const size_t dLength) {

	std::string outString(reinterpret_cast<const char *>(&id), 2);
	outString.append(reinterpret_cast<const char *>(data), dLength);
	auto data_ptr = reinterpret_cast<const unsigned char *>(outString.data());

	size_t bLength = 0;
	mbedtls_base64_encode(nullptr, 0, &bLength, data_ptr, outString.size());

	auto outBuffer = new unsigned char[bLength +1];

	mbedtls_base64_encode(outBuffer, bLength, &bLength, data_ptr, outString.size());
	outBuffer[bLength] = '\n';
	esp_spp_write(SPP_handle, bLength, outBuffer);

	delete outBuffer;
}

SPP_Server::SPP_Server(int dummy) : SPP_handle(-1), values() {
	assert(headServer == nullptr);
	headServer = this;

	device_name = "Tap Badge";

	esp_err_t ret;

	ret = esp_bt_controller_mem_release(ESP_BT_MODE_BLE);
	ESP_ERROR_CHECK(ret);

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	ret = esp_bt_controller_init(&bt_cfg);
	ESP_ERROR_CHECK(ret);

	ret = esp_bt_controller_enable(ESP_BT_MODE_CLASSIC_BT);
	ESP_ERROR_CHECK(ret);

	ret = esp_bluedroid_init();
	ESP_ERROR_CHECK(ret);

	ret = esp_bluedroid_enable();
	ESP_ERROR_CHECK(ret);

	ret = esp_bt_gap_register_callback(static_GAP_callback);
	ESP_ERROR_CHECK(ret);
	ret = esp_spp_register_callback(static_SPP_callback);
	ESP_ERROR_CHECK(ret);

	ret = esp_spp_init(ESP_SPP_MODE_CB);
	ESP_ERROR_CHECK(ret);

	esp_bt_io_cap_t iocap = ESP_BT_IO_CAP_NONE;
	esp_bt_gap_set_security_param(ESP_BT_SP_IOCAP_MODE, &iocap, 1);

	esp_bt_pin_code_t pin = "NeXaMe";
	esp_bt_gap_set_pin(ESP_BT_PIN_TYPE_FIXED, 0, pin);

	puts("SPP should be starting...");
}

void SPP_Server::GAP_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param) {
	printf("SPP GAP evt no. %d\n", event);
}
void SPP_Server::SPP_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
	std::string boop = "Boop!";
	std::string dataStr;

	switch(event) {
	case ESP_SPP_INIT_EVT:
		puts("SPP initialised!");
		esp_bt_dev_set_device_name(device_name.data());
		esp_bt_gap_set_scan_mode(ESP_BT_SCAN_MODE_CONNECTABLE_DISCOVERABLE);

		esp_spp_start_srv(ESP_SPP_SEC_NONE, ESP_SPP_ROLE_SLAVE, 0, device_name.data());
	break;
	case ESP_SPP_START_EVT:
		puts("SPP started!");
	break;
	case ESP_SPP_SRV_OPEN_EVT:
		SPP_handle = param->start.handle;
		puts("SPP open!");
	break;
	case ESP_SPP_DATA_IND_EVT:
		dataStr = std::string(reinterpret_cast<char *>(param->data_ind.data), param->data_ind.len);
		printf(("SPP data received: " + dataStr + "\n").data());

		this->write_packet('A', boop.data(), boop.size());
	break;
	default:
		printf("SPP unknown value: %d\n", int(event));
	break;
	}
}

} /* namespace Bluetooth */
