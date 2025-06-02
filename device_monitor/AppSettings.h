#pragma once
#include <qobject.h>


class AppSettings :
    public QObject
{
    Q_OBJECT;

	Q_PROPERTY(bool showDeviceListOnStartup
		READ showsDeviceListOnStartup
		WRITE setShowDeviceListOnStartup
		NOTIFY showDeviceListOnStartupChanged);

	Q_PROPERTY(bool showSettingsOnStartup
		READ showsSettingsOnStartup
		WRITE setShowSettingsOnStartup
		NOTIFY showSettingsOnStartupChanged);

	Q_PROPERTY(bool showNotifications
		READ showsNotifications
		WRITE setShowNotifications
		NOTIFY showNotificationsChanged);

	Q_PROPERTY(bool showNotificationsPortAdded
		READ showsNotificationsPortAdded
		WRITE setShowNotificationsPortAdded
		NOTIFY showNotificationsPortAddedChanged);

	Q_PROPERTY(bool showNotificationsPortRemoved
		READ showsNotificationsPortRemoved
		WRITE setShowNotificationsPortRemoved
		NOTIFY showNotificationsPortRemovedChanged);

	Q_PROPERTY(bool showNotificationsPortsChanged
		READ showsNotificationsPortsChanged
		WRITE setShowNotificationsPortsChanged
		NOTIFY showNotificationsPortsChangedChanged);

	Q_PROPERTY(bool showOnlyRelevantPorts
		READ showsOnlyRelevantPorts
		WRITE setShowOnlyRelevantPorts
		NOTIFY showOnlyRelevantPortsChanged);

	Q_PROPERTY(QString relevantPortPattern
		READ getLikelyPortPattern
		WRITE setRelevantPortPattern
		NOTIFY relevantPortPatternChanged);

	Q_PROPERTY(bool firstStart
		READ isFirstStart
		WRITE setFirstStart );
public:
	AppSettings(QObject* parent = nullptr);
	~AppSettings();
	
	// Getters
	bool showsDeviceListOnStartup() const { return showDeviceListOnStartup; }
	bool showsSettingsOnStartup() const { return showSettingsOnStartup; }
	bool showsNotifications() const { return showNotifications; }
	bool showsNotificationsPortAdded() const { return showNotificationsPortAdded; }
	bool showsNotificationsPortRemoved() const { return showNotificationsPortRemoved; }
	bool showsNotificationsPortsChanged() const { return showNotificationsPortsChanged; }
	bool showsOnlyRelevantPorts() const { return showOnlyRelevantPorts; }
	QString getLikelyPortPattern() const { return relevantPortPattern; }
	bool isFirstStart() const { return firstStart; }

	void EnableAutosave();
	bool Load();
	void Print(std::ostream &os);

public slots:
	void Save();
	void setShowDeviceListOnStartup(bool value);
	void setShowSettingsOnStartup(bool value);
	void setShowNotifications(bool value);
	void setShowNotificationsPortAdded(bool value);
	void setShowNotificationsPortRemoved(bool value);
	void setShowNotificationsPortsChanged(bool value);
	void setShowOnlyRelevantPorts(bool value);
	void setRelevantPortPattern(QString value);
	void setFirstStart(bool value);

	/// AppSettings is going to have those common functions lol
	void OpenAppDirInExplorer();
	void ResetSettings();
	void CopyToClipboard(QString text);

signals:
	void settingsChanged();
	void showDeviceListOnStartupChanged(bool value);
	void showSettingsOnStartupChanged(bool value);
	void showNotificationsChanged(bool value);
	void showNotificationsPortAddedChanged(bool value);
	void showNotificationsPortRemovedChanged(bool value);
	void showNotificationsPortsChangedChanged(bool value);
	void showOnlyRelevantPortsChanged(bool value);
	void relevantPortPatternChanged(QString value);

private:

	std::wstring appDataPath() const;
	std::wstring getAppDirectory() const;
	std::wstring getSettingsPath() const;

	std::pair<std::string, std::string> splitKV(const std::string& line) const;
	void setSetting(const std::string& key, const std::string& value);

	bool showDeviceListOnStartup{ true };
	bool showSettingsOnStartup{ false };
	bool showNotifications{ true };
	bool showNotificationsPortAdded{ true };
	bool showNotificationsPortRemoved{ true };
	bool showNotificationsPortsChanged{ false };
	bool showOnlyRelevantPorts{ true };
	QString relevantPortPattern{ "USB Serial" };

	bool firstStart{ true };
};

