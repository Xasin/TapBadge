
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

#include <sstream>
#include <string.h>

#define RECONNECT_TIME 10000

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
	NotifyHandler::PatternElement btStart[] = {
			{Color(Material::BLUE, 100), 100000},
			{Color(Material::BLUE, 50), 200000},
	};

	uint64_t lastAdvStart;
	while(true) {
		lastAdvStart = xTaskGetTickCount();
		ble_handler->start_advertising(2000);
		note.flash(btStart, 3);
		vTaskDelay(2000);

		if(ble_handler->client_connection_time == 0) {
			vTaskDelay(lastAdvStart + RECONNECT_TIME - xTaskGetTickCount());
		}
		else {
			uint64_t delay_time = ble_handler->client_connection_time + RECONNECT_TIME - 1000 - xTaskGetTickCount();
			ble_handler->client_connection_time = 0;
			vTaskDelay(delay_time);
		}
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

    uint8_t touchVal;
    uint8_t whoIs = 0;

    Bluetooth::Service customService(ble_handler);
    customService.set_uuid32(0x123456);

    Bluetooth::Characteristic touchChar(&customService);
    touchChar.set_uuid32(0x1);
    touchChar.set_value(&whoIs, 1, 1);
    touchChar.can_write(true);
    touchChar.write_cb = [&whoIs](Bluetooth::Characteristic::write_dataset data) {
    	whoIs = *reinterpret_cast<uint8_t *>(data.data);
    };

    std::string testData;
    Bluetooth::Characteristic morseData(&customService);
    morseData.set_uuid32(0x2);
    morseData.set_value(&testData, 1);
    morseData.read_cb = [&touchVal, &testData, &morseData](Characteristic::read_dataset data) {
    	morseData.serve_read(data, testData.data(), testData.length());
    	testData.clear();
    };

    customService.add_char(&morseData);
    customService.add_char(&touchChar);
    ble_handler->add_service(&customService);

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);

    testPad = new Touch::Control(TOUCH_PAD_NUM0);
   testPad->charDetectHandle = morse.getDecodeHandle();

    battery = new Peripheral::Batman(ADC2_GPIO2_CHANNEL);


    uint32_t colors[] = {Material::RED, Material::CYAN, Material::GREEN, Material::PURPLE, Material::BLUE, Material::ORANGE};

    for(uint8_t i=0; i<6; i++) {
		rgb.fill(colors[i]);
		rgb.fadeTransition(200000);
		vTaskDelay(200/portTICK_PERIOD_MS);
    }
    rgb.clear();
    rgb.apply();

    morse.word_callback = [&testData, &whoIs](std::string &word) {
    	if(word == "!off") {
    		rgb.fill(0); rgb.apply(); rgb.update();
    		ble_handler->disable();

    		esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    		esp_deep_sleep_start();
    	}

		testData += word + "\n";

    	if(word == "!b")
    		ble_handler->start_advertising(10000);
    	else if(word == "x")
    		whoIs = 1;
    	else if(word == "n")
    		whoIs = 2;
    	else if(word == "m")
    		whoIs = 3;
    	else if(word == "off")
    		whoIs = 0;

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

    while (true) {
    	batLvl = battery->read();
    	batteryC->setBatLevel(batLvl);

    	touchVal = testPad->read_raw();

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

