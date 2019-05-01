/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com> All rights reserved.
#
# Available under the 3-clause BSD license
# See the LICENSE file for full details
#
*/

//===========================================
//  Lumina-DE source code
//  Copyright (c) 2016, Henry Hu
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================

#ifndef DESKTOP_LDESKTOPBACKGROUND_H
#define DESKTOP_LDESKTOPBACKGROUND_H

#include <QString>
#include <QWidget>
#include <QPixmap>

class LDesktopBackground: public QWidget
{
    Q_OBJECT

public:
    LDesktopBackground();
    virtual ~LDesktopBackground();

    virtual void paintEvent(QPaintEvent*);
    static QPixmap setBackground(const QString&,
                                 const QString&,
                                 QRect geom);

private:
    QPixmap *bgPixmap;
};

#endif // DESKTOP_LDESKTOPBACKGROUND_H
