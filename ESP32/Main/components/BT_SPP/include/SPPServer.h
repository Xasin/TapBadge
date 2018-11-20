/*
 * SPPHandler.h
 *
 *  Created on: 17 Nov 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BT_SPP_SPPSERVER_H_
#define COMPONENTS_BT_SPP_SPPSERVER_H_

#include <map>
#include <functional>
#include <string>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_gap_bt_api.h"
#include "esp_bt_device.h"
#include "esp_spp_api.h"

#include "mbedtls/base64.h"

#include "SPPValue.h"

namespace Bluetooth {

class SPP_Server {
	static SPP_Server* headServer;

	static void static_SPP_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
	static void static_GAP_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

	uint32_t SPP_handle;

	std::map<uint16_t, SPPValue*> values;

	std::string device_name;

	std::string inputBuffer;

	void write_packet(uint16_t id, const void * data, const size_t length);
	void decode_packet(const void *rawData, size_t length);

public:
	void SPP_callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param);
	void GAP_callback(esp_bt_gap_cb_event_t event, esp_bt_gap_cb_param_t *param);

	SPP_Server(int dummy);
};

} /* namespace Bluetooth */

#endif /* COMPONENTS_BT_SPP_SPPSERVER_H_ */
