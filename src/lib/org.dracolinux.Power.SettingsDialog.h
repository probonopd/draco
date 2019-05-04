/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#ifndef DIALOG_H
#define DIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QSpinBox>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QIcon>
#include <QLabel>
#include <QPixmap>
#include <QTabWidget>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QMessageBox>
#include <QApplication>
#include <QDebug>
#include <QSlider>
#include <QGroupBox>
#include <QDateTime>
#include <QScrollArea>

class Dialog : public QDialog
{
    Q_OBJECT

public:
   explicit Dialog(QWidget *parent = nullptr);
   ~Dialog();

private:
    QDBusInterface *dbus;
    QComboBox *lidActionBattery;
    QComboBox *lidActionAC;
    QComboBox *criticalActionBattery;
    QSpinBox *criticalBattery;
    QSpinBox *autoSleepBattery;
    QSpinBox *autoSleepAC;
    QCheckBox *desktopSS;
    QCheckBox *desktopPM;
    QCheckBox *showNotifications;
    QCheckBox *showSystemTray;
    QCheckBox *disableLidAction;
    QComboBox *autoSleepBatteryAction;
    QComboBox *autoSleepACAction;
    QString backlightDevice;
    QSlider *backlightSliderBattery;
    QSlider *backlightSliderAC;
    QCheckBox *backlightBatteryCheck;
    QCheckBox *backlightACCheck;
    QCheckBox *backlightBatteryLowerCheck;
    QCheckBox *backlightACHigherCheck;
    QCheckBox *warnOnLowBattery;
    QCheckBox *warnOnVeryLowBattery;

    QCheckBox *notifyOnBattery;
    QCheckBox *notifyOnAC;
    QLabel *lidActionACLabel;
    QLabel *lidActionBatteryLabel;
    QLabel *batteryBacklightLabel;
    QLabel *acBacklightLabel;
    QCheckBox *backlightMouseWheel;
    QCheckBox *suspendLockScreen;
    QCheckBox *resumeLockScreen;
    QCheckBox *bypassKernel;

private slots:
    void setupWidgets();
    void populate();
    void loadSettings();
    void saveSettings();
    void setDefaultAction(QComboBox *box, int action);
    void setDefaultAction(QSpinBox *box, int action);
    void setDefaultAction(QComboBox *box, QString value);
    void handleLidActionBattery(int index);
    void handleLidActionAC(int index);
    void handleCriticalAction(int index);
    void handleCriticalBattery(int value);
    void handleAutoSleepBattery(int value);
    void handleAutoSleepAC(int value);
    void handleDesktopSS(bool triggered);
    void handleDesktopPM(bool triggered);
    void handleShowNotifications(bool triggered);
    void handleShowSystemTray(bool triggered);
    void handleDisableLidAction(bool triggered);
    void handleAutoSleepBatteryAction(int index);
    void handleAutoSleepACAction(int index);
    void checkPerms();
    void handleBacklightBatteryCheck(bool triggered);
    void handleBacklightACCheck(bool triggered);
    void handleBacklightBatterySlider(int value);
    void handleBacklightACSlider(int value);
    void hibernateWarn();
    void sleepWarn();
    void handleBacklightBatteryCheckLower(bool triggered);
    void handleBacklightACCheckHigher(bool triggered);
    void enableBacklight(bool enabled);
    void handleWarnOnLowBattery(bool triggered);
    void handleWarnOnVeryLowBattery(bool triggered);
    void handleNotifyBattery(bool triggered);
    void handleNotifyAC(bool triggered);
    void enableLid(bool enabled);
    void handleBacklightMouseWheel(bool triggered);
    void handleSuspendLockScreen(bool triggered);
    void handleResumeLockScreen(bool triggered);
    void handleKernelBypass(bool triggered);
};

#endif // DIALOG_H
