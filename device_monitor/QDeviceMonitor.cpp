#include "QDeviceMonitor.h"
#include "WinDeviceMonitor.h"

QDeviceMonitor::QDeviceMonitor( QObject* parent ) 
	: QObject(parent) 
	, deviceMonitor{ std::make_unique<WinDeviceMonitor>() }
{}

QDeviceMonitor::~QDeviceMonitor() {}

QStringList QDeviceMonitor::getSerialDevices() const {
	auto deviceList = deviceMonitor->GetListOfSerialDevices();
	QStringList resultList{};

	for (const std::wstring& device : deviceList) {
		resultList.push_back(QString::fromStdWString(device));
	}

	return resultList;
}