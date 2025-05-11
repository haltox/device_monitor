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

	Q_PROPERTY(bool showMostLikelyPort
		READ showsMostLikelyPort
		WRITE setShowMostLikelyPort
		NOTIFY showMostLikelyPortChanged);

	Q_PROPERTY(QString likelyPortPattern
		READ getLikelyPortPattern
		WRITE setLikelyPortPattern
		NOTIFY likelyPortPatternChanged);

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
	bool showsMostLikelyPort() const { return showMostLikelyPort; }
	QString getLikelyPortPattern() const { return likelyPortPattern; }
	bool isFirstStart() const { return firstStart; }

	void EnableAutosave();
	bool Load();
	void Print(std::ostream &os);

public slots:
	void Save();
	void setShowDeviceListOnStartup(bool value);
	void setShowSettingsOnStartup(bool value);
	void setShowNotifications(bool value);
	void setShowMostLikelyPort(bool value);
	void setLikelyPortPattern(QString value);
	void setFirstStart(bool value);

signals:
	void settingsChanged();
	void showDeviceListOnStartupChanged(bool value);
	void showSettingsOnStartupChanged(bool value);
	void showNotificationsChanged(bool value);
	void showMostLikelyPortChanged(bool value);
	void likelyPortPatternChanged(QString value);

private:

	std::wstring appDataPath() const;
	std::wstring getAppDirectory() const;
	std::wstring getSettingsPath() const;

	std::pair<std::string, std::string> splitKV(const std::string& line) const;
	void setSetting(const std::string& key, const std::string& value);

	bool showDeviceListOnStartup{ true };
	bool showSettingsOnStartup{ false };
	bool showNotifications{ true };
	bool showMostLikelyPort{ true };
	QString likelyPortPattern{ "USB Serial" };

	bool firstStart{ true };
};

