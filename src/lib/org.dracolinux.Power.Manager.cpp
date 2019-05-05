/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#include "org.dracolinux.Power.Manager.h"
#include "power_def.h"
#include "draco.h"

#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusPendingReply>
#include <QXmlStreamReader>
#include <QProcess>
#include <QMapIterator>
#include <QDebug>
#include <QDBusReply>

Power::Power(QObject *parent) : QObject(parent)
  , upower(nullptr)
  , logind(nullptr)
  , ckit(nullptr)
  , pmd(nullptr)
  , wasDocked(false)
  , wasLidClosed(false)
  , wasOnBattery(false)
  , wakeAlarm(false)
  , suspendWakeupBattery(0)
  , suspendWakeupAC(0)
  , lockScreenOnSuspend(true)
  , lockScreenOnResume(false)
{
    setup();
    timer.setInterval(TIMEOUT_CHECK);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(check()));
    timer.start();
}

Power::~Power()
{
    clearDevices();
    releaseSuspendLock();
}

QMap<QString, Device *> Power::getDevices()
{
    return devices;
}

bool Power::availableService(const QString &service,
                          const QString &path,
                          const QString &interface)
{
    QDBusInterface iface(service,
                         path,
                         interface,
                         QDBusConnection::systemBus());
    if (iface.isValid()) { return true; }
    return false;
}

bool Power::availableAction(const Power::PKMethod &method,
                               const Power::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKConsoleKit:
        service = CONSOLEKIT_SERVICE;
        path = CONSOLEKIT_PATH;
        interface = CONSOLEKIT_MANAGER;
        break;
    case PKLogind:
        service = LOGIND_SERVICE;
        path = LOGIND_PATH;
        interface = LOGIND_MANAGER;
        break;
    case PKUPower:
        service = UPOWER_SERVICE;
        path = UPOWER_PATH;
        interface = UPOWER_MANAGER;
        break;
    default:
        return false;
    }
    switch (method) {
    case PKCanRestart:
        cmd = PK_CAN_RESTART;
        break;
    case PKCanPowerOff:
        cmd = PK_CAN_POWEROFF;
        break;
    case PKCanSuspend:
        cmd = PK_CAN_SUSPEND;
        break;
    case PKCanHibernate:
        cmd = PK_CAN_HIBERNATE;
        break;
    case PKCanHybridSleep:
        cmd = PK_CAN_HYBRIDSLEEP;
        break;
    case PKSuspendAllowed:
        cmd = PK_SUSPEND_ALLOWED;
        break;
    case PKHibernateAllowed:
        cmd = PK_HIBERNATE_ALLOWED;
        break;
    default:
        return false;
    }
    QDBusInterface iface(service, path, interface,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return false; }
    QDBusMessage reply = iface.call(cmd);
    if (reply.arguments().first().toString() == DBUS_OK_REPLY) { return true; }
    bool result = reply.arguments().first().toBool();
    if (!reply.errorMessage().isEmpty()) { result = false; }
    return result;
}

QString Power::executeAction(const Power::PKAction &action,
                                const Power::PKBackend &backend)
{
    QString service, path, interface, cmd;
    switch (backend) {
    case PKConsoleKit:
        service = CONSOLEKIT_SERVICE;
        path = CONSOLEKIT_PATH;
        interface = CONSOLEKIT_MANAGER;
        break;
    case PKLogind:
        service = LOGIND_SERVICE;
        path = LOGIND_PATH;
        interface = LOGIND_MANAGER;
        break;
    case PKUPower:
        service = UPOWER_SERVICE;
        path = UPOWER_PATH;
        interface = UPOWER_MANAGER;
        break;
    default:
        return QObject::tr(PK_NO_BACKEND);
    }
    switch (action) {
    case PKRestartAction:
        cmd = PK_RESTART;
        break;
    case PKPowerOffAction:
        cmd = PK_POWEROFF;
        break;
    case PKSuspendAction:
        cmd = PK_SUSPEND;
        break;
    case PKHibernateAction:
        cmd = PK_HIBERNATE;
        break;
    case PKHybridSleepAction:
        cmd = PK_HYBRIDSLEEP;
        break;
    default:
        return QObject::tr(PK_NO_ACTION);
    }
    QDBusInterface iface(service, path, interface,
                         QDBusConnection::systemBus());
    if (!iface.isValid()) { return QObject::tr(DBUS_FAILED_CONN); }

    QDBusMessage reply;
    if (backend == PKUPower) { reply = iface.call(cmd); }
    else { reply = iface.call(cmd, true); }

    return reply.errorMessage();
}

QStringList Power::find()
{
    QStringList result;
    QDBusMessage call = QDBusMessage::createMethodCall(UPOWER_SERVICE,
                                                       QString("%1/devices").arg(UPOWER_PATH),
                                                       DBUS_INTROSPECTABLE,
                                                       "Introspect");
    QDBusPendingReply<QString> reply = QDBusConnection::systemBus().call(call);
    if (reply.isError()) {
        qWarning() << "powerkit find devices failed, check the upower service!!!";
        return result;
    }
    QList<QDBusObjectPath> objects;
    QXmlStreamReader xml(reply.value());
    if (xml.hasError()) { return result; }
    while (!xml.atEnd()) {
        xml.readNext();
        if (xml.tokenType() == QXmlStreamReader::StartElement &&
            xml.name().toString() == "node" ) {
            QString name = xml.attributes().value("name").toString();
            if (!name.isEmpty()) { objects << QDBusObjectPath(UPOWER_DEVICES + name); }
        }
    }
    foreach (QDBusObjectPath device, objects) {
        result << device.path();
    }
    return result;
}

void Power::setup()
{
    QDBusConnection system = QDBusConnection::systemBus();
    if (system.isConnected()) {
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(const QDBusObjectPath&)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_ADDED,
                       this,
                       SLOT(deviceAdded(const QString&)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(const QDBusObjectPath&)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_REMOVED,
                       this,
                       SLOT(deviceRemoved(const QString&)));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_CHANGED,
                       this,
                       SLOT(deviceChanged()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       DBUS_DEVICE_CHANGED,
                       this,
                       SLOT(deviceChanged()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       UPOWER_NOTIFY_RESUME,
                       this,
                       SLOT(handleResume()));
        system.connect(UPOWER_SERVICE,
                       UPOWER_PATH,
                       UPOWER_SERVICE,
                       UPOWER_NOTIFY_SLEEP,
                       this,
                       SLOT(handleSuspend()));
        system.connect(LOGIND_SERVICE,
                       LOGIND_PATH,
                       LOGIND_MANAGER,
                       PK_PREPARE_FOR_SUSPEND,
                       this,
                       SLOT(handlePrepareForSuspend(bool)));
        system.connect(CONSOLEKIT_SERVICE,
                       CONSOLEKIT_PATH,
                       CONSOLEKIT_MANAGER,
                       PK_PREPARE_FOR_SLEEP,
                       this,
                       SLOT(handlePrepareForSuspend(bool)));
        if (upower == nullptr) {
            upower = new QDBusInterface(UPOWER_SERVICE,
                                        UPOWER_PATH,
                                        UPOWER_MANAGER,
                                        system,
                                        this);
        }
        if (logind == nullptr) {
            logind = new QDBusInterface(LOGIND_SERVICE,
                                        LOGIND_PATH,
                                        LOGIND_MANAGER,
                                        system,
                                        this);
        }
        if (ckit == nullptr) {
            ckit = new QDBusInterface(CONSOLEKIT_SERVICE,
                                      CONSOLEKIT_PATH,
                                      CONSOLEKIT_MANAGER,
                                      system,
                                      this);
        }
        if (pmd == nullptr) {
            pmd = new QDBusInterface(Draco::powerdSessionName(),
                                     Draco::powerdSessionPath(),
                                     QString("%1.Manager").arg(Draco::powerdSessionName()),
                                     system,
                                     this);
        }
        if (!suspendLock) { registerSuspendLock(); }
        scan();
    }
}

void Power::check()
{
    qDebug() << "PK CHECK";
    if (!QDBusConnection::systemBus().isConnected()) {
        setup();
        return;
    }
    if (!suspendLock) { registerSuspendLock(); }
    if (!upower->isValid()) { scan(); }
}

void Power::scan()
{
    qDebug() << "PK SCAN";
    QStringList foundDevices = find();
    for (int i=0; i < foundDevices.size(); i++) {
        QString foundDevicePath = foundDevices.at(i);
        if (devices.contains(foundDevicePath)) { continue; }
        Device *newDevice = new Device(foundDevicePath, this);
        connect(newDevice,
                SIGNAL(deviceChanged(QString)),
                this,
                SLOT(handleDeviceChanged(QString)));
        devices[foundDevicePath] = newDevice;
    }
    UpdateDevices();
    emit UpdatedDevices();
}

void Power::deviceAdded(const QDBusObjectPath &obj)
{
    qDebug() << "PK DEVICE ADDED" << obj.path();
    deviceAdded(obj.path());
}

void Power::deviceAdded(const QString &path)
{
    qDebug() << "PK DEVICE ADDED" << path;
    if (!upower->isValid()) { return; }
    if (path.startsWith(QString(DBUS_JOBS).arg(UPOWER_PATH))) { return; }
    emit DeviceWasAdded(path);
    scan();
}

void Power::deviceRemoved(const QDBusObjectPath &obj)
{
    qDebug() << "PK DEVICE REMOVED" << obj.path();
    deviceRemoved(obj.path());
}

void Power::deviceRemoved(const QString &path)
{
    qDebug() << "PK DEVICE REMOVED" << path;
    if (!upower->isValid()) { return; }
    bool deviceExists = devices.contains(path);
    if (path.startsWith(QString(DBUS_JOBS).arg(UPOWER_PATH))) { return; }
    if (deviceExists) {
        if (find().contains(path)) { return; }
        delete devices.take(path);
        emit DeviceWasRemoved(path);
    }
    scan();
}

void Power::deviceChanged()
{
    qDebug() << "PK DEVICE(S) CHANGED";
    if (wasLidClosed != LidIsClosed()) {
        if (!wasLidClosed && LidIsClosed()) {
            emit LidClosed();
        } else if (wasLidClosed && !LidIsClosed()) {
            emit LidOpened();
        }
    }
    wasLidClosed = LidIsClosed();

    if (wasOnBattery != OnBattery()) {
        if (!wasOnBattery && OnBattery()) {
            emit SwitchedToBattery();
        } else if (wasOnBattery && !OnBattery()) {
            emit SwitchedToAC();
        }
    }
    wasOnBattery = OnBattery();

    emit UpdatedDevices();
}

void Power::handleDeviceChanged(const QString &device)
{
    qDebug() << "PK HANDLE DEVICE CHANGED" << device;
    if (device.isEmpty()) { return; }
    deviceChanged();
}

void Power::handleResume()
{
    if (HasLogind() || HasConsoleKit()) { return; }
    qDebug() << "handle resume from upower";
    handlePrepareForSuspend(false);
}

void Power::handleSuspend()
{
    if (HasLogind() || HasConsoleKit()) { return; }
    qDebug() << "handle suspend from upower";
    if (lockScreenOnSuspend) { LockScreen(); }
    emit PrepareForSuspend();
}

void Power::handlePrepareForSuspend(bool prepare)
{
    qDebug() << "handle prepare for suspend/resume from consolekit/logind" << prepare;
    if (prepare) {
        if (lockScreenOnSuspend) { LockScreen(); }
        emit PrepareForSuspend();
        releaseSuspendLock(); // we are ready for suspend
    }
    else { // resume
        UpdateDevices();
        if (lockScreenOnResume) { LockScreen(); }
        if (hasWakeAlarm() &&
             wakeAlarmDate.isValid() &&
             CanHibernate())
        {
            qDebug() << "we may have a wake alarm" << wakeAlarmDate;
            QDateTime currentDate = QDateTime::currentDateTime();
            if (currentDate>=wakeAlarmDate && wakeAlarmDate.secsTo(currentDate)<300) {
                qDebug() << "wake alarm is active, that means we should hibernate";
                clearWakeAlarm();
                Hibernate();
                return;
            }
        }
        clearWakeAlarm();
        emit PrepareForResume();
    }
}

void Power::clearDevices()
{
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        delete device.value();
    }
    devices.clear();
}

void Power::handleNewInhibitScreenSaver(const QString &application, const QString &reason, quint32 cookie)
{
    qDebug() << "PK HANDLE NEW SCREEN SAVER INHIBITOR" << application << reason << cookie;
    Q_UNUSED(reason)
    ssInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Power::handleNewInhibitPowerManagement(const QString &application, const QString &reason, quint32 cookie)
{
    qDebug() << "PK HANDLE NEW POWER INHIBITOR" << application << reason << cookie;
    Q_UNUSED(reason)
    pmInhibitors[cookie] = application;
    emit UpdatedInhibitors();
}

void Power::handleDelInhibitScreenSaver(quint32 cookie)
{
    qDebug() << "PK HANDLE REMOVE SCREEN SAVER COOKIE" << cookie;
    if (ssInhibitors.contains(cookie)) {
        ssInhibitors.remove(cookie);
        emit UpdatedInhibitors();
    }
}

void Power::handleDelInhibitPowerManagement(quint32 cookie)
{
    qDebug() << "PK HANDLE REMOVE POWER COOKIE" << cookie;
    if (pmInhibitors.contains(cookie)) {
        pmInhibitors.remove(cookie);
        emit UpdatedInhibitors();
    }
}

bool Power::registerSuspendLock()
{
    if (suspendLock) { return false; }
    qDebug() << "register suspend lock";
    QDBusReply<QDBusUnixFileDescriptor> reply;
    if (HasLogind() && logind->isValid()) {
        reply = ckit->call("Inhibit",
                           "sleep",
                           "powerkit",
                           "Lock screen etc",
                           "delay");
    } else if (HasConsoleKit() && ckit->isValid()) {
        reply = ckit->call("Inhibit",
                           "sleep",
                           "powerkit",
                           "Lock screen etc",
                           "delay");
    }
    if (reply.isValid()) {
        suspendLock.reset(new QDBusUnixFileDescriptor(reply.value()));
        return true;
    } else {
        qDebug() << reply.error();
    }
    return false;
}

void Power::setWakeAlarmFromSettings()
{
    if (!CanHibernate()) { return; }
    int wmin = OnBattery()?suspendWakeupBattery:suspendWakeupAC;
    if (wmin>0) {
        qDebug() << "we need to set a wake alarm" << wmin << "min from now";
        QDateTime date = QDateTime::currentDateTime().addSecs(wmin*60);
        setWakeAlarm(date);
    }
}

bool Power::HasConsoleKit()
{
    qDebug() << "PK CHECK FOR CONSOLEKIT";
    if (ckit) { return ckit->isValid(); }
    return availableService(CONSOLEKIT_SERVICE,
                            CONSOLEKIT_PATH,
                            CONSOLEKIT_MANAGER);
}

bool Power::HasLogind()
{
    qDebug() << "PK CHECK FOR LOGIND";
    if (logind) { return logind->isValid(); }
    return availableService(LOGIND_SERVICE,
                            LOGIND_PATH,
                            LOGIND_MANAGER);
}

bool Power::HasUPower()
{
    qDebug() << "PK CHECK FOR UPOWER";
    if (upower) { return upower->isValid(); }
    return availableService(UPOWER_SERVICE,
                            UPOWER_PATH,
                            UPOWER_MANAGER);
}

bool Power::hasPMD()
{
    qDebug() << "PK CHECK FOR POWERKITD";
    return availableService(Draco::powerdSessionName(),
                            Draco::powerdSessionPath(),
                            QString("%1.Manager").arg(Draco::powerdSessionName()));
}

bool Power::hasWakeAlarm()
{
    qDebug() << "PK CHECK FOR WAKE ALARM";
    return wakeAlarm;
}

bool Power::CanRestart()
{
    qDebug() << "PK CHECK FOR RESTART SUPPORT";
    if (HasLogind()) {
        return availableAction(PKCanRestart, PKLogind);
    } else if (HasConsoleKit()) {
        return availableAction(PKCanRestart, PKConsoleKit);
    }
    return false;
}

bool Power::CanPowerOff()
{
    qDebug() << "PK CHECK FOR SHUTDOWN SUPPORT";
    if (HasLogind()) {
        return availableAction(PKCanPowerOff, PKLogind);
    } else if (HasConsoleKit()) {
        return availableAction(PKCanPowerOff, PKConsoleKit);
    }
    return false;
}

bool Power::CanSuspend()
{
    qDebug() << "PK CHECK FOR SUSPEND SUPPORT";
    if (HasLogind()) {
        return availableAction(PKCanSuspend, PKLogind);
    } else if (HasConsoleKit()) {
        return availableAction(PKCanSuspend, PKConsoleKit);
    } else if (HasUPower()) {
        return availableAction(PKSuspendAllowed, PKUPower);
    }
    return false;
}

bool Power::CanHibernate()
{
    qDebug() << "PK CHECK FOR HIBERNATE SUPPORT";
    if (HasLogind()) {
        return availableAction(PKCanHibernate, PKLogind);
    } else if (HasConsoleKit()) {
        return availableAction(PKCanHibernate, PKConsoleKit);
    } else if (HasUPower()) {
        return availableAction(PKHibernateAllowed, PKUPower);
    }
    return false;
}

bool Power::CanHybridSleep()
{
    qDebug() << "PK CHECK FOR HYBRIDSLEEP SUPPORT";
    if (HasLogind()) {
        return availableAction(PKCanHybridSleep, PKLogind);
    } else if (HasConsoleKit()) {
        return availableAction(PKCanHybridSleep, PKConsoleKit);
    }
    return false;
}

QString Power::Restart()
{
    qDebug() << "try to restart";
    if (HasLogind()) {
        return executeAction(PKRestartAction, PKLogind);
    } else if (HasConsoleKit()) {
        return executeAction(PKRestartAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString Power::PowerOff()
{
    qDebug() << "try to poweroff";
    if (HasLogind()) {
        return executeAction(PKPowerOffAction, PKLogind);
    } else if (HasConsoleKit()) {
        return executeAction(PKPowerOffAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString Power::Suspend()
{
    qDebug() << "try to suspend";
    if (lockScreenOnSuspend) { LockScreen(); }
    if (HasLogind()) {
        setWakeAlarmFromSettings();
        return executeAction(PKSuspendAction, PKLogind);
    } else if (HasConsoleKit()) {
        setWakeAlarmFromSettings();
        return executeAction(PKSuspendAction, PKConsoleKit);
    } else if (HasUPower()) {
        return executeAction(PKSuspendAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString Power::Hibernate()
{
    qDebug() << "try to hibernate";
    if (lockScreenOnSuspend) { LockScreen(); }
    if (HasLogind()) {
        return executeAction(PKHibernateAction, PKLogind);
    } else if (HasConsoleKit()) {
        return executeAction(PKHibernateAction, PKConsoleKit);
    } else if (HasUPower()) {
        return executeAction(PKHibernateAction, PKUPower);
    }
    return QObject::tr(PK_NO_BACKEND);
}

QString Power::HybridSleep()
{
    qDebug() << "try to hybridsleep";
    if (lockScreenOnSuspend) { LockScreen(); }
    if (HasLogind()) {
        return executeAction(PKHybridSleepAction, PKLogind);
    } else if (HasConsoleKit()) {
        return executeAction(PKHybridSleepAction, PKConsoleKit);
    }
    return QObject::tr(PK_NO_BACKEND);
}

bool Power::setWakeAlarm(const QDateTime &date)
{
    qDebug() << "try to set wake alarm" << date;
    if (pmd && date.isValid() && CanHibernate()) {
        if (!pmd->isValid()) { return false; }
        QDBusMessage reply = pmd->call("SetWakeAlarm",
                                       date.toString("yyyy-MM-dd HH:mm:ss"));
        bool alarm = reply.arguments().first().toBool() && reply.errorMessage().isEmpty();
        qDebug() << "WAKE OK?" << alarm;
        wakeAlarm = alarm;
        if (alarm) {
            qDebug() << "wake alarm was set to" << date;
            wakeAlarmDate = date;
        }
        return alarm;
    }
    return false;
}

void Power::clearWakeAlarm()
{
    wakeAlarm = false;
}

bool Power::IsDocked()
{
    qDebug() << "PK CHECK FOR DOCK";
    if (logind->isValid()) { return logind->property(LOGIND_DOCKED).toBool(); }
    if (upower->isValid()) { return upower->property(UPOWER_DOCKED).toBool(); }
    return false;
}

bool Power::LidIsPresent()
{
    qDebug() << "PK CHECK FOR LID";
    if (upower->isValid()) { return upower->property(UPOWER_LID_IS_PRESENT).toBool(); }
    return false;
}

bool Power::LidIsClosed()
{
    qDebug() << "PK CHECK LID STATUS";
    if (upower->isValid()) { return upower->property(UPOWER_LID_IS_CLOSED).toBool(); }
    return false;
}

bool Power::OnBattery()
{
    qDebug() << "PK CHECK FOR ON BATTERY";
    if (upower->isValid()) { return upower->property(UPOWER_ON_BATTERY).toBool(); }
    return false;
}

double Power::BatteryLeft()
{
    qDebug() << "PK CHECK FOR BATTERY LEFT";
    if (OnBattery()) { UpdateBattery(); }
    double batteryLeft = 0;
    QMapIterator<QString, Device*> device(devices);
    int batteries = 0;
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        {
            batteryLeft += device.value()->percentage;
            batteries++;
        } else { continue; }
    }
    return batteryLeft/batteries;
}

void Power::LockScreen()
{
    qDebug() << "lock screen";
    QProcess proc;
    proc.start(XSCREENSAVER_LOCK);
    proc.waitForFinished();
    proc.close();
}

bool Power::HasBattery()
{
    qDebug() << "PK CHECK IF HAS BATTERY";
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) { return true; }
    }
    return false;
}

qlonglong Power::TimeToEmpty()
{
    qDebug() << "PK CHECK FOR TIME TO EMPTY";
    if (OnBattery()) { UpdateBattery(); }
    qlonglong result = 0;
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        { result += device.value()->timeToEmpty; }
    }
    return result;
}

qlonglong Power::TimeToFull()
{
    qDebug() << "PK CHECK FOR TIME TO FULL";
    if (OnBattery()) { UpdateBattery(); }
    qlonglong result = 0;
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery &&
            device.value()->isPresent &&
            !device.value()->nativePath.isEmpty())
        { result += device.value()->timeToFull; }
    }
    return result;
}

void Power::UpdateDevices()
{
    qDebug() << "PK UPDATE DEVICES";
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        device.value()->update();
    }
}

void Power::UpdateBattery()
{
    qDebug() << "PK UPDATE BATTERY";
    QMapIterator<QString, Device*> device(devices);
    while (device.hasNext()) {
        device.next();
        if (device.value()->isBattery) {
            device.value()->updateBattery();
        }
    }
}

void Power::UpdateConfig()
{
    qDebug() << "PK UPDATE CONFIG";
    emit Update();
}

QStringList Power::ScreenSaverInhibitors()
{
    QStringList result;
    QMapIterator<quint32, QString> i(ssInhibitors);
    while (i.hasNext()) {
        i.next();
        result << i.value();
    }
    return result;
}

QStringList Power::PowerManagementInhibitors()
{
    QStringList result;
    QMapIterator<quint32, QString> i(pmInhibitors);
    while (i.hasNext()) {
        i.next();
        result << i.value();
    }
    return result;
}

const QDateTime Power::getWakeAlarm()
{
    return wakeAlarmDate;
}

void Power::releaseSuspendLock()
{
    qDebug() << "release suspend lock";
    suspendLock.reset(NULL);
}

void Power::setSuspendWakeAlarmOnBattery(int value)
{
    qDebug() << "set suspend wake alarm on battery" << value;
    suspendWakeupBattery = value;
}

void Power::setSuspendWakeAlarmOnAC(int value)
{
    qDebug() << "set suspend wake alarm on ac" << value;
    suspendWakeupAC = value;
}

void Power::setLockScreenOnSuspend(bool lock)
{
    qDebug() << "set lock screen on suspend" << lock;
    lockScreenOnSuspend = lock;
}

void Power::setLockScreenOnResume(bool lock)
{
    qDebug() << "set lock screen on resume" << lock;
    lockScreenOnResume = lock;
}

bool Power::setDisplayBacklight(const QString &device, int value)
{
    qDebug() << "PK SET DISPLAY BACKLIGHT" << device << value;
    if (!pmd) { return false; }
    if (!pmd->isValid()) { return false; }
    QDBusMessage reply = pmd->call("SetDisplayBacklight",
                                   device,
                                   value);
    bool backlight = reply.arguments().first().toBool() && reply.errorMessage().isEmpty();
    qDebug() << "BACKLIGHT OK?" << backlight << reply.errorMessage();
    return backlight;
}
