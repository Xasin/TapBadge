#include "tap_ble.h"

Tap_BLE::Tap_BLE(QObject *parent) :
	QObject(parent),
	connHandler(this),
	mqtt_client(this), mqtt_sub_whoIs(nullptr), mqtt_reconnet(this),
	batteryPercent(0), batteryMV(0),
	deviceWhoIs(0), whoIs(0)
{
	setup_mqtt();

	setup_bt();
}

void Tap_BLE::setup_mqtt() {
	connect(&mqtt_reconnet, &QTimer::timeout,
			  this, [this]() {
		if(this->mqtt_client.state() == QMqttClient::Connected)
			return;

		this->mqtt_client.disconnectFromHost();
		this->mqtt_client.connectToHost();

		this->mqtt_reconnet.start(5000);
	});

	connect(&mqtt_client, &QMqttClient::disconnected, &mqtt_reconnet,
			  [this]() {
		qDebug()<<"Mqtt disconnected :c";

		mqtt_client.connectToHost();
		this->mqtt_reconnet.start(2000);
	});

	connect(&mqtt_client, &QMqttClient::connected, this,
				[this]() {
		setup_mqtt_subs();
	});

	mqtt_client.setCleanSession(false);
	mqtt_client.setClientId("XasPhone_TapApp");
	mqtt_client.setHostname("iot.eclipse.org");
	mqtt_client.setPort(1883);
	mqtt_client.setKeepAlive(10);

	mqtt_client.setWillQoS(1);
	mqtt_client.setWillTopic("Personal/Xasin/Tap/Connected");
	mqtt_client.setWillRetain(true);
	mqtt_client.setWillMessage(QString("APP DOWN").toUtf8());

	mqtt_client.connectToHost();
}

void Tap_BLE::setup_mqtt_subs() {
	qDebug()<<"MQTT connected!";

	if(connHandler.getConnectionStatus() == connHandler.CONNECTED)
		this->m_pub("Connected", "OK", 2, true);

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

		connHandler.write_data('sw', QByteArray(reinterpret_cast<char *>(&whoIs), 1));

		emit whoIsChanged();
	});

	auto mqtt_tap_cmd = this->mqtt_client.subscribe(QString("Personal/Xasin/Tap/#"), 1);
	connect(mqtt_tap_cmd, &QMqttSubscription::messageReceived, this,
			  [this](QMqttMessage msg) {
		auto key = msg.topic().levels()[3];

		qDebug()<<"Got control message for: "<<key;

		if(key == "StdbyColor" && msg.payload().length() == 3)
			connHandler.write_data('sc', msg.payload());
	});
}

void Tap_BLE::setup_bt() {
	connect(&connHandler, &BLE_Handler::connectionStatusUpdated, this,
			  [this](BLE_Handler::CONNECTION_STATUS stat) {

		QByteArray oData = QString(stat == BLE_Handler::CONNECTED ? "OK" : "").toUtf8();
		this->m_pub("Connected", oData.data(), oData.length());
	});

	connHandler.data_cb['hi'] = [this](const QByteArray &data) {
		this->m_pub("Morse/Out", data.data(), data.size());
	};

	connHandler.data_cb['BT'] = [this](const QByteArray &data) {
#pragma pack(1)
		struct BatteryPacket {
			int8_t  charge;
			int16_t mvLevel;
		};
#pragma pack()

		BatteryPacket bat = *(reinterpret_cast<const BatteryPacket*>(data.data()));

		this->batteryMV = bat.mvLevel;
		this->batteryPercent = bat.charge;

		m_pub("Battery", &bat, sizeof(bat));

		emit deviceDataUpdated();
	};

	connHandler.initiate_find("Tap Badge");
}

void Tap_BLE::m_pub(QString topic, const void *data_ptr, int length, bool retain) {
	mqtt_client.publish("Personal/Xasin/Tap/" + topic,
							  QByteArray::fromRawData(reinterpret_cast<const char *>(data_ptr), length),
							  1, retain);
}

BLE_Handler * Tap_BLE::getHandler() {
	return &(this->connHandler);
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
	connHandler.write_data('sw', QByteArray(reinterpret_cast<char *>(&whoIs), 1));

	QString whoIsNames[] = {"none", "Xasin", "Neira", "Mesh"};
	mqtt_client.publish(QString("Personal/Xasin/Switching/Who"), whoIsNames[whoIs].toUtf8(), 1, true);

	emit whoIsChanged();
}
