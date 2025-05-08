#pragma once

#include <vector>
#include <string>
#include "SLStackAllocator.h"


class WinDeviceMonitor
{

public:
	WinDeviceMonitor();

	static void doStuff();
	static void doStuff2();

	std::vector<std::string> ListDevices();

private:
	SLStackAllocator<4096, 8> allocator;

};

