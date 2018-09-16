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

#include <string.h>

esp_err_t event_handler(void *ctx, system_event_t *event)
{
    return ESP_OK;
}

void testTask(void * params) {
	vTaskDelay(500/portTICK_PERIOD_MS);
	while(true) {
		vTaskDelay(10000 / portTICK_PERIOD_MS);
		//puts("Second delay :>");
	}
}

extern "C" void app_main(void)
{
    nvs_flash_init();
    tcpip_adapter_init();
    ESP_ERROR_CHECK( esp_event_loop_init(event_handler, NULL) );
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK( esp_wifi_init(&cfg) );
    ESP_ERROR_CHECK( esp_wifi_set_storage(WIFI_STORAGE_RAM) );
    ESP_ERROR_CHECK( esp_wifi_set_mode(WIFI_MODE_STA) );
    wifi_config_t sta_config = {};
    strcpy(reinterpret_cast<char*>(&sta_config.sta.ssid), "TP_LINK_####");
    ESP_ERROR_CHECK( esp_wifi_set_config(WIFI_IF_STA, &sta_config) );
    //ESP_ERROR_CHECK( esp_wifi_start() );
    //ESP_ERROR_CHECK( esp_wifi_connect() );

    Touch::Control testPad = Touch::Control(TOUCH_PAD_NUM0);
    Peripheral::NeoController rgb = Peripheral::NeoController(GPIO_NUM_14, RMT_CHANNEL_0, 16);

    esp_pm_config_esp32_t power_config = {};
    power_config.max_freq_mhz = 240;
	power_config.min_freq_mhz = 20;
//	power_config.light_sleep_enable = true;
    esp_pm_configure(&power_config);

    ledc_timer_config_t ledc_timer = {};
    ledc_timer.duty_resolution = LEDC_TIMER_11_BIT;
    ledc_timer.freq_hz = 1000;
    ledc_timer.speed_mode = LEDC_LOW_SPEED_MODE;
    ledc_timer.timer_num = LEDC_TIMER_0;
    // Set configuration of timer0 for high speed channels
    //ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_config = {
    	.gpio_num = GPIO_NUM_13,
		.speed_mode = LEDC_LOW_SPEED_MODE,
    	.channel = LEDC_CHANNEL_0,
		.intr_type = LEDC_INTR_DISABLE,
		.timer_sel  = LEDC_TIMER_0,
		.duty 	 = 0,
    };
    //ledc_channel_config(&ledc_config);
    //gpio_set_drive_capability(GPIO_NUM_13, GPIO_DRIVE_CAP_0);

    //ledc_fade_func_install(0);

    TaskHandle_t xHandle = NULL;
    //xTaskCreate(testTask, "TTask", 2048, NULL, 1, &xHandle);

    //esp_sleep_enable_timer_wakeup(3000000);

    //gpio_set_direction(2, GPIO_MODE_OUTPUT);
    //rtc_gpio_hold_en(GPIO_NUM_2);

    esp_pm_lock_handle_t noSleep = NULL;
    esp_pm_lock_create(ESP_PM_NO_LIGHT_SLEEP, 0, NULL, &noSleep);
    esp_pm_lock_handle_t noAPB   = NULL;
    esp_pm_lock_create(ESP_PM_APB_FREQ_MAX, 0, NULL, &noAPB);



    int level = 0;
    uint8_t i=0;
    while (true) {
    	esp_pm_lock_acquire(noSleep);
    	esp_pm_lock_acquire(noAPB);
    	level = testPad.read_raw();
    	//printf("Touch is: %4d (means: %d)\n", level, level < 300);

    	//ledc_set_fade_with_time(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, (level < 300) ? 2047 : 0, 100);
    	//ledc_fade_start(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, LEDC_FADE_WAIT_DONE);

    	rgb.clear();
    	for(uint8_t i=0; i<(16-(16*level)/1200); i++)
    		rgb[i]->g = 2;
    	rgb[i++]->r = 10;
    	if(i>=16) i=0;
    	rgb.update();

    	esp_pm_lock_release(noSleep);
    	esp_pm_lock_release(noAPB);

    	vTaskDelay(100/portTICK_PERIOD_MS);

        //puts("In main loop! Hell yeah >:D");
    }
}

