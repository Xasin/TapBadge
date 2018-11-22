
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

#include "SPPServer.h"

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
		note.flash(btStart, 2);
		vTaskDelay(5000);
	}
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

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);

    testPad = new Touch::Control(TOUCH_PAD_NUM0);
    testPad->charDetectHandle = morse.getDecodeHandle();

    battery = new Peripheral::Batman(ADC2_GPIO2_CHANNEL);

    std::string testData;
    volatile uint8_t whoIs;
    uint8_t touchVal;

    uint32_t colors[] = {Material::RED, Material::CYAN, Material::GREEN, Material::PURPLE, Material::BLUE, Material::ORANGE};

    Bluetooth::SPP_Server testServer(void);

    testServer.values['A'] = new Bluetooth::SPP_Value(testServer, uint16_t('A'));

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

    		esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    		esp_deep_sleep_start();
    	}

		testData += word + "\n";

    	if(word == "x")
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

