#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <qqmlcontext.h>
#include <qquickwindow.h>
#include <QIcon>

#include <iostream>
#include <stdint.h>

#include "DebugManager.h"
#include "QDeviceMonitor.h"
#include "AppSettings.h"

void application_cleanup()
{
}

void application_startup() 
{
    DebugManager::Init();

    atexit(&application_cleanup);
}

void updateWindowsIcon(QQmlApplicationEngine &engine) {
    QIcon icon = QIcon{ ":/port2.png" };

    QList<QObject*> rootObjects = engine.rootObjects();
    for (QObject* obj : rootObjects) {
        for (int i = 0; i < obj->children().size(); i++) {
            QObject* child = obj->children().at(i);

            QQuickWindow* window = qobject_cast<QQuickWindow*>(child);
            if (window) {
                window->setIcon(icon);
            }
        }
    }
}

int main(int argc, char *argv[])
{
    application_startup();

#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	QCoreApplication::setAttribute(Qt::AA_UseDesktopOpenGL);
#endif

    QGuiApplication app(argc, argv);
    app.setQuitOnLastWindowClosed(false);

    QDeviceMonitor deviceMonitor{};
	
    AppSettings appSettings{};
    appSettings.Load();
    appSettings.EnableAutosave();

    QQmlApplicationEngine engine;
    
	qmlRegisterSingletonInstance(
		"Grim.DeviceMonitor",
		1, 0, "DeviceMonitor",
		&deviceMonitor);

    qmlRegisterSingletonInstance(
        "Grim.Settings",
        1, 0, "Settings",
        &appSettings );
    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (!engine.rootObjects().isEmpty())
    {
		updateWindowsIcon(engine);
        app.exec();
    }

	appSettings.Print(std::cout);
    appSettings.Save();

	if (DebugManager::Instance().IsDebug())
	{
		std::cout << "Press any key to continue..." << std::endl;
		std::cin.ignore();
	}
}
