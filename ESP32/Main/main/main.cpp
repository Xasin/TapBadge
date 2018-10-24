
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

#include "driver/ledc.h"
#include "driver/touch_pad.h"

#include "Control.h"
#include "MorseHandle.h"

#include "NeoController.h"
#include "NotifyHandler.h"

#include "BLEHandler.h"

#include "peripheral/batman.h"

#include <string.h>

using namespace Peripheral;

Peripheral::BLE_Handler *ble_handler;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void setup_bt() {
    ble_handler = new Peripheral::BLE_Handler("Tap Badge");

    ble_handler->set_GAP_param(ble_handler->get_GAP_defaults());

    vTaskDelay(10);
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    esp_timer_init();

    esp_pm_config_esp32_t power_config = {};
    power_config.max_freq_mhz = 80;
	power_config.min_freq_mhz = 20;
	//power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    setup_bt();

    uint32_t colors[] = {Material::RED, Material::PINK, Material::PURPLE, Material::DEEP_PURPLE, Material::INDIGO,
							 Material::BLUE, Material::CYAN, Material::GREEN, Material::LIME, Material::YELLOW, Material::AMBER, Material::ORANGE, Material::DEEP_ORANGE};

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

    while(true) {
		ble_handler->start_advertising();
		vTaskDelay(20000);
		ble_handler->disable();
		vTaskDelay(5000);
    }
}

