
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

#include "services/BatteryService.h"

#include "peripheral/batman.h"

#include <string.h>

using namespace Peripheral;

Peripheral::NeoController rgb  = Peripheral::NeoController(GPIO_NUM_5, RMT_CHANNEL_0, 1);
Peripheral::NotifyHandler note = Peripheral::NotifyHandler(&rgb);

Peripheral::MorseHandle morse = Peripheral::MorseHandle(120);
Touch::Control *testPad;

Peripheral::Batman *battery;

Peripheral::BLE_Handler *ble_handler;
Peripheral::Bluetooth::BatteryService *batteryC;

uint16_t batLvl;

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void testTask(void * params) {
	vTaskDelay(500/portTICK_PERIOD_MS);
	NotifyHandler::PatternElement testPattern[] = {
			{Material::RED, 50000},
			{Material::RED, 50000},
			{Material::GREEN, 20000},
			{Material::GREEN, 50000},
			{Material::BLUE,  20000},
			{Material::BLUE, 50000},
			{0, 50000}
	};

	while(true) {
		for(uint8_t i=40; i!=0; i--) {
	    	printf("Bat. lvl: %4d | Touch: %1d\n", batLvl = battery->read(), testPad->read_raw());
	    	vTaskDelay(3000);
		}
		//vTaskDelay(120000 / portTICK_PERIOD_MS);
		note.flash(testPattern, 7);
	}
}

void setup_bt() {
    ble_handler = new Peripheral::BLE_Handler("Tap Badge");
    batteryC = new Peripheral::Bluetooth::BatteryService(ble_handler, 3650, 4000);

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
	power_config.min_freq_mhz = 80;
	power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    setup_bt();

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);

    testPad = new Touch::Control(TOUCH_PAD_NUM0);
   testPad->charDetectHandle = morse.getDecodeHandle();

    battery = new Peripheral::Batman(ADC2_GPIO2_CHANNEL);


    uint32_t colors[] = {Material::RED, Material::PINK, Material::PURPLE, Material::DEEP_PURPLE, Material::INDIGO,
							 Material::BLUE, Material::CYAN, Material::GREEN, Material::LIME, Material::YELLOW, Material::AMBER, Material::ORANGE, Material::DEEP_ORANGE};

    for(uint8_t i=0; i<12; i++) {
		rgb.fill(colors[i]);
		rgb.fadeTransition(100000);
		vTaskDelay(200/portTICK_PERIOD_MS);
    }
    rgb.clear();
    rgb.apply();

    volatile uint8_t whoIs = 0;

    morse.word_callback = [&whoIs](std::string &word) {
    	if(word == "!off") {
    		rgb.fill(0); rgb.apply(); rgb.update();
    		ble_handler->disable();

    		esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    		esp_deep_sleep_start();
    	}

    	if(word == "!b")
    		ble_handler->start_advertising(10000);

    	if(word == "x")
    		whoIs = 1;
    	else if(word == "n")
    		whoIs = 2;
    	else if(word == "m")
    		whoIs = 3;
    	else if(word == "off")
    		whoIs = 0;
    	else
    		return;

    	rgb.apply();
    	rgb.update();
    };

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

    ble_handler->start_advertising();

    while (true) {
    	batLvl = battery->read();
    	batteryC->setBatLevel(batLvl);

    	switch(whoIs) {
    	case 1:
    		note.flash(xasinPattern, 4);
    		break;
    	case 2:
    		note.flash(neiraPattern, 4);
    		break;
    	case 3:
    		note.flash(meshPattern, 5);
    		break;
    	default: break;

    	}
    	vTaskDelay(3000 / portTICK_PERIOD_MS);
    }
}

