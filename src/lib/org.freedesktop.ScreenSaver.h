/*
# PowerKit <https://github.com/rodlie/powerkit>
# Copyright (c) 2018, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
*/

#ifndef SCREENSAVER_H
#define SCREENSAVER_H

#include <QObject>
#include <QTimer>
#include <QTime>
#include <QMap>
#include <QString>

class ScreenSaver : public QObject
{
    Q_OBJECT

public:
    explicit ScreenSaver(QObject *parent = NULL);

private:
    QTimer timer;
    QMap<quint32, QTime> clients;

signals:
    void newInhibit(const QString &application,
                    const QString &reason,
                    quint32 cookie);
    void removedInhibit(quint32 cookie);

private slots:
    int randInt(int low, int high);
    quint32 genCookie();
    void checkForExpiredClients();
    bool canInhibit();
    void timeOut();
    void pingPM();

public slots:
    void SimulateUserActivity();
    quint32 Inhibit(const QString &application,
                    const QString &reason);
    void UnInhibit(quint32 cookie);
};

#endif // SCREENSAVER_H
