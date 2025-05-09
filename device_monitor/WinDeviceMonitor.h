#pragma once

#include <Windows.h>
#include <Cfgmgr32.h>

#include <vector>
#include <string>
#include "SLStackAllocator.h"
#include <optional>

class WinDeviceMonitor
{

public:
	WinDeviceMonitor();

	std::vector<std::wstring> GetInterfaceClassInstanceIdList( 
		const GUID& interfaceClass ) const;
	
	std::optional<std::wstring> GetDeviceIdFromInterfaceClassInstanceId( 
		const std::wstring& intfClassInstaceId ) const;
	
	std::optional<DEVINST> LocateDeviceInstance( 
		const std::wstring& devId ) const;

	std::optional<std::wstring> GetDevicePrettyName( 
		DEVINST devId ) const;

	std::vector<std::wstring> GetListOfSerialDevices() const;

private:

	std::vector<std::wstring> SplitWCStrList( WCHAR* buffer, ULONG bufferLen ) const;

private:
	mutable SLStackAllocator<8192, 8> allocator;

};

