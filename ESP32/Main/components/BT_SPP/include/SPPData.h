/*
 * SPPData.h
 *
 *  Created on: 23 Nov 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BT_SPP_SPPDATA_H_
#define COMPONENTS_BT_SPP_SPPDATA_H_

#include "SPPValue.h"

#include <functional>

namespace Bluetooth {

class SPP_Server;

class SPP_Data: public SPP_Value {
private:
	bool data_changed;

	void accept_data(const void *data, size_t length);
	void on_reconnect();

public:
	void	*data;
	size_t 	data_length;

	bool allow_write;
	bool allow_partial_write;

	std::function<void (void)> write_callback;

	SPP_Data(SPP_Server &server, uint16_t id, void *data, size_t data_length);
	SPP_Data(SPP_Server &server, uint16_t id);

	template<typename T>
	void set(T* data, bool allow_write = false, bool allow_partial = false);

	bool update();
	void update_r();
};

} /* namespace Peripheral */

#endif /* COMPONENTS_BT_SPP_SPPDATA_H_ */
