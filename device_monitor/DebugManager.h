#pragma once
#include <memory>

class DebugManager
{

public:
	static DebugManager& init();
	static DebugManager& instance();

	bool IsDebug() const;

private:
	DebugManager();

	class DebugManagerImpl;
	std::unique_ptr<DebugManagerImpl> impl;
};

