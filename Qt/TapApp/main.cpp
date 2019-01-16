
#include <QGuiApplication>
#include <QQmlApplicationEngine>

//#include <QAndroidService>
//#include <QAndroidJniObject>
//#include <QtAndroid>

#include <QQmlContext>

#include "tap_ble.h"

#include "ble_handler.h"

#include "spphandler.h"
#include "sppvalue.h"

int main(int argc, char *argv[])
{
	qDebug()<<"BACKGROUND!";

	/*QAndroidService app(argc, argv);

	if(QAndroidService::arguments().count() > 1) {
			auto badge = new Tap_BLE();
			badge->getHandler()->find("Tap Badge");

			return app.exec();
	}
	else {
		QAndroidJniObject::callStaticMethod<void>("com/xasin/ServiceStarter",
																	 "startMyService",
																	 "(Landroid/content/Context;)V",
																	 QtAndroid::androidActivity().object());*/

		QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

		QGuiApplication app(argc, argv);
		QQmlApplicationEngine engine;

		auto badge = new Tap_BLE();

		qmlRegisterUncreatableType<BLE_Handler>("com.xasin.tap", 1, 0, "BLE_Handler", "");
		engine.rootContext()->setContextProperty("tapBadge", badge);

		engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
		if (engine.rootObjects().isEmpty())
			return -1;

		return app.exec();
	//}
}
