#include <QGuiApplication>
#include <QQmlApplicationEngine>


#include <iostream>
#include <stdint.h>

#include "DebugManager.h"
#include "QDeviceMonitor.h"

void application_cleanup()
{
}

void application_startup() 
{
    DebugManager::Init();

    atexit(&application_cleanup);
}

int main(int argc, char *argv[])
{
    application_startup();

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QDeviceMonitor deviceMonitor{};

    QQmlApplicationEngine engine;
    
	qmlRegisterSingletonInstance(
		"Grim.DeviceMonitor",
		1, 0, "DeviceMonitor",
		&deviceMonitor);
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (!engine.rootObjects().isEmpty())
    {
        app.exec();
    }

    std::string x;
    std::cin >> x;
}
