/*
 * Color.h
 *
 *  Created on: 19 Sep 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_NEOCONTROLLER_COLOR_H_
#define COMPONENTS_NEOCONTROLLER_COLOR_H_

#include "driver/rmt.h"

enum Material : uint32_t {
	BLACK 	= 0x000000,
	RED 	= 0xF42316,
	PINK	= 0xE91E63,
	PURPLE	= 0x9C27B0,
	DEEP_PURPLE = 0x6A3AB7,
	INDIGO	= 0x3F51B5,
	BLUE	= 0x0546FF,
	CYAN	= 0x00CCE4,
	GREEN	= 0x20DF2C,
	LIME	= 0xCCE210,
	YELLOW	= 0xEBEB00,
	AMBER	= 0xFFC007,
	ORANGE	= 0xFF9800,
	DEEP_ORANGE = 0xFF4700
};

namespace Peripheral {

class Color {
public:
	struct ColorData {
		uint8_t g;
		uint8_t r;
		uint8_t b;
	};

	uint8_t r;
	uint8_t g;
	uint8_t b;

	Color();
	Color(uint32_t cCode, uint8_t brightness = 255);

	ColorData getLEDValue();

	void set(uint32_t cCode);
	void set(Color color);
	void set(uint32_t cCode, uint8_t div);

	void bMod(uint8_t div);

	Color overlay(Color topColor, uint8_t level);
	void  overlay(Color bottom, Color top, uint8_t alpha);

	Color operator +(Color secondColor);
	Color operator *(uint8_t brightness);
};


} /* namespace Peripheral */

#endif /* COMPONENTS_NEOCONTROLLER_COLOR_H_ */
