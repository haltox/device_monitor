#include "QDeviceMonitor.h"
#include "WinDeviceMonitor.h"
#include <iostream>

QDeviceMonitor::QDeviceMonitor( QObject* parent ) 
	: QObject(parent) 
	, deviceMonitor{ std::make_unique<WinDeviceMonitor>() }
{
	subscriptionId = deviceMonitor->RegisterDeviceChangeNotification(
		[this]() {
			std::cout << "Devices changed" << std::endl;
			emit serialDevicesChanged();
		}
	);

	connect(this, &QDeviceMonitor::serialDevicesChanged, 
		this, &QDeviceMonitor::OnDeviceChange, 
		Qt::QueuedConnection);

	auto deviceList = getSerialDevices();
	connectedDevices = std::set<QString>{ deviceList.begin(), deviceList.end() };

	deviceMonitor->StartMonitoringDeviceConnections();
}

QDeviceMonitor::~QDeviceMonitor() {
	disconnect(this, &QDeviceMonitor::serialDevicesChanged, 
		this, &QDeviceMonitor::OnDeviceChange);
	deviceMonitor->StopMonitoringDeviceConnections();
	deviceMonitor->UnregisterDeviceChangeNotification(subscriptionId);
}

void QDeviceMonitor::OnDeviceChange()
{
	auto deviceList = getSerialDevices();
	std::set<QString> newSetOfDevices{ deviceList.begin(), deviceList.end() };
	
	// Signal added devices
	for (const QString& deviceName : newSetOfDevices) {
		if (connectedDevices.find(deviceName) == connectedDevices.end()) {
			emit serialDeviceAdded(deviceName);
		}
	}

	// Signal removed devices
	for (const QString& deviceName : connectedDevices) {
		if (newSetOfDevices.find(deviceName) == newSetOfDevices.end()) {
			emit serialDeviceRemoved(deviceName);
		}
	}

	std::swap(connectedDevices, newSetOfDevices);
}

QStringList QDeviceMonitor::getSerialDevices() const 
{
	auto deviceList = deviceMonitor->GetListOfSerialDevices();
	QStringList resultList{};

	for (const std::wstring& device : deviceList) {
		resultList.push_back(QString::fromStdWString(device));
	}

	return resultList;
}