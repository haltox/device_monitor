#include "DebugManager.h"

#include <iostream>
#include <Windows.h>

class NoopDebugDelegate {
public:
	void init() {};
	void uninit() {};
	bool IsDebug() const { return false; }
};

class ActuallyDebuggingDebugDelegate {
public:
	void init();
	void uninit();
	bool IsDebug() const { return true; }
};

#ifdef DEBUG
using DebugDelegate = ActuallyDebuggingDebugDelegate;
#else
using DebugDelegate = NoopDebugDelegate;
#endif

class DebugManager::DebugManagerImpl{

private:
	DebugDelegate delegate;

public:
	DebugManagerImpl();
	~DebugManagerImpl();

	bool IsDebug() const;
};


std::unique_ptr<DebugManager> DebugManagerInstance;

DebugManager::DebugManager() 
	: impl{ new DebugManagerImpl() } {
}

DebugManager& init() {
	if (DebugManagerInstance != nullptr ) {
		if (DebugManagerInstance->IsDebug()) {
			throw std::runtime_error{ "Attempting to init DebugManager twice" };
		}
	}
}

DebugManager& DebugManager::instance() {
	if (DebugManagerInstance == nullptr) {
		throw std::runtime_error{ "DebugManger must be initialized before instance access" };
	}
}

bool DebugManager::IsDebug() const {
	return impl->IsDebug();
}

////////////// actual delegate

void ActuallyDebuggingDebugDelegate::init() {
	HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);

	AllocConsole();
	freopen("conin$", "r", stdin);
	HANDLE hStdin2 = GetStdHandle(STD_INPUT_HANDLE);
	freopen("conout$", "w", stdout);
	freopen("conout$", "w", stderr);

	std::cout << (unsigned long long)hStdin << "," << (unsigned long long)hStdin2 << std::endl;
}

void ActuallyDebuggingDebugDelegate::uninit() {
	FreeConsole();
}

////////////// pimpl

DebugManager::DebugManagerImpl::DebugManagerImpl() {
	delegate.init();
}

DebugManager::DebugManagerImpl::~DebugManagerImpl() {
	delegate.uninit();
}

bool DebugManager::DebugManagerImpl::IsDebug() const{
	return delegate.IsDebug();
}