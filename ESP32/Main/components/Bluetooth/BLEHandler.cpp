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
void BLE_Handler::GATTs_Callback(esp_gatts_cb_event_t event, esp_gatt_if_t iface, esp_ble_gatts_cb_param_t *param) {
	assert(masterHandler != nullptr);
	masterHandler->process_GATTs(event, iface, param);
}

void BLE_Handler::register_service(Service *service) {
	assert(GATT_if != 0);

	esp_ble_gatts_create_service(GATT_if, &service->id, service->get_no_handles());
}

BLE_Handler::BLE_Handler(const char *name) :
		BT_status(UNINITIALIZED),
		connected_device(), connection_id(0),
		GAP_param(), GAP_param_rsp(), GATT_if(0),
		services(0), name(name) {
	assert(masterHandler == nullptr);
	masterHandler = this;

	puts("Starting BT handler.");

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();
	auto ret = esp_bt_controller_init(&bt_cfg);
	ESP_ERROR_CHECK(ret);
}

void BLE_Handler::process_GAP(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t *param) {
	switch(event) {
	case ESP_GAP_BLE_ADV_DATA_SET_COMPLETE_EVT:
		puts("GAP: Adv data set");
	break;
	case ESP_GAP_BLE_ADV_START_COMPLETE_EVT:
		puts("GAP: Adv started");
		if(BT_status == IDLE)
			BT_status = ADVERTISING;
	break;

	case ESP_GAP_BLE_AUTH_CMPL_EVT:
		puts("GAP: Authentication completed.");
	break;
	case ESP_GAP_BLE_SEC_REQ_EVT:
		puts("GAP: security requested.");
		esp_ble_gap_security_rsp(param->ble_security.ble_req.bd_addr, false);
	break;
	case ESP_GAP_BLE_PASSKEY_NOTIF_EVT:
		printf("GAP: security passkey is: %d (TODO callback)\n", param->ble_security.key_notif.passkey);
	break;
	case ESP_GAP_BLE_NC_REQ_EVT:
		puts("GAP: numerical comparison requested.");
	break;
	case ESP_GAP_BLE_ADV_STOP_COMPLETE_EVT:
		puts("GAP: Adv stopped");
		if(BT_status == ADVERTISING)
			BT_status = IDLE;
	break;
	case ESP_GAP_BLE_UPDATE_CONN_PARAMS_EVT:
		puts("GAP: Update conn params.");
	break;
	case ESP_GAP_BLE_KEY_EVT:
	break;
	default:
		printf("Unknown GAP: %d\n", (uint32_t)event);
	}
}
void BLE_Handler::process_GATTs(esp_gatts_cb_event_t event, esp_gatt_if_t iface, esp_ble_gatts_cb_param_t *param) {
	switch(event) {
	case ESP_GATTS_REG_EVT:
		printf("GATT: IF %d\n", iface);
		GATT_if = iface;
		for(auto s: services)
			register_service(s);
	break;
	case ESP_GATTS_READ_EVT: {
		auto data = param->read;
		printf("GATT: Read requested for handle %d\n", data.handle);

		for(auto s: services) {
			for(auto c: s->characteristics) {
				if(c->attr_handle == data.handle) {
					c->read_reply(data);
					return;
				}
			}
		}

		puts("GATT: No matching read handle found!");
		esp_ble_gatts_send_response(GATT_if, data.conn_id, data.trans_id, ESP_GATT_INVALID_HANDLE, nullptr);
		break;
	}
	case ESP_GATTS_WRITE_EVT: {
		auto data = param->write;
		printf("GATT: Write requested for handle: %d\n", data.handle);

		for(auto s: services) {
			for(auto c: s->characteristics) {
				if(c->attr_handle == data.handle) {
					c->handle_write(&data);
					return;
				}
			}
		}
		puts("GATT: No matching write handle found!");
		esp_ble_gatts_send_response(GATT_if, data.conn_id, data.trans_id, ESP_GATT_INVALID_HANDLE, nullptr);
	}
	break;
	case ESP_GATTS_CREATE_EVT:
		printf("GATT: Service UUID: 0x%X; handle is: %d\n", param->create.service_id.id.uuid.uuid.uuid32, param->create.service_handle);
		services[param->create.service_id.id.inst_id]->set_handle(param->create.service_handle);
	break;
	case ESP_GATTS_ADD_CHAR_EVT: {
		auto p = param->add_char;
		printf("GATT: Characteristic UUID: 0x%X; Attribute Handle: %d; Service Handle: %d; Status: %d\n", p.char_uuid.uuid.uuid32, p.attr_handle, p.service_handle, p.status);
		Bluetooth::Service *service = nullptr;
		for(auto s: services)
				if(s->handle == p.service_handle) {
					service = s;
					break;
				}
		if(service == nullptr) break;
		for(auto c: service->characteristics)
			if(c->is_uuid(p.char_uuid)) {
				c->attr_handle = p.attr_handle;
				printf("Wrote attribute handle!\n");
				break;
			}
		break;
		}
	case ESP_GATTS_START_EVT:
	break;
	case ESP_GATTS_CONNECT_EVT:
		BT_status = CONNECTED;
		puts("GATT: Client connected");

		this->stop_advertising();

		memcpy(connected_device, param->connect.remote_bda, ESP_BD_ADDR_LEN);
		connection_id = param->connect.conn_id;
		esp_ble_set_encryption(connected_device, ESP_BLE_SEC_ENCRYPT);
	break;
	case ESP_GATTS_DISCONNECT_EVT:
		BT_status = IDLE;

		puts("GATT: Client disconnected");
		this->start_advertising();
	break;
	default:
		printf("GATT: Unknown event: %d\n", (uint32_t)event);
	}
}

esp_ble_adv_data_t BLE_Handler::get_GAP_defaults() {
	esp_ble_adv_data_t o = {};
	o.set_scan_rsp = false;
	o.include_name = true;
	o.include_txpower = true;
	o.min_interval =  100 / 1.25;
	o.max_interval =  100 / 1.25;
	o.appearance   = 6<<6;
	o.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

	return o;
}
void BLE_Handler::set_GAP_param(esp_ble_adv_data_t params) {
	if(params.set_scan_rsp)
		GAP_param_rsp = params;
	else
		GAP_param = params;

	//ESP_ERROR_CHECK(esp_ble_gap_config_adv_data(&params));
}

void BLE_Handler::add_service(Service * newService) {
	newService->id.id.inst_id = services.size();
	services.push_back(newService);

	if(GATT_if != 0)
		register_service(newService);
}

void BLE_Handler::setup_GATTS() {
	if(BT_status != UNINITIALIZED)
		return;

	puts("BT: First initialisation");

	auto ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	ESP_ERROR_CHECK(ret);

	ret = esp_bluedroid_init();
	ESP_ERROR_CHECK(ret);

	ret = esp_bluedroid_enable();
	ESP_ERROR_CHECK(ret);

	ret = esp_ble_gap_register_callback(BLE_Handler::GAP_Callback);
	ESP_ERROR_CHECK(ret);
	ret = esp_ble_gatts_register_callback(BLE_Handler::GATTs_Callback);
	ESP_ERROR_CHECK(ret);

	auto authen_mode = ESP_LE_AUTH_NO_BOND;
	esp_ble_gap_set_security_param(ESP_BLE_SM_AUTHEN_REQ_MODE, &authen_mode, 1);
	auto io_mode = ESP_IO_CAP_NONE;
	esp_ble_gap_set_security_param(ESP_BLE_SM_IOCAP_MODE, &io_mode, 1);

	esp_ble_gap_config_adv_data(&GAP_param);
	if(GAP_param_rsp.set_scan_rsp)
		esp_ble_gap_config_adv_data(&GAP_param_rsp);

	vTaskDelay(10);

	ret = esp_ble_gatts_app_register(1);
	ESP_ERROR_CHECK(ret);

	vTaskDelay(10);

	BT_status = IDLE;
}

void BLE_Handler::enable() {
	setup_GATTS();

	if(BT_status != DISABLED)
		return;

	puts("BT: Re-Enabling controller");

	BT_status = STARTING;

	auto ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	ESP_ERROR_CHECK(ret);

	while(esp_bt_controller_get_status() != ESP_BT_CONTROLLER_STATUS_ENABLED)
			vTaskDelay(1);

//	ret = esp_bluedroid_enable();
//	ESP_ERROR_CHECK(ret);

	BT_status = IDLE;
}
void BLE_Handler::disable() {
	if(BT_status == DISABLED)
		return;

	disconnect();
	stop_advertising();

	puts("BT: Disabling controller");
//	auto ret = esp_bluedroid_disable();
//	ESP_ERROR_CHECK(ret);

	auto ret = esp_bt_controller_disable();
	ESP_ERROR_CHECK(ret);

	while(esp_bt_controller_get_status() == ESP_BT_CONTROLLER_STATUS_ENABLED)
		vTaskDelay(1);

	BT_status = DISABLED;
}

void BLE_Handler::disconnect() {
	return;

	if(BT_status == CONNECTED) {
		puts("BT: Disconnecting");
		esp_ble_gatts_close(GATT_if, connection_id);

		while(BT_status == CONNECTED)
			vTaskDelay(1);
	}
}

void BLE_Handler::start_advertising() {
	enable();
	if(BT_status == ADVERTISING)
		return;

	puts("BT: Start advertising");

	esp_ble_gap_config_adv_data(&GAP_param);
	if(GAP_param_rsp.set_scan_rsp)
		esp_ble_gap_config_adv_data(&GAP_param_rsp);

	esp_ble_gap_set_device_name(name);

	esp_ble_adv_params_t params = {};
	params.adv_int_min = GAP_param.min_interval*2;
	params.adv_int_max = GAP_param.max_interval*2;

	params.adv_type = ADV_TYPE_IND;

	params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
	params.channel_map = ADV_CHNL_ALL;
	params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

	ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&params));
}
void BLE_Handler::stop_advertising() {
	if(BT_status == ADVERTISING) {
		puts("BT: Stop advertising");
		esp_ble_gap_stop_advertising();
		while(BT_status == ADVERTISING)
			vTaskDelay(1);
	}
}

} /* namespace Peripheral */
