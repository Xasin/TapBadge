#include "tap_ble.h"

Tap_BLE::Tap_BLE(QObject *parent) :
	QObject(parent),
	ble_handler(this),
	mqtt_client(this), mqtt_sub_whoIs(nullptr), mqtt_reconnet(this),
	batteryPercent(0), batteryMV(0),
	deviceWhoIs(0), whoIs(0)
{
	connect(&ble_handler, &BLE_Handler::properties_updated,
			  this, &Tap_BLE::onProperties_updated);

	connect(&mqtt_reconnet, &QTimer::timeout,
			  this, [this]() {
		if(this->mqtt_client.state() == QMqttClient::Connected)
			return;

		this->mqtt_client.disconnectFromHost();
		this->mqtt_client.connectToHost();

		this->mqtt_reconnet.start(30000);
	});

	connect(&mqtt_client, &QMqttClient::disconnected, &mqtt_reconnet,
			  [this]() {
		qDebug()<<"Mqtt disconnected :c";

		mqtt_client.connectToHost();
		this->mqtt_reconnet.start(5000);
	});

	connect(&mqtt_client, &QMqttClient::connected, this,
			  [this]() {
		qDebug()<<"MQTT connected!";

		mqtt_reconnet.stop();

		mqtt_sub_whoIs = this->mqtt_client.subscribe(QString("Personal/Xasin/Switching/Who"));
		connect(mqtt_sub_whoIs, &QMqttSubscription::messageReceived, this,
				  [this](QMqttMessage msg) {
			QString newWhoIs = msg.payload();
			if(newWhoIs == this->whoIs)
				return;

			int whoIsCode = 0;
			if(newWhoIs == "Xasin")
				whoIsCode = 1;
			else if(newWhoIs == "Neira")
				whoIsCode = 2;
			else if(newWhoIs == "Mesh")
				whoIsCode = 3;

			this->whoIs = whoIsCode;
			QByteArray data; data.push_back(char(whoIs));
			ble_handler.write_data(QBluetoothUuid(quint32(0x1)), data);
			emit whoIsChanged();
		});
	});

	mqtt_client.setCleanSession(false);
	mqtt_client.setClientId("XasPhone_TapApp");
	mqtt_client.setHostname("iot.eclipse.org");
	mqtt_client.setPort(1883);
	mqtt_client.setKeepAlive(10000);

	mqtt_client.connectToHost();
}

BLE_Handler * Tap_BLE::getHandler() {
	return &(this->ble_handler);
}

int Tap_BLE::getBatteryPercent() {
	return this->batteryPercent;
}
int Tap_BLE::getBatteryMV() {
	return int(this->batteryMV);
}

int Tap_BLE::getWhoIs() {
	return whoIs;
}
void Tap_BLE::setWhoIs(int whoIs) {
	if(whoIs < 0 || whoIs > 3)
		return;
	if(whoIs == this->whoIs)
		return;

	this->whoIs = whoIs;
	QByteArray data; data.push_back(char(whoIs));
	ble_handler.write_data(QBluetoothUuid(quint32(0x1)), data);

	QString whoIsNames[] = {"none", "Xasin", "Neira", "Mesh"};
	mqtt_client.publish(QString("Personal/Xasin/Switching/Who"), whoIsNames[whoIs].toUtf8(), 1, true);

	emit whoIsChanged();
}

void Tap_BLE::onProperties_updated() {
	auto data = ble_handler.get_data(QBluetoothUuid(quint32(0x2a1a)));
	if(data != nullptr)
		batteryMV = *(reinterpret_cast<uint16_t*>(data->data()));

	data = ble_handler.get_data(QBluetoothUuid(quint32(0x2a19)));
	if(data != nullptr)
		batteryPercent = *(reinterpret_cast<uint8_t*>(data->data()));

	data = ble_handler.get_data(QBluetoothUuid(quint32(1)));
	if(data != nullptr) {
		int newWhois = *(reinterpret_cast<char*>(data->data()));

		if((newWhois != deviceWhoIs)) {
			deviceWhoIs = newWhois;
			ble_handler.clear_data(QBluetoothUuid(quint32(0x1)));

			if(newWhois != whoIs) {
				whoIs = newWhois;

				QString whoIsNames[] = {"none", "Xasin", "Neira", "Mesh"};
				mqtt_client.publish(QString("Personal/Xasin/Switching/Who"), whoIsNames[whoIs].toUtf8(), 1, true);
				emit whoIsChanged();
			}
		}
	}

	data = ble_handler.get_data(QBluetoothUuid(quint32(2)));
	if(data != nullptr) {
		qDebug()<<"Test boop value is:"<<(*data);
	}

	emit deviceDataUpdated();
	qDebug()<<"Battery MV is:"<<batteryMV;
}
