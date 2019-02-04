
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
#include "BLESlaveChannel.h"

#include "Control.h"
#include "MorseHandle.h"

#include "NeoController.h"
#include "NotifyHandler.h"

#include "peripheral/batman.h"
#include "BatteryManager.h"

#include "xasin/mqtt/Handler.h"

#include <sstream>
#include <string.h>

#define WIFI_PASSWD "f36eebda48\0"
#define WIFI_SSID   "TP-LINK_84CDC2\0"

using namespace Peripheral;

auto testHandler = Xasin::MQTT::Handler();

auto mainRegisters = Xasin::Communication::RegisterBlock();
auto ble_channel   = Xasin::Communication::BLE_SlaveChannel("Tap Badge", mainRegisters);

Peripheral::NeoController rgb  = Peripheral::NeoController(GPIO_NUM_5, RMT_CHANNEL_0, 1);
Peripheral::NotifyHandler note = Peripheral::NotifyHandler(&rgb);

Peripheral::MorseHandle morse = Peripheral::MorseHandle(120);
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

	switch(event->event_id) {
    case SYSTEM_EVENT_STA_START:
	puts("WiFi STA started!");
        esp_wifi_connect();
        break;
    case SYSTEM_EVENT_STA_GOT_IP:
    	puts("WiFi connected!");
    	break;
    case SYSTEM_EVENT_STA_DISCONNECTED:
    	puts("WiFi disconnected!");
    	esp_wifi_connect();
    	break;
    default:  break;
	}

    return ESP_OK;
}

void testTask(void * params) {
	puts("Battery task started!");

	vTaskDelay(500/portTICK_PERIOD_MS);

	auto batRegister   = Xasin::Communication::ComRegister('BT', mainRegisters, &batStat, sizeof(batStat));

	NotifyHandler::PatternElement noBLE[] = {
			{Material::BLUE, 150000},
			{Material::RED,  300000}
	};

	TickType_t lastDelayTime;

	while(true) {
		batStat.mvLevel = battery->read();
		batStat.chgLevel = batteryEval.capacity_for_voltage(batStat.mvLevel);

		batRegister.update();
		printf("VBat: %4d (%3d%%)\n", batStat.mvLevel, batStat.chgLevel);

		NotifyHandler::PatternElement btStart[] = {
				{Color(Material::RED).merge_overlay(Material::GREEN, batStat.chgLevel * 2.5).bMod(100), 100000},
				{Color(Material::RED).merge_overlay(Material::GREEN, batStat.chgLevel * 2.5).bMod(50), 200000},
		};

		note.flash(btStart, 2);
		if(!ble_channel.is_connected())
			note.flash(noBLE, 2);

		vTaskDelayUntil(&lastDelayTime, 9000 / portTICK_PERIOD_MS);
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

    //ble_channel.start();

    // WIFI TEST CODE - FIXME
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

	ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
	ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
	ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );

	wifi_config_t wifi_cfg = {};
	wifi_sta_config_t* sta_cfg = &(wifi_cfg.sta);

	memcpy(sta_cfg->password, WIFI_PASSWD, strlen(WIFI_PASSWD));
	memcpy(sta_cfg->ssid, WIFI_SSID, strlen(WIFI_SSID));

	ESP_ERROR_CHECK( esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_cfg) );
	ESP_ERROR_CHECK( esp_wifi_start() );
	// END WIFI TEST CODE

    testHandler.start("mqtt://iot.eclipse.org");

    testPad = new Touch::Control(TOUCH_PAD_NUM0);
    testPad->charDetectHandle = morse.getDecodeHandle();

    battery = new Peripheral::Batman(ADC2_GPIO2_CHANNEL);

    char whoIs = 0;

    auto whoIsRegister = Xasin::Communication::ComRegister('sw', mainRegisters, &whoIs, 1, true);
    auto morseRegister = Xasin::Communication::ComRegister('hi', mainRegisters);

    auto stdbyColor = Xasin::Communication::ComRegister('sc', mainRegisters);
    stdbyColor.write_cb = [](Xasin::Communication::Data_Packet data) {
    	uint32_t cCode = 0;
    	memcpy(&cCode, data.data, 3);

    	note.set_stdby_color(cCode);
    };

    uint32_t colors[] = {Material::RED, Material::CYAN, Material::GREEN, Material::PURPLE, Material::BLUE, Material::ORANGE};

    for(uint8_t i=0; i<6; i++) {
		rgb.fill(colors[i]);
		rgb.fadeTransition(200000);
		vTaskDelay(200/portTICK_PERIOD_MS);
    }
    rgb.clear();
    rgb.apply();

    TaskHandle_t xHandle = NULL;
    xTaskCreate(testTask, "TTask", 2048, NULL, 2, &xHandle);


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

    	const char *test = "Hi!";
    	testHandler.publish_to("Xasin/Boop", test, 3);
    }
}

