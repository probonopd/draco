#ifndef COMMON_H
#define COMMON_H

#include <QObject>
#include <QStringList>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDirIterator>
#include <QApplication>

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
    static const QString terminalApp()
    {
        return "qterminal";
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
    static bool xdgOpenCheck()
    {
        // "replace" xdg-open with draco-launcher
        QString shadowPath =  QString("%1/bin").arg(configDir());
        QString shadowXDG = QString("%1/xdg-open").arg(shadowPath);
        if (!QFile::exists(shadowPath)) {
            qDebug() << "create shadow bin dir" << shadowPath;
            QDir shadowDir(shadowPath);
            shadowDir.mkpath(shadowPath);
        }
        if (!QFile::exists(shadowXDG)) {
            qDebug() << "create shadow xdg-open" << shadowXDG;
            QFile symlink(QString("%1/%2").arg(qApp->applicationDirPath()).arg(launcherApp()));
            if (!symlink.link(shadowXDG)) {
                qDebug() << "FAILED TO CREATE XDG-OPEN SYMLINK!";
                return false;
            }
        }
        return true;
    }
    static const QString windowManager()
    {
        QString wm = "openbox";
        return wm;
    }
    static const QString windowManagerConf()
    {
        QString conf = QString("%1/rc.xml").arg(configDir());
        if (!QFile::exists(conf)) {
            qDebug() << "RC XML MISSING!";
            QFile rc(conf);
            QFile def(":/theme/rc.xml");
            if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qDebug() << "OPEN FALLBACK RC XML";
                if (rc.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    rc.write(def.readAll());
                    qDebug() << "WRITE NEW RC XML";
                    rc.close();
                }
                def.close();
            }
        }
        return conf;
    }
    static const QString windowManagerCmdStart()
    {
        QString cmd = QString("%1 --config-file %2").arg(windowManager()).arg(windowManagerConf());
        return cmd;
    }
    static const QString windowManagerCmdReConf()
    {
        QString reconf = QString("%1 --reconfigure").arg(windowManagerCmdStart());
        return reconf;
    }
    static const QString dracoStyleConf()
    {
        QString style = QString("%1/draco.qss").arg(configDir());
        if (!QFile::exists(style)) { return ":/theme/draco.qss"; }
        return style;
    }
    static const QString panelStyleSheet()
    {
        QString result;
        QFile qss(dracoStyleConf());
        if (qss.open(QIODevice::ReadOnly|QIODevice::Text)) {
            result = qss.readAll();
            qss.close();
        }
        return result;
    }
    static const QString desktopStyleSheet()
    {
        QString result;
        QFile qss(dracoStyleConf());
        if (qss.open(QIODevice::ReadOnly|QIODevice::Text)) {
            result = qss.readAll();
            qss.close();
        }
        return result;
    }
    static void themeEngineCheckConf()
    {
        QString conf = QString("%1/.config/qt5ct/qt5ct.conf").arg(QDir::homePath());
        if (!QFile::exists(conf)) {
            qDebug() << "QT5CT CONF MISSING!";
            QFile file(conf);
            QFile def(":/theme/qt5ct.conf");
            if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qDebug() << "OPEN FALLBACK QT5CT CONF";
                if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    file.write(def.readAll());
                    qDebug() << "WRITE NEW QT5CT CONF";
                    file.close();
                }
                def.close();
            }
        }
    }
    static void checkGtk2Conf()
    {
        QString conf = QString("%1/.gtkrc-2.0").arg(QDir::homePath());
        if (!QFile::exists(conf)) {
            qDebug() << "GTK2 CONF MISSING!";
            QFile file(conf);
            QFile def(":/theme/gtkrc-2.0");
            if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qDebug() << "OPEN FALLBACK GTK2 CONF";
                if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    file.write(def.readAll());
                    qDebug() << "WRITE NEW GTK2 CONF";
                    file.close();
                }
                def.close();
            }
        }
    }
    static void checkGtk3Conf()
    {
        QString gtkdir = QString("%1/.config/gtk-3.0").arg(QDir::homePath());
        QString conf = QString("%1/%2/settings.ini").arg(QDir::homePath()).arg(gtkdir);
        if (!QFile::exists(conf)) {
            QDir dir(gtkdir);
            if (!dir.exists()) { dir.mkpath(gtkdir); }
            qDebug() << "GTK3 CONF MISSING!";
            QFile file(conf);
            QFile def(":/theme/settings.ini");
            if (def.open(QIODevice::ReadOnly|QIODevice::Text)) {
                qDebug() << "OPEN FALLBACK GTK3 CONF";
                if (file.open(QIODevice::WriteOnly|QIODevice::Text)) {
                    file.write(def.readAll());
                    qDebug() << "WRITE NEW GTK3 CONF";
                    file.close();
                }
                def.close();
            }
        }
    }
    static void checkConfigs()
    {
        xdgOpenCheck();
        themeEngineCheckConf();
        checkGtk2Conf();
        checkGtk3Conf();
    }
};

#endif // COMMON_H
