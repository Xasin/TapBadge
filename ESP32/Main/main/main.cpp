
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

Peripheral::NeoController rgb  = Peripheral::NeoController(GPIO_NUM_5, RMT_CHANNEL_0, 1);
Peripheral::NotifyHandler note = Peripheral::NotifyHandler(&rgb);

Peripheral::MorseHandle morse = Peripheral::MorseHandle(120);

Peripheral::BLE_Handler *ble_handler;

uint16_t batLvl;

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
		vTaskDelay(120000 / portTICK_PERIOD_MS);
		note.flash(testPattern, 7);
	}
}

void setup_bt() {
    ble_handler = new Peripheral::BLE_Handler("Tap Badge");
    auto tService = Peripheral::Bluetooth::Service(ble_handler);
    tService.set_uuid16(0x180F);

    auto tChar = Peripheral::Bluetooth::Characteristic(&tService);
    tChar.set_uuid16(0x2A19);
    tChar.can_write(true);

    tChar.write_cb = [](Peripheral::Bluetooth::Characteristic::write_dataset data) {
    	rgb.fill(0xFFFFFF);
    	rgb.apply();
    	rgb.update();
    };

    auto tChar2 = Peripheral::Bluetooth::Characteristic(&tService);

    tChar2.set_uuid32(0x1234);
    tChar2.value.attr_len = 2;
    tChar2.value.attr_max_len = 2;
    batLvl = 0x1337;
    tChar2.value.attr_value = reinterpret_cast<uint8_t *>(&batLvl);

    tService.set_primary(true);

    tService.add_char(&tChar);
    tService.add_char(&tChar2);

    ble_handler->add_service(&tService);

    ble_handler->set_GAP_param(ble_handler->get_GAP_defaults());

    ble_handler->enable();

    //ble_handler->start_advertising();
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
	power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    setup_bt();

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);

    Touch::Control testPad = Touch::Control(TOUCH_PAD_NUM0);
    testPad.charDetectHandle = morse.getDecodeHandle();

    Peripheral::Batman battery = Peripheral::Batman(ADC2_GPIO2_CHANNEL);


    uint32_t colors[] = {Material::RED, Material::PINK, Material::PURPLE, Material::DEEP_PURPLE, Material::INDIGO,
							 Material::BLUE, Material::CYAN, Material::GREEN, Material::LIME, Material::YELLOW, Material::AMBER, Material::ORANGE, Material::DEEP_ORANGE};

    for(uint8_t i=0; i<12; i++) {
		rgb.fill(colors[i]);
		rgb.fadeTransition(100000);
		vTaskDelay(200/portTICK_PERIOD_MS);
    }
    rgb.clear();

    morse.word_callback = [](std::string &word) {
    	if(word == "red")
    		rgb.fill(Material::RED);
    	else if(word == "blue")
    		rgb.fill(Material::BLUE);
    	else if(word == "green")
    		rgb.fill(Material::GREEN);
    	else if(word == "off")
    		rgb.fill(0);
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

    while (true) {
    	batLvl = battery.read();
    	//tChar.testData = batLvl / 42;

    	printf("Bat. lvl: %4d | Touch: %1d\n", batLvl, testPad.read_raw());

    	ble_handler->start_advertising();
    	note.flash(xasinPattern, 4);

    	vTaskDelay(3000 / portTICK_PERIOD_MS);
    	ble_handler->disable();

    	note.flash(neiraPattern, 4);

    	vTaskDelay(3000);

    	note.flash(meshPattern, 5);

    	vTaskDelay(3000);
    }
}

