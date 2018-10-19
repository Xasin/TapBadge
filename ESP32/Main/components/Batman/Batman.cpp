/*
 * Batman.cpp
 *
 *  Created on: 17 Oct 2018
 *      Author: xasin
 */

#include "Batman.h"

namespace Peripheral {

uint16_t Batman::read_raw() {
	uint32_t resMult = 0;
	int res;

	esp_err_t r;
	for(uint8_t i=10; i!=0; i--) {
		r = adc2_get_raw(adcChannel, ADC_WIDTH_BIT_12, &res);
		if(r == ESP_OK)
			resMult += res;
		else {
			i++;
		}

		vTaskDelay(1);
	}


	return resMult;
}

int Batman::read() {
	return 2*(read_raw() * (uint32_t)3512) / 40950;
}

uint8_t Batman::get_chgstat() {
	return 0;

	gpio_set_pull_mode(statusChannel, GPIO_PULLUP_ONLY);
	if(gpio_get_level(statusChannel) == 0)
		return 1;
	gpio_set_pull_mode(statusChannel, GPIO_PULLDOWN_ONLY);
	if(gpio_get_level(statusChannel) == 1)
		return 2;
	return 0;
}

Batman::Batman(adc2_channel_t aChan, gpio_num_t statChan) : adcChannel(aChan), statusChannel(statChan) {
	gpio_set_pull_mode(statChan, GPIO_FLOATING);
	gpio_set_direction(statChan, GPIO_MODE_DISABLE);
	gpio_num_t adcChan;
	adc2_pad_get_io_num(aChan, &adcChan);
	gpio_set_direction(adcChan, GPIO_MODE_DISABLE);
	gpio_set_pull_mode(adcChan, GPIO_FLOATING);

	adc_power_on();
	adc2_config_channel_atten(adcChannel, ADC_ATTEN_DB_11);
}

} /* namespace Peripheral */
