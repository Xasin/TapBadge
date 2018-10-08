/*
 * NeoController.cpp
 *
 *  Created on: 14 Sep 2018
 *      Author: xasin
 */

#include "NeoController.h"

namespace Peripheral {

static void IRAM_ATTR u8_to_WS2812(const void* source, rmt_item32_t* destination,
	size_t source_size, size_t wanted_elements,
	size_t* translated_size, size_t* translated_items) {

	*translated_size  = 0;
	*translated_items = 0;

	rmt_item32_t bits[2] = {};
	bits[0].duration0 = 0.35 * 20; bits[0].level0 = 1;
	bits[0].duration1 = 0.8  * 20; bits[0].level1 = 0;

	bits[1].duration0 = 0.7  * 20; bits[1].level0 = 1;
	bits[1].duration1 = 0.6  * 20; bits[1].level1 = 0;

	while(*translated_size < source_size && *translated_items < wanted_elements) {
		for(uint8_t i=0; i<8; i++) {
			destination->val = bits[(*reinterpret_cast<const int8_t *>(source))>>(7-i) & 1].val;

			(*translated_items)++;
			destination++;
		}

		(*translated_size)++;
		source++;
	}
}

NeoController::NeoController(gpio_num_t pin, rmt_channel_t channel, uint8_t length) : pinNo(pin), length(length), channel(channel) {
	this->colors = new Color[length];

	clear();

	rmt_config_t cfg = {};
	rmt_tx_config_t tx_cfg = {};
	tx_cfg.idle_level = RMT_IDLE_LEVEL_LOW;
	tx_cfg.idle_output_en = true;
	tx_cfg.idle_output_en = true;
	tx_cfg.loop_en = false;
	cfg.tx_config = tx_cfg;

	cfg.channel  = channel;
	cfg.rmt_mode = RMT_MODE_TX;
	cfg.clk_div  = 4;
	cfg.gpio_num = pinNo;
	cfg.mem_block_num = 1;

	rmt_config(&cfg);
	rmt_driver_install(channel, 0, 0);
	rmt_translator_init(channel, u8_to_WS2812);
}

void NeoController::update() {
	rmt_write_sample(channel, reinterpret_cast<const unsigned char *>(this->colors), length*3, true);
}

void NeoController::clear() {
	for(uint8_t i=0; i<length; i++)
		colors[i] = {0};
}

NeoController::Color * NeoController::operator [](int id) {
	if(id < length)
		return &this->colors[id];
	else
		return NULL;
}

} /* namespace Touch */
