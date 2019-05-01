/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef POWER_CLIENT_H
#define POWER_CLIENT_H

#include <QDBusInterface>

class PowerClient
{
public:
    static double getBatteryLeft(QDBusInterface *iface);
    static bool hasBattery(QDBusInterface *iface);
    static bool onBattery(QDBusInterface *iface);
    static qlonglong timeToEmpty(QDBusInterface *iface);
    static bool canHibernate(QDBusInterface *iface);
    static bool canSuspend(QDBusInterface *iface);
    static bool canRestart(QDBusInterface *iface);
    static bool canPowerOff(QDBusInterface *iface);
    static bool lidIsPresent(QDBusInterface *iface);
    static void lockScreen(QDBusInterface *iface);
};

#endif // POWER_CLIENT_H
