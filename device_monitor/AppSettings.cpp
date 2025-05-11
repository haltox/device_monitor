#include "AppSettings.h"
#include <fstream>
#include <shlobj_core.h>
#include <iostream>

#include "DebugManager.h"

AppSettings::AppSettings(QObject* parent)
	: QObject{parent}
{}

AppSettings::~AppSettings()
{ }

void AppSettings::setShowDeviceListOnStartup(bool value) {
	if (value != showDeviceListOnStartup) {
		showDeviceListOnStartup = value;
		emit settingsChanged();
		emit showDeviceListOnStartupChanged(value);
	}
}

void AppSettings::setShowSettingsOnStartup(bool value) {
	if (value != showSettingsOnStartup) {
		showSettingsOnStartup = value;
		emit settingsChanged();
		emit showSettingsOnStartupChanged(value);
	}
}

void AppSettings::setShowNotifications(bool value) {
	if (value != showNotifications) {
		showNotifications = value;
		emit settingsChanged();
		emit showNotificationsChanged(value);
	}
}

void AppSettings::setShowMostLikelyPort(bool value) {
	if (value != showMostLikelyPort) {
		showMostLikelyPort = value;
		emit settingsChanged();
		emit showMostLikelyPortChanged(value);
	}
}

void AppSettings::setLikelyPortPattern(QString value) {
	if (value != likelyPortPattern) {
		likelyPortPattern = value;
		emit settingsChanged();
		emit likelyPortPatternChanged(value);
	}
}

void AppSettings::setFirstStart(bool value) {
	firstStart = value;
}

void AppSettings::EnableAutosave()
{
	// Enable autosave
	connect(this, &AppSettings::settingsChanged, this, &AppSettings::Save);
}

bool AppSettings::Load()
{
	std::fstream settingsFile{ getSettingsPath(), std::ios::in };
	std::string txt{};
	
	if (!settingsFile.is_open()) {
		return false;
	}

	while (!settingsFile.eof()) {
		std::getline(settingsFile, txt);
		
		auto kv = splitKV(txt);
		if (kv.first.empty() || kv.second.empty()) continue;

		setSetting(kv.first, kv.second);
	}

	return true;
}

void AppSettings::Save()
{
	std::fstream settingsFile{ getSettingsPath(), std::ios::out | std::ios::trunc };
	Print(settingsFile);
}

void AppSettings::Print(std::ostream& os)
{
	os << "showDeviceListOnStartup=" << (showDeviceListOnStartup ? "true" : "false") << "\n";
	os << "showSettingsOnStartup=" << (showSettingsOnStartup ? "true" : "false") << "\n";
	os << "showNotifications=" << (showNotifications ? "true" : "false") << "\n";
	os << "showMostLikelyPort=" << (showMostLikelyPort ? "true" : "false") << "\n";
	os << "likelyPortPattern=" << likelyPortPattern.toStdString() << "\n";
}

std::wstring AppSettings::appDataPath() const
{
	WCHAR* path = nullptr;
	SHGetKnownFolderPath(
		FOLDERID_RoamingAppData,
		0,
		NULL,
		&path
	);

	std::wstring appDataPath{ path };
	CoTaskMemFree(path);

	return appDataPath;
}

std::wstring AppSettings::getAppDirectory() const {
	std::wstring path{ appDataPath() };
	std::wstring segments[] = { L"GrimC", L"DeviceMonitor" };

	for (const auto& segment : segments) {
		path += L"\\" + segment;
		if ( !CreateDirectoryW(path.c_str(), NULL) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			DebugManager::Instance().Throw("Failed to create directory: " + std::to_string(GetLastError()));
		}
	}

	return path;
}

std::wstring AppSettings::getSettingsPath() const
{
	std::wstring settingsPath{ getAppDirectory() };
	settingsPath += L"\\settings.ini";
	return settingsPath;
}

std::pair<std::string, std::string> AppSettings::splitKV(const std::string& line) const
{
	size_t pos = line.find('=');
	if (pos == std::string::npos) {
		return { "", ""};
	}

	std::string key = line.substr(0, pos);
	std::string value = line.substr(pos + 1);
	return { key, value };
}

void AppSettings::setSetting(const std::string& key, const std::string& value)
{
	if (key == "showDeviceListOnStartup") {
		setShowDeviceListOnStartup(value == "true");
	}
	else if (key == "showSettingsOnStartup") {
		setShowSettingsOnStartup(value == "true");
	}
	else if (key == "showNotifications") {
		setShowNotifications(value == "true");
	}
	else if (key == "showMostLikelyPort") {
		setShowMostLikelyPort(value == "true");
	}
	else if (key == "likelyPortPattern") {
		setLikelyPortPattern(QString::fromStdString(value));
	}
}