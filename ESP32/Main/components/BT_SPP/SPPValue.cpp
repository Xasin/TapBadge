/*
 * SPPValue.cpp
 *
 *  Created on: 17 Nov 2018
 *      Author: xasin
 */

#include "SPPValue.h"

#include "SPPServer.h"

namespace Bluetooth {

SPP_Value::SPP_Value(SPP_Server& server, uint16_t id) : server(server), id(id) {

}

bool SPP_Value::write(void *data, size_t length) {
	if(!server.is_connected())
		return false;

	server.write_packet(id, data, length);

	return true;
}

void SPP_Value::on_reconnect() {
	puts("System reconnected");
}
void SPP_Value::accept_data(const void *data, size_t length) {
	std::string data_str(reinterpret_cast<const char *>(data), length);

	data_str += '\0';

	printf("SPP ID: %d Got raw data: %s\n", id, data_str.data());
}

} /* namespace Bluetooth */
