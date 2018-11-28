#ifndef SPP_VALUE_H
#define SPP_VALUE_H

#include <QObject>

#include "spphandler.h"

class SPPValue : public QObject
{
	Q_OBJECT
protected:
	friend SPPHandler;

	SPPHandler &handler;

	const uint16_t id;

	QByteArray dataBuffer;
	bool dataChanged;

	virtual void receive_data(const QByteArray &data);
	virtual void on_reconnected();

public:
	std::function<void (const QByteArray &data)> receiveLambda;

	SPPValue(SPPHandler &handler, uint16_t id, QObject *parent = nullptr);

	bool writeData(const QByteArray &data, bool retained = false);

signals:
	void onDataReceived(const QByteArray &data);
};

#endif // SPP_VALUE_H
