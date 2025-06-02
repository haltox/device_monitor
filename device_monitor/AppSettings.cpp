#include "AppSettings.h"
#include <fstream>
#include <shlobj_core.h>
#include <iostream>

#include "DebugManager.h"
#include <qapplication.h>
#include <qclipboard.h>

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

void AppSettings::setShowNotificationsPortAdded(bool value) {
	if (value != showNotificationsPortAdded) {
		showNotificationsPortAdded = value;
		emit settingsChanged();
		emit showNotificationsPortAddedChanged(value);
	}
}

void AppSettings::setShowNotificationsPortRemoved(bool value) {
	if (value != showNotificationsPortRemoved) {
		showNotificationsPortRemoved = value;
		emit settingsChanged();
		emit showNotificationsPortRemovedChanged(value);
	}
}

void AppSettings::setShowNotificationsPortsChanged(bool value) {
	if (value != showNotificationsPortsChanged) {
		showNotificationsPortsChanged = value;
		emit settingsChanged();
		emit showNotificationsPortsChangedChanged(value);
	}
}

void AppSettings::setShowOnlyRelevantPorts(bool value) {
	if (value != showOnlyRelevantPorts) {
		showOnlyRelevantPorts = value;
		emit settingsChanged();
		emit showOnlyRelevantPortsChanged(value);
	}
}

void AppSettings::setRelevantPortPattern(QString value) {
	if (value != relevantPortPattern) {
		relevantPortPattern = value;
		emit settingsChanged();
		emit relevantPortPatternChanged(value);
	}
}

void AppSettings::setFirstStart(bool value) {
	firstStart = value;
}

void AppSettings::OpenAppDirInExplorer() {
	std::wstring appDir = getAppDirectory();

	// Use ShellExecute to open the specified location in Explorer.
	ShellExecute(NULL, L"open", appDir.c_str(), NULL, NULL, SW_SHOWNORMAL);
}

void AppSettings::ResetSettings() {
	AppSettings freshBoi{};

	setShowDeviceListOnStartup(freshBoi.showsDeviceListOnStartup());
	setShowSettingsOnStartup(freshBoi.showsSettingsOnStartup());
	setShowNotifications(freshBoi.showsNotifications());
	setShowNotificationsPortAdded(freshBoi.showsNotificationsPortAdded());
	setShowNotificationsPortRemoved(freshBoi.showsNotificationsPortRemoved());
	setShowNotificationsPortsChanged(freshBoi.showsNotificationsPortsChanged());
	setShowOnlyRelevantPorts(freshBoi.showsOnlyRelevantPorts());
	setRelevantPortPattern(freshBoi.getLikelyPortPattern());
}

void AppSettings::CopyToClipboard(QString text) {
	QApplication::clipboard()->setText(text);
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
		setFirstStart(false);
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
	os << "showNotificationsPortAdded=" << (showNotificationsPortAdded ? "true" : "false") << "\n";
	os << "showNotificationsPortRemoved=" << (showNotificationsPortRemoved ? "true" : "false") << "\n";
	os << "showNotificationsPortsChanged=" << (showNotificationsPortsChanged ? "true" : "false") << "\n";
	os << "showOnlyRelevantPorts=" << (showOnlyRelevantPorts ? "true" : "false") << "\n";
	os << "relevantPortPattern=" << relevantPortPattern.toStdString() << "\n";
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
	else if (key == "showNotificationsPortAdded") {
		setShowNotificationsPortAdded(value == "true");
	}
	else if (key == "showNotificationsPortRemoved") {
		setShowNotificationsPortRemoved(value == "true");
	}
	else if (key == "showNotificationsPortsChanged") {
		setShowNotificationsPortsChanged(value == "true");
	}
	else if (key == "showOnlyRelevantPorts") {
		setShowOnlyRelevantPorts(value == "true");
	}
	else if (key == "relevantPortPattern") {
		setRelevantPortPattern(QString::fromStdString(value));
	}
}