/*
 * SPPValue.h
 *
 *  Created on: 17 Nov 2018
 *      Author: xasin
 */

#ifndef COMPONENTS_BT_SPP_SPPVALUE_H_
#define COMPONENTS_BT_SPP_SPPVALUE_H_

#include "stddef.h"
#include "stdint.h"

namespace Bluetooth {

class SPP_Server;

class SPP_Value {
private:
	SPP_Server& server;

protected:
	friend SPP_Server;

	virtual void accept_data(const void *data, size_t length);
	virtual void on_reconnect();

	bool	 data_changed;

public:
	const uint16_t id;

	void 	*data_location;
	size_t 	 data_length;

	bool write_into_data;

	SPP_Value(SPP_Server& server, uint16_t id);

	bool write(void *data, size_t length);
	void write_retained(void *data, size_t length);

	void update_retained();
};

} /* namespace Bluetooth */

#endif /* COMPONENTS_BT_SPP_SPPVALUE_H_ */
