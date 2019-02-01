/*
 * NotifyHandler.cpp
 *
 *  Created on: 21 Sep 2018
 *      Author: xasin
 */

#include "NotifyHandler.h"

namespace Peripheral {

NotifyHandler::NotifyHandler(NeoController * const rgb) :
		rgb(rgb), standby_color(0) {
	blinkSemaphore = xSemaphoreCreateMutex();
}

void NotifyHandler::flash(PatternElement * const pattern, uint8_t num) {
	lock();

	for(uint8_t i=0; i<num; i++) {
		(*rgb)[0].set(pattern[i].tColor);
		rgb->fadeTransition(pattern[i].fadeDuration);
	}

	(*rgb)[0] = standby_color;
	rgb->fadeTransition(100000);

	release();
}

void NotifyHandler::set_stdby_color(Color c) {
	standby_color = c;

	if(xSemaphoreGetMutexHolder(blinkSemaphore) == nullptr) {
		lock();

		(*rgb)[0] = standby_color;
		rgb->fadeTransition(300000);

		release();
	}
}

void NotifyHandler::lock() {
	xSemaphoreTake(blinkSemaphore, portMAX_DELAY);
}
void NotifyHandler::release() {
	xSemaphoreGive(blinkSemaphore);
}

} /* namespace Peripheral */
