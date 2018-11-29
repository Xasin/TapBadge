#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include <QQmlContext>

#include "tap_ble.h"

#include "ble_handler.h"

#include "spphandler.h"
#include "sppvalue.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

	QGuiApplication app(argc, argv);

	auto badge = new Tap_BLE();

	QQmlApplicationEngine engine;

	qmlRegisterUncreatableType<BLE_Handler>("com.xasin.tap", 1, 0, "BLE_Handler", "");
	engine.rootContext()->setContextProperty("tapBadge", badge);

	engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;

	return app.exec();
}
