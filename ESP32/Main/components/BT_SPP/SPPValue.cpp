/*
 * SPPValue.cpp
 *
 *  Created on: 17 Nov 2018
 *      Author: xasin
 */

#include "SPPValue.h"
#include "SPPServer.h"

#include <cstring>

namespace Bluetooth {

SPP_Value::SPP_Value(SPP_Server& server, uint16_t id) :
		server(server),
		data_changed(false),
		id(id),
		data_location(nullptr), data_length(0),
		write_into_data(false) {
}

void SPP_Value::on_reconnect() {
	if(data_changed) {
		write(data_location, data_length);
		data_changed = false;
	}
}
void SPP_Value::accept_data(const void *data, size_t length) {
	std::string data_str(reinterpret_cast<const char *>(data), length);

	data_str += '\0';

	printf("SPP ID: %d Got raw data: %s\n", id, data_str.data());

	if(write_into_data && length <= data_length && data_location != nullptr)
		memcpy(data_location, data, length);
}


bool SPP_Value::write(void *data, size_t length) {
	if(!server.is_connected())
		return false;

	if(data != nullptr && length != 0)
		server.write_packet(id, data, length);
	else
		server.write_packet(id, 0, 0);

	return true;
}
void SPP_Value::write_retained(void *data, size_t length) {
	if(!write(data, length)) {
		data_location = data;
		data_length = length;
		data_changed = true;
	}
}

void SPP_Value::update_retained() {
	if(!write(data_location, data_length))
		data_changed = true;
}

} /* namespace Bluetooth */
