#include "WinDeviceMonitor.h"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <Cfgmgr32.h>

#include "DebugManager.h"

WinDeviceMonitor::WinDeviceMonitor()
	: allocator { 4096 }
{}

DWORD wootwoot(
	_In_ HCMNOTIFICATION       hNotify,
	_In_opt_ PVOID             Context,
	_In_ CM_NOTIFY_ACTION      Action,
	_In_reads_bytes_(EventDataSize) PCM_NOTIFY_EVENT_DATA EventData,
	_In_ DWORD                 EventDataSize
) {
	std::wcout << L"event : ";

	switch (EventData->FilterType) {
	case CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE :
	{
		OLECHAR buffer[128];
		LPOLESTR strBuffer = buffer;
		StringFromCLSID(EventData->u.DeviceInterface.ClassGuid, &strBuffer);
		std::wcout << strBuffer << std::endl;

	} break;

	case CM_NOTIFY_FILTER_TYPE_DEVICEHANDLE :
	case CM_NOTIFY_FILTER_TYPE_DEVICEINSTANCE:
		break;
	}

	return ERROR_SUCCESS;
}

void WinDeviceMonitor::doStuff() {

	// Setup 

	GUID KSCATEGORY_CAPTURE;
	(void)CLSIDFromString(L"{6994AD05-93EF-11D0-A3CC-00A0C9223196}", &KSCATEGORY_CAPTURE);

	// The good one
	//GUID_DEVINTERFACE_COMPORT

	CM_NOTIFY_FILTER filter;
	memset(&filter, 0, sizeof(filter));
	filter.cbSize = sizeof(CM_NOTIFY_FILTER);
	filter.FilterType = CM_NOTIFY_FILTER_TYPE_DEVICEINTERFACE;
	filter.u.DeviceInterface.ClassGuid = KSCATEGORY_CAPTURE;

	HCMNOTIFICATION context;
	CONFIGRET result = CM_Register_Notification(&filter, nullptr, &wootwoot, &context);
	if (result != CR_SUCCESS) 
	{
		throw std::runtime_error{ "Could not setup device notifications" };
	}
}

void WinDeviceMonitor::doStuff2() 
{
	GUID interfaceGUID;

	//(void)CLSIDFromString(L"{6994AD05-93EF-11D0-A3CC-00A0C9223196}", &interfaceGUID);
	interfaceGUID = GUID_DEVINTERFACE_COMPORT;

	WCHAR buffer[1024];

	CM_Get_Device_Interface_List(&interfaceGUID,
		NULL,
		buffer,
		1024,
		CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

	std::wcout << buffer << std::endl;
}


std::vector<std::string> WinDeviceMonitor::ListDevices()
{
	std::vector < std::string > result{};



	return result;
}