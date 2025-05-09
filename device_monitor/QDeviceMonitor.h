#pragma once
#include <qobject.h>
#include <memory>

class WinDeviceMonitor;

class QDeviceMonitor :
    public QObject
{
	Q_OBJECT;

	Q_PROPERTY(QStringList serialDevices 
		READ getSerialDevices 
		NOTIFY serialDevicesChanged);

public:
	QDeviceMonitor(QObject* parent = nullptr);
	~QDeviceMonitor();

	QStringList getSerialDevices() const;

signals:
	void serialDevicesChanged();

private:
	std::unique_ptr<WinDeviceMonitor> deviceMonitor;
};

