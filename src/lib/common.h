#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDirIterator>

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

    static const QString launcherApp()
    {
        return QString("%1-launcher").arg(DESKTOP_APP);
    }
    static const QString sessionFile()
    {
        return QString("%1/.%2-desktop-session")
               .arg(QDir::tempPath())
               .arg(DESKTOP_APP);
    }
    static const QString configDir()
    {
        QString path = QString("%1/.config/%2-desktop").arg(QDir::homePath()).arg(DESKTOP_APP);
        QDir dir(path);
        if (!dir.exists(path)) { dir.mkpath(path); }
        return path;
    }
    static const QString sessionSettingsFile()
    {
        QString file = QString("%1/%2.conf")
                       .arg(configDir())
                       .arg(DE_SESSION_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user session settings!";
            QFile mkfile(file);
            if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
        }
        return file;
    }
    static const QString desktopSettingsFile()
    {
        QString file = QString("%1/%2.conf")
                       .arg(configDir())
                       .arg(DE_DESKTOP_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user desktop settings!";
            QFile mkfile(file);
            if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
        }
        return file;
    }
    static const QString envSettingsFile()
    {
        QString file = QString("%1/%2.conf")
                       .arg(configDir())
                       .arg(DE_ENV_SETTINGS);
        if (!QFile::exists(file)) {
            qDebug() << "no user env settings!";
            QFile mkfile(file);
            if (mkfile.open(QIODevice::WriteOnly)) { mkfile.close(); }
        }
        return file;
    }
    static QStringList iconLocations(const QString &appPath)
    {
        QStringList result;
        result << QString("%1/.local/share/icons").arg(QDir::homePath());
        result << QString("%1/../share/icons").arg(appPath);
        result << "/usr/share/icons" << "/usr/local/share/icons";
        result << "/usr/pkg/share/icons";
        return result;
    }
    static QStringList pixmapLocations(const QString &appPath)
    {
        QStringList result;
        result << QString("%1/.local/share/pixmaps").arg(QDir::homePath());
        result << QString("%1/../share/pixmaps").arg(appPath);
        result << "/usr/share/pixmaps" << "/usr/local/share/pixmaps";
        result << "/usr/pkg/share/pixmaps";
        return result;
    }
    static QStringList applicationLocations(const QString &appPath)
    {
        QStringList result;
        result << QString("%1/.local/share/applications").arg(QDir::homePath());
        result << QString("%1/../share/applications").arg(appPath);
        result << "/usr/share/applications" << "/usr/local/share/applications";
        result << "/usr/pkg/share/applications";
        return result;
    }
};

#endif // COMMON_H
