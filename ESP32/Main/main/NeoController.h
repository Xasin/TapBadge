/*
 * NeoController.h
 *
 *  Created on: 14 Sep 2018
 *      Author: xasin
 */

#ifndef MAIN_NEOCONTROLLER_H_
#define MAIN_NEOCONTROLLER_H_

#include "driver/rmt.h"

namespace Peripheral {

class NeoController {
private:
	struct Color {
		uint8_t g;
		uint8_t r;
		uint8_t b;
	};

	Color * colors;
	const gpio_num_t pinNo;
	const uint8_t length;
	const rmt_channel_t channel;

public:
	NeoController(gpio_num_t pin, rmt_channel_t channel, uint8_t length);

	void update();

	void clear();
	Color * operator [](int id);
};

} /* namespace Touch */

#endif /* MAIN_NEOCONTROLLER_H_ */
