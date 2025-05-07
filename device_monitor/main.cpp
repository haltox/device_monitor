#include <QGuiApplication>
#include <QQmlApplicationEngine>

#include "DebugManager.h"
#include "WinDeviceMonitor.h"

void application_startup() 
{
    DebugManager::init();
}

int main(int argc, char *argv[])
{
    application_startup();

    ///
    WinDeviceMonitor::doStuff();

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
