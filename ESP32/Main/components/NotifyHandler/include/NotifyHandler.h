/*
 * NotifyHandler.h
 *
 *  Created on: 21 Sep 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_NOTIFYHANDLER_NOTIFYHANDLER_H_
#define COMPONENTS_NOTIFYHANDLER_NOTIFYHANDLER_H_

#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

#include "NeoController.h"

namespace Peripheral {

class NotifyHandler {
private:
	NeoController * const rgb;

	SemaphoreHandle_t blinkSemaphore;

	Color standby_color;

public:
	struct PatternElement {
		Color 	 tColor;
		uint32_t fadeDuration;
	};

	NotifyHandler(NeoController * const rgb);

	void flash(PatternElement * const pattern, uint8_t num);

	void set_stdby_color(Color c);

	void lock();
	void release();
};

} /* namespace Peripheral */

#endif /* COMPONENTS_NOTIFYHANDLER_NOTIFYHANDLER_H_ */
