#pragma once
#include <qobject.h>
#include <memory>
#include "PubSubChannel.h"
#include <set>

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
	void serialDeviceAdded(const QString device);
	void serialDeviceRemoved(const QString device);

private slots:
	void OnDeviceChange();

private:
	std::unique_ptr<WinDeviceMonitor> deviceMonitor;
	PubSubChannel::SubscriptionId subscriptionId{ 0 };
	
	std::set<QString> connectedDevices{};
};

