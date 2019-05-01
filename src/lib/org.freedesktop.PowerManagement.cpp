/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

#include "org.freedesktop.PowerManagement.h"
#include <QMapIterator>
#include <QDBusConnection>
#include <QCoreApplication>
#include <QProcess>

#include "power_def.h"

PowerManagement::PowerManagement(QObject *parent) : QObject(parent)
{
    timer.setInterval(PM_TIMEOUT);
    connect(&timer, SIGNAL(timeout()),
            this, SLOT(timeOut()));
    timer.start();
}

int PowerManagement::randInt(int low, int high)
{
    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
    return qrand() % ((high + 1) - low) + low;
}

quint32 PowerManagement::genCookie()
{
    int low = 0;
    int high = 1000;
    quint32 cookie = (quint32)randInt(low, high);
    while(!clients.contains(cookie)) {
        if (!clients.contains(cookie)) { clients[cookie] = QTime::currentTime(); }
        else { cookie = (quint32)randInt(low, high); }
    }
    return cookie;
}

void PowerManagement::checkForExpiredClients()
{
    QMapIterator<quint32, QTime> client(clients);
    while (client.hasNext()) {
        client.next();
        if (client.value()
            .secsTo(QTime::currentTime())>=PM_MAX_INHIBIT) {
            clients.remove(client.key());
        }
    }
}

bool PowerManagement::canInhibit()
{
    checkForExpiredClients();
    if (clients.size()>0) { return true; }
    return false;
}

void PowerManagement::timeOut()
{
    if (canInhibit()) { SimulateUserActivity(); }
}

void PowerManagement::SimulateUserActivity()
{
    emit HasInhibitChanged(true);
}

quint32 PowerManagement::Inhibit(const QString &application,
                                 const QString &reason)
{
    quint32 cookie = genCookie();
    timeOut();
    emit newInhibit(application, reason, cookie);
    emit HasInhibitChanged(canInhibit());
    return cookie;
}

void PowerManagement::UnInhibit(quint32 cookie)
{
    if (clients.contains(cookie)) { clients.remove(cookie); }
    timeOut();
    emit removedInhibit(cookie);
    emit HasInhibitChanged(canInhibit());
}

bool PowerManagement::HasInhibit()
{
    return canInhibit();
}
