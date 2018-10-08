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

public:
	struct PatternElement {
		Color 	 tColor;
		uint32_t fadeDuration;
	};

	NotifyHandler(NeoController * const rgb);

	void flash(PatternElement * const pattern, uint8_t num);

	void lock();
	void release();
};

} /* namespace Peripheral */

#endif /* COMPONENTS_NOTIFYHANDLER_NOTIFYHANDLER_H_ */
