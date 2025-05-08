#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "DebugManager.h"
#include "WinDeviceMonitor.h"

#include <iostream>
#include <stdint.h>
#include "SLStackAllocator.h"

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

    ///
    WinDeviceMonitor::doStuff2();

    ////


#if defined(Q_OS_WIN)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    return app.exec();
}
