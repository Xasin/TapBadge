#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "tap_ble.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	auto badge = new Tap_BLE();
	badge->startDeviceSearch("Tap Badge");

	QQmlApplicationEngine engine;

	engine.rootContext()->setContextProperty("tapBadge", badge);

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
