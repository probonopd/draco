#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QString>
#include <QDir>
#include <QFile>
#include <QDebug>

#define DE_SESSION_SETTINGS "sessionsettings"
#define DE_DESKTOP_SETTINGS "desktopsettings"
#define DE_PLUGIN_SETTINGS "pluginsettings"
#define DE_ENV_SETTINGS "envsettings"

class Draco : public QObject
{
    Q_OBJECT

public:
    Draco() {}
    virtual ~Draco() {}

    static const QString sessionFile()
    {
        return QString("%1/.%2-desktop-session")
               .arg(QDir::tempPath())
               .arg(DESKTOP_APP);
    }
    static const QString sessionSettingsFile()
    {
        QString file = QString("%1/.config/%2-desktop/%3.conf")
                       .arg(QDir::homePath())
                       .arg(DESKTOP_APP)
                       .arg(DE_SESSION_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user session settings!";
            // TODO add default
        }
        return file;
    }
    static const QString desktopSettingsFile()
    {
        QString file = QString("%1/.config/%2-desktop/%3.conf")
                       .arg(QDir::homePath())
                       .arg(DESKTOP_APP)
                       .arg(DE_DESKTOP_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user desktop settings!";
            // TODO add default
        }
        return file;
    }
    static const QString envSettingsFile()
    {
        QString file = QString("%1/.config/%2-desktop/%3.conf")
                       .arg(QDir::homePath())
                       .arg(DESKTOP_APP)
                       .arg(DE_ENV_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user env settings!";
            // TODO add default
        }
        return file;
    }
};

#endif // COMMON_H
