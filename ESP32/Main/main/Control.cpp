/*
 * Control.cpp
 *
 *  Created on: 11 Sep 2018
 *      Author: xasin
 */

#include "Control.h"

namespace Touch {

void Control::char_detect_isr() {
	auto stats = touch_pad_get_status();
	touch_pad_clear_status();

	bool touched = (stats == padNo+1);
	touch_pad_set_trigger_mode(touched ? TOUCH_TRIGGER_ABOVE : TOUCH_TRIGGER_BELOW);

	if(charDetectHandle != nullptr)
		xTaskNotifyFromISR(charDetectHandle, touched, eSetValueWithOverwrite, nullptr);
}

Control::Control(touch_pad_t padNo) : padNo(padNo), charDetectHandle(nullptr) {

	touch_pad_init();
	touch_pad_set_fsm_mode(TOUCH_FSM_MODE_TIMER);
	//touch_pad_set_fsm_mode(TOUCH_FSM_MODE_SW);

	touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);

	touch_pad_config(padNo, 30);
	touch_pad_io_init(padNo);

	touch_pad_isr_register([](void *args) {
		((Control*)args)->char_detect_isr();
	}, this);
	touch_pad_intr_enable();

	touch_pad_set_meas_time(0x1F00, 0x07ff);
}

uint16_t Control::read_raw() {
	uint16_t output = 0;

	//touch_pad_sw_start();
	//touch_pad_read(padNo, &output);
	touch_pad_read(padNo, &output);

	return output;
}

bool Control::touched() {
	return (this->read_raw() < 30);
}

} /* namespace Touch */
