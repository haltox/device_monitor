#include "WinDeviceMonitor.h"

#include <Windows.h>
#include <stdio.h>
#include <iostream>
#include <Cfgmgr32.h>

#include <vector>
#include <string>
#include <wchar.h>

#include "DebugManager.h"

#include <initguid.h>
#include <Devpkey.h>
#include <strsafe.h>
#include <iomanip>

#include <functional>

//com setup class :
// {4d36e978-e325-11ce-bfc1-08002be10318}


WinDeviceMonitor::WinDeviceMonitor()
	: allocator { }
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

std::vector<std::wstring> 
WinDeviceMonitor::GetInterfaceClassInstanceIdList(
	const GUID& interfaceClass ) const
{
	std::vector < std::wstring > interfaceClassInstanceList{};
	CONFIGRET result = CR_SUCCESS;
	GUID deviceClass = interfaceClass;

	do {
		ULONG requiredSize = 0;
		CM_Get_Device_Interface_List_SizeW(&requiredSize, 
			&deviceClass, 
			nullptr, 
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		WCHAR* buffer = allocator.alloc<WCHAR*>(requiredSize * sizeof(WCHAR));
		CONFIGRET result = CM_Get_Device_Interface_ListW(&deviceClass, 
			nullptr, 
			buffer, 
			requiredSize, 
			CM_GET_DEVICE_INTERFACE_LIST_PRESENT);

		if (result == CR_SUCCESS) {
			interfaceClassInstanceList = SplitWCStrList(buffer, requiredSize);
		}
		allocator.free(buffer);

	} while (result != CR_SUCCESS);

	return interfaceClassInstanceList;
}

std::optional<std::wstring> 
WinDeviceMonitor::GetDeviceIdFromInterfaceClassInstanceId(
	const std::wstring& intfClassInstaceId) const
{
	ULONG size = 256;
	CONFIGRET result;
	std::wstring instanceId{};

	std::optional<std::wstring> optInstanceId{std::nullopt};

	do {
		BYTE* buffer = allocator.alloc<BYTE*>(size);
		if (buffer == nullptr) {
			DebugManager::Instance().Throw("no mem avail for allocation");
			break;
		}

		ULONG bufferSize = size;
		DEVPROPKEY key_InstanceId = DEVPKEY_Device_InstanceId;
		DEVPROPTYPE propType = 0;

		result = CM_Get_Device_Interface_PropertyW(intfClassInstaceId.c_str(),
			&key_InstanceId, 
			&propType, buffer, &bufferSize, 0);

		DebugManager::Instance().ThrowIf(propType != DEVPROP_TYPE_STRING, 
			"Invalid prop type");

		if (result == CR_SUCCESS) {
			optInstanceId = std::wstring{ (WCHAR*)buffer };
		}

		allocator.free(buffer);
		size *= 2;
	} while (result == CR_BUFFER_SMALL);

	return optInstanceId;
}

std::optional<DEVINST> 
WinDeviceMonitor::LocateDeviceInstance(
	const std::wstring& devId ) const
{
	// Somehow, CM_Locate_DevNode does not accept a const WCHAR* buffer.
	// I have to copy the device instance id somewhere else.
	WCHAR* writableDeviceId = allocator.alloc<WCHAR*>(MAX_DEVICE_ID_LEN);
	StringCchCopy(writableDeviceId, MAX_DEVICE_ID_LEN, devId.c_str());

	DEVINST deviceInstance = { 0 };
	CONFIGRET result = CM_Locate_DevNode(&deviceInstance, 
		writableDeviceId, 
		CM_LOCATE_DEVNODE_NORMAL);

	allocator.free(writableDeviceId);
	
	return result == CR_SUCCESS 
		? std::make_optional<>(deviceInstance) 
		: std::nullopt;
}

std::optional<std::wstring> 
WinDeviceMonitor::GetDevicePrettyName( DEVINST devId ) const
{
	std::wstring prettyName{ L"" };

	DEVPROPKEY propKey_prettyName = DEVPKEY_Device_FriendlyName;
	DEVPROPTYPE propType = { 0 };
	
	ULONG bufferSize = 256 * sizeof(WCHAR);
	WCHAR* prettyNameBuf = allocator.alloc<WCHAR*>(bufferSize);

	CONFIGRET result = CM_Get_DevNode_PropertyW(devId, 
		&propKey_prettyName, 
		&propType, 
		(BYTE*)prettyNameBuf, 
		&bufferSize, 0);
	
	if (result == CR_SUCCESS) {
		prettyName = prettyNameBuf;
	}

	allocator.free(prettyNameBuf);
	return result == CR_SUCCESS
		? std::make_optional<>(prettyName)
		: std::nullopt;
}

std::vector<std::wstring> 
WinDeviceMonitor::GetListOfSerialDevices() const
{
	static const GUID deviceClassComPort = GUID_DEVINTERFACE_COMPORT;
	
	std::vector<std::wstring> serialDevices{};
	auto pushInVector = [&serialDevices](const std::wstring& name) { 
		serialDevices.push_back(name); 
		return std::make_optional<int>(0);
	};

	auto intfClasses = GetInterfaceClassInstanceIdList(deviceClassComPort);
	for (std::wstring& intf : intfClasses) {
		GetDeviceIdFromInterfaceClassInstanceId(intf)
			.and_then(std::bind(&WinDeviceMonitor::LocateDeviceInstance, this, std::placeholders::_1))
			.and_then(std::bind(&WinDeviceMonitor::GetDevicePrettyName, this, std::placeholders::_1))
			.and_then(pushInVector);
	}
	
	return serialDevices;
}

std::vector<std::wstring> 
WinDeviceMonitor::SplitWCStrList(WCHAR* buffer, ULONG bufferLen) const
{
	std::vector < std::wstring > deviceList{};

	WCHAR* end = buffer + bufferLen;
	while (buffer < end) {
		auto len = wcslen(buffer);
		if (len == 0) {
			break;
		}

		deviceList.push_back(std::wstring{ buffer });
		buffer += wcslen(buffer) + 1;
	}

	return deviceList;
}