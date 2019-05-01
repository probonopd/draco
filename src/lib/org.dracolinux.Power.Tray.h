/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SYSTRAY_H
#define SYSTRAY_H

#include <QObject>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <QSettings>
#include <QPainter>
#include <QMap>
#include <QMapIterator>
#include <QProcess>
#include <QMap>
#include <QFileSystemWatcher>
#include <QEvent>
#include <QWheelEvent>
#include <QMenu>
#include <QLabel>
#include <QFrame>
#include <QWidgetAction>
#include <QSlider>
#include <QTreeWidget>
#include <QProgressBar>
#include <QTabWidget>

//#include "common.h"

#include "org.freedesktop.PowerManagement.h"
#include "org.freedesktop.ScreenSaver.h"
#include "org.dracolinux.Power.ScreenX11.h"
#include "org.dracolinux.Power.Manager.h"

//#include "powerkit_freedesktop_pm.h"
//#include "powerkit_freedesktop_ss.h"
//#include "powerkit_x11_screens.h"
//#include "powerkit.h"

#undef Bool
#undef Status
#include <X11/extensions/scrnsaver.h>
#undef CursorShape

#undef None
#undef KeyPress
#undef KeyRelease
#undef FocusIn
#undef FocusOut
#undef FontChange
#undef Expose
#undef FrameFeature
#undef Unsorted

#define XSCREENSAVER_RUN "xscreensaver -no-splash"

#define DEVICE_UUID Qt::UserRole+1
#define DEVICE_TYPE Qt::UserRole+2
#define MAX_WIDTH 150

class TrayIcon : public QSystemTrayIcon
{
    Q_OBJECT
public:
    enum WheelAction {
        WheelUp,
        WheelDown
    };
    TrayIcon(QObject *parent = 0)
        : QSystemTrayIcon(parent), wheel_delta(0) {}
    TrayIcon(const QIcon &icon, QObject *parent = 0)
        : QSystemTrayIcon(icon, parent), wheel_delta(0) {}
    bool event(QEvent *event);
signals:
    void wheel(TrayIcon::WheelAction action);
private:
    int wheel_delta;
};

class SysTray : public QObject
{
    Q_OBJECT

public:
    explicit SysTray(QObject *parent = NULL);
    ~SysTray();

private:
    TrayIcon *tray;
    PowerKit *man;
    PowerManagement *pm;
    ScreenSaver *ss;
    bool wasLowBattery;
    bool wasVeryLowBattery;
    int lowBatteryValue;
    int critBatteryValue;
    bool hasService;
    int lidActionBattery;
    int lidActionAC;
    int criticalAction;
    int autoSuspendBattery;
    int autoSuspendAC;
    QTimer *timer;
    int timeouts;
    bool showNotifications;
    bool desktopSS;
    bool desktopPM;
    bool showTray;
    bool disableLidOnExternalMonitors;
    int autoSuspendBatteryAction;
    int autoSuspendACAction;
    QProcess *xscreensaver;
    bool startupScreensaver;
    QMap<quint32,QString> ssInhibitors;
    QMap<quint32,QString> pmInhibitors;
    QString internalMonitor;
    QFileSystemWatcher *watcher;
    bool lidXrandr;
    bool lidWasClosed;
    QString backlightDevice;
    bool hasBacklight;
    bool backlightOnBattery;
    bool backlightOnAC;
    int backlightBatteryValue;
    int backlightACValue;
    bool backlightBatteryDisableIfLower;
    bool backlightACDisableIfHigher;
    QProcess *configDialog;
    bool warnOnLowBattery;
    bool warnOnVeryLowBattery;
    bool notifyOnBattery;
    bool notifyOnAC;
    bool backlightMouseWheel;
    bool ignoreKernelResume;

    QMenu *powerMenu;
    QAction *actSettings;
    QAction *actPowerOff;
    QAction *actRestart;
    QAction *actSuspend;
    QAction *actHibernate;
    QLabel *labelBatteryStatus;
    QLabel *labelBatteryIcon;
    //QLabel *labelBatteryLeft;
    QFrame *menuFrame;
    QWidgetAction *menuHeader;
    QSlider *backlightSlider;
    QLabel *backlightLabel;
    QFileSystemWatcher *backlightWatcher;
    QTreeWidget *deviceTree;
    QMap<QString,QProgressBar*> devicesProg;
    QTreeWidget *inhibitorTree;
    QTabWidget *powerTab;

private slots:
    void trayActivated(QSystemTrayIcon::ActivationReason reason);
    void checkDevices();
    void handleClosedLid();
    void handleOpenedLid();
    void handleOnBattery();
    void handleOnAC();
    void loadSettings();
    void registerService();
    void handleHasInhibitChanged(bool has_inhibit);
    void handleLow(double left);
    void handleVeryLow(double left);
    void handleCritical(double left);
    void drawBattery(double left);
    void timeout();
    int xIdle();
    void resetTimer();
    void setInternalMonitor();
    bool internalMonitorIsConnected();
    bool externalMonitorIsConnected();
    void handleNewInhibitScreenSaver(const QString &application,
                                     const QString &reason,
                                     quint32 cookie);
    void handleNewInhibitPowerManagement(const QString &application,
                                         const QString &reason,
                                         quint32 cookie);
    void handleDelInhibitScreenSaver(quint32 cookie);
    void handleDelInhibitPowerManagement(quint32 cookie);
    void handleScreensaverFinished(int exitcode);
    void showMessage(const QString &title,
                     const QString &msg,
                     bool critical = false);
    void handleConfChanged(const QString &file);
    void disableHibernate();
    void disableSuspend();
    void handlePrepareForSuspend();
    void handlePrepareForResume();
    void switchInternalMonitor(bool toggle);
    void handleTrayWheel(TrayIcon::WheelAction action);
    void handleDeviceChanged(const QString &path);
    void handleConfigDialogFinished(int result);
    void showConfigDialog();
    void populateMenu();
    void updateMenu();
    void updateBacklight(QString file);
    void handleBacklightSlider(int value);
    void updatePowerDevices();
    bool powerDeviceExists(QString uid);
    void powerDeviceRemove(QString uid);
    void getInhibitors();
};

#endif // SYSTRAY_H
