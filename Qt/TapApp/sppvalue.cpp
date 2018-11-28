#include "sppvalue.h"

SPPValue::SPPValue(SPPHandler &handler, uint16_t id, QObject * parent) :
	QObject((parent == nullptr) ? &handler : parent), handler(handler), id(id),
	dataBuffer(), dataChanged(false),
	receiveLambda(nullptr)
{
	handler.values[id] = this;
}

void SPPValue::receive_data(const QByteArray &data) {
	dataChanged = false;
	dataBuffer.clear();

	emit onDataReceived(data);
	if(receiveLambda)
		receiveLambda(data);
}
void SPPValue::on_reconnected() {
	if(dataChanged) {
		dataChanged = false;
		handler.write_raw(id, dataBuffer);
		dataBuffer.clear();
	}
}

bool SPPValue::writeData(const QByteArray &data, bool retained) {
	if(!handler.write_raw(this->id, data)) {
		if(retained) {
			dataChanged = true;
			dataBuffer = data;
		}
		else
			return false;
	}

	return true;
}
