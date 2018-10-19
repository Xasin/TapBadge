/*
 * Batman.h
 *
 *  Created on: 17 Oct 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BATMAN_BATMAN_H_
#define COMPONENTS_BATMAN_BATMAN_H_

#include "driver/gpio.h"
#include "driver/adc.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

namespace Peripheral {

class Batman {
public:
	const adc2_channel_t adcChannel;
	const gpio_num_t	 statusChannel;

	uint16_t read_raw();
	int read();

	uint8_t get_chgstat();

	Batman(adc2_channel_t analogChannel, gpio_num_t statusChannel);
};

} /* namespace Peripheral */

#endif /* COMPONENTS_BATMAN_BATMAN_H_ */
