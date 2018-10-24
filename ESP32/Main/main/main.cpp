
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_pm.h"
#include "esp_timer.h"
#include "esp32/pm.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include <string.h>

#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_bt_defs.h"
#include "esp_bt_device.h"

#include "esp_gap_ble_api.h"
#include "esp_gatts_api.h"


esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    esp_timer_init();

    esp_pm_config_esp32_t power_config = {};
    power_config.max_freq_mhz = 80;
	power_config.min_freq_mhz = 80;
	//power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    //setup_bt();

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

	esp_bt_controller_init(&bt_cfg);
    esp_bt_controller_enable(ESP_BT_MODE_BLE);

    esp_bluedroid_init();
    esp_bluedroid_enable();

	esp_ble_adv_data_t o = {};
	o.set_scan_rsp = false;
	o.include_name = true;
	o.include_txpower = true;
	o.min_interval =  100 / 1.25;
	o.max_interval =  100 / 1.25;
	o.appearance   = 6<<6;
	o.flag = (ESP_BLE_ADV_FLAG_GEN_DISC | ESP_BLE_ADV_FLAG_BREDR_NOT_SPT);

	esp_ble_gap_config_adv_data(&o);

    esp_ble_gatts_app_register(1);

    esp_ble_adv_params_t params = {};
	params.adv_int_min = o.min_interval*2;
	params.adv_int_max = o.max_interval*2;

	params.adv_type = ADV_TYPE_IND;

	params.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
	params.channel_map = ADV_CHNL_ALL;
	params.adv_filter_policy = ADV_FILTER_ALLOW_SCAN_ANY_CON_ANY;

	vTaskDelay(100);

	esp_ble_gap_set_device_name("ESP TEST");

	ESP_ERROR_CHECK(esp_ble_gap_start_advertising(&params));

	esp_ble_gap_set_device_name("ESP TEST");

	puts("Connect now - then disconnect.");

	vTaskDelay(20000);

    while(true) {
		puts("Stopping BT");
		esp_ble_gap_stop_advertising();
		vTaskDelay(1);

		esp_bt_controller_disable();
    	vTaskDelay(5000);

		puts("Starting BT");
		esp_bt_controller_enable(ESP_BT_MODE_BLE);
		vTaskDelay(1);
		esp_ble_gap_start_advertising(&params);
		vTaskDelay(20000);
    }
}

