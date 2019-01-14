/*
 * NotifyHandler.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: xasin
 */

#include "NotifyHandler.h"

namespace Peripheral {

NotifyHandler::NotifyHandler(NeoController * const rgb) : rgb(rgb) {
	blinkSemaphore = xSemaphoreCreateMutex();
}

void NotifyHandler::flash(PatternElement * const pattern, uint8_t num) {
	lock();

	Color oldC = *(rgb->get(0));

	for(uint8_t i=0; i<num; i++) {
		(*rgb)[0].set(pattern[i].tColor);
		rgb->fadeTransition(pattern[i].fadeDuration);
	}

	(*rgb)[0] = oldC;
	rgb->fadeTransition(30000);

	release();
}

void NotifyHandler::lock() {
	xSemaphoreTake(blinkSemaphore, portMAX_DELAY);
}
void NotifyHandler::release() {
	xSemaphoreGive(blinkSemaphore);
}

} /* namespace Peripheral */
