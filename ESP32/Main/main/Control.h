/*
 * Control.h
 *
 *  Created on: 11 Sep 2018
 *      Author: xasin
 */

#include "driver/touch_pad.h"

#ifndef CONTROL_H_
#define CONTROL_H_

namespace Touch {

class Control {
private:
	const touch_pad_t padNo;

public:
	Control(touch_pad_t padNo);

	uint16_t read_raw();
	bool touched();
};

} /* namespace Touch */

#endif /* CONTROL_H_ */
