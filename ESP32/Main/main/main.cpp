#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "esp_pm.h"
#include "esp32/pm.h"
#include "nvs_flash.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"

#include "driver/ledc.h"
#include "driver/touch_pad.h"

#include "Control.h"
#include "NeoController.h"
#include "NotifyHandler.h"

#include "BLEHandler.h"

#include <string.h>

using namespace Peripheral;

Peripheral::NeoController rgb  = Peripheral::NeoController(GPIO_NUM_14, RMT_CHANNEL_0, 16);
Peripheral::NotifyHandler note = Peripheral::NotifyHandler(&rgb);

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void testTask(void * params) {
	vTaskDelay(500/portTICK_PERIOD_MS);
	NotifyHandler::PatternElement testPattern[] = {
			{Material::RED, 500000},
			{Material::RED, 500000},
			{Material::GREEN, 200000},
			{Material::GREEN, 500000},
			{Material::BLUE,  200000},
			{Material::BLUE, 500000},
			{0, 500000}
	};

	while(true) {
		vTaskDelay(10000 / portTICK_PERIOD_MS);
		note.flash(testPattern, 7);
	}
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    //ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    //ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    //ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {};
    strcpy(reinterpret_cast<char*>(&sta_config.sta.ssid), "TP_LINK_####");
    //ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    //ESP_ERROR_CHECK( esp_wifi_start() );
    //ESP_ERROR_CHECK( esp_wifi_connect() );

    Touch::Control testPad = Touch::Control(TOUCH_PAD_NUM0);

    esp_pm_config_esp32_t power_config = {};
    power_config.max_freq_mhz = 240;
	power_config.min_freq_mhz = 20;
	power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);

    //esp_sleep_enable_timer_wakeup(3000000);

    //gpio_set_direction(2, GPIO_MODE_OUTPUT);
    //rtc_gpio_hold_en(GPIO_NUM_2);

    Peripheral::BLE_Handler tHandler = Peripheral::BLE_Handler();

    uint32_t colors[] = {Material::RED, Material::PINK, Material::PURPLE, Material::DEEP_PURPLE, Material::INDIGO,
							 Material::BLUE, Material::CYAN, Material::GREEN, Material::LIME, Material::YELLOW, Material::AMBER, Material::ORANGE, Material::DEEP_ORANGE};

    for(uint8_t i=0; i<12; i++) {
		rgb.fill(colors[i]);
		rgb.swipeTransition(300000, i%2);
    }
    rgb.clear();

    NotifyHandler::PatternElement xasinPattern[] = {
    		{Color(Material::RED, 80), 50000},
			{Color(Material::RED, 30), 60000},
			{Color(Material::CYAN, 100), 60000},
			{0, 500000}};

    NotifyHandler::PatternElement neiraPattern[] = {
    		{Color(Material::YELLOW, 80), 100000},
			{Color(Material::BLUE,   40), 100000},
			{Color(Material::YELLOW, 120), 100000},
    		{0, 500000}};

    NotifyHandler::PatternElement meshPattern[] = {
    		{Color(Material::GREEN, 70), 60000},
			{Color(Material::PURPLE, 120), 50000},
			{0,  50000},
			{Color(Material::PURPLE, 120), 50000},
			{0, 500000}};

    while (true) {
    	note.flash(xasinPattern, 4);

    	vTaskDelay(4000 / portTICK_PERIOD_MS);

    	note.flash(neiraPattern, 4);

    	vTaskDelay(4000);

    	note.flash(meshPattern, 5);

    	vTaskDelay(4000);
    }
}

