
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


#include "esp_wifi.h"
#include <cstring>


#include "RegisterBlock.h"

#include "Control.h"
#include "MorseHandle.h"

#include "NeoController.h"
#include "NotifyHandler.h"

#include "peripheral/batman.h"
#include "BatteryManager.h"

#include "xasin/mqtt/Handler.h"
#include "xasin/mqtt/MQTTSlaveChannel.h"

#include <sstream>
#include <string.h>

#include "esp_log.h"

#define WIFI_PASSWD "f36eebda48\0"
#define WIFI_SSID   "TP-LINK_84CDC2\0"

using namespace Peripheral;

auto testHandler = Xasin::MQTT::Handler();

auto mainRegisters = Xasin::Communication::RegisterBlock();
auto mqtt_channel  = Xasin::MQTT::MQTTSlaveChannel(testHandler, "Xasin/Tap", mainRegisters);

auto rgb  = Peripheral::NeoController(GPIO_NUM_5, RMT_CHANNEL_0, 1);
auto note = Peripheral::NotifyHandler(&rgb);

auto morse = Peripheral::MorseHandle(120);
Touch::Control 			*testPad;

#pragma pack(1)
struct {
	int8_t  chgLevel;
	int16_t mvLevel;
} batStat;
#pragma pack()

Peripheral::Batman *battery;
auto batteryEval = Housekeeping::BatteryManager();

esp_err_t event_handler(void *ctx, system_event_t *event)
{
	testHandler.wifi_handler(event);
	testHandler.try_wifi_reconnect(event);

    return ESP_OK;
}

void init() {
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );

    esp_timer_init();

    esp_pm_config_esp32_t power_config = {};
    power_config.max_freq_mhz = 80;
	power_config.min_freq_mhz = 80;
	power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

	testHandler.start_wifi(WIFI_SSID, WIFI_PASSWD, 2);

	testHandler.start("mqtt://0hLHKZccHuUG2CAANPCopCUldDKadSv4Y6ByTBAqWiUOIkejYExd6fmaIWaKDtEr@mqtt.flespi.io");

    testPad = new Touch::Control(TOUCH_PAD_NUM0, 45);
    testPad->charDetectHandle = morse.getDecodeHandle();
}

extern "C" void app_main(void)
{
	init();

    char whoIs = 0;
    auto whoIsRegister = Xasin::Communication::ComRegister('sw', mainRegisters, &whoIs, 1, true);
    auto morseRegister = Xasin::Communication::ComRegister('hi', mainRegisters);

    auto stdbyColor = Xasin::Communication::ComRegister('sc', mainRegisters);
    stdbyColor.write_cb = [](Xasin::Communication::Data_Packet data) {
    	uint32_t cCode = 0;
    	memcpy(&cCode, data.data, 3);

    	note.set_stdby_color(cCode);
    };

    uint32_t colors[] = {Material::RED, Material::GREEN, Material::BLUE};

    for(uint8_t i=0; i<3; i++) {
		rgb.fill(colors[i]);
		rgb.fadeTransition(120000);
		vTaskDelay(200/portTICK_PERIOD_MS);
    }
    rgb.fill(0);
    rgb.apply();
    rgb.update();

    esp_log_level_set("XAQTT", ESP_LOG_INFO);

    std::string morsePrefix = "";
    morse.word_callback = [&](std::string &word) {
    	if(word == "!off") {
    		NotifyHandler::PatternElement offP[] = {{Material::RED, 500000}};
    		note.flash(offP, 1);

    		rgb.fill(0); rgb.apply(); rgb.update();

    		esp_sleep_disable_wakeup_source(ESP_SLEEP_WAKEUP_ALL);
    		esp_deep_sleep_start();
    	}

    	if(word.data()[0] == '!') {
    		if(word.length() == 1)
    			morsePrefix = "";
    		else
    			morsePrefix = word + '!';
    	}
    	else
    		word = morsePrefix + word;

    	morseRegister.update(word);

    	if(word == "swx")
    		whoIs = 1;
    	else if(word == "swn")
    		whoIs = 2;
    	else if(word == "swm")
    		whoIs = 3;
    	else if(word == "sws")
    		whoIs = 0;

    	rgb.apply();
    	rgb.update();
    };

    NotifyHandler::PatternElement xasinPattern[] = {
    		{Color(Material::RED, 80), 50000},
			{Color(Material::RED, 30), 60000},
			{Color(Material::CYAN, 100), 60000}};

    NotifyHandler::PatternElement neiraPattern[] = {
    		{Color(Material::YELLOW, 80), 100000},
			{Color(Material::BLUE,   40), 100000},
			{Color(Material::YELLOW, 120), 100000}};

    NotifyHandler::PatternElement meshPattern[] = {
    		{Color(Material::GREEN, 70), 60000},
			{Color(Material::PURPLE, 120), 50000},
			{0,  50000},
			{Color(Material::PURPLE, 120), 50000}};

    while (true) {
    	printf("Touch is: %d\n", testPad->read_raw());

    	switch(whoIs) {
    	case 1:
    		note.flash(xasinPattern, 3);
    		break;
    	case 2:
    		note.flash(neiraPattern, 3);
    		break;
    	case 3:
    		note.flash(meshPattern, 4);
    		break;
    	default: break;

    	}
    	vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
