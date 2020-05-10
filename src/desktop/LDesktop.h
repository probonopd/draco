/*
#
# Draco Desktop Environment <https://dracolinux.org>
# Copyright (c) 2019, Ole-André Rodlie <ole.andre.rodlie@gmail.com>
# All rights reserved.
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>
#
# This file incorporates work covered by the following copyright and
# permission notice:
#
# Lumina Desktop Environment (https://lumina-desktop.org)
# Copyright (c) 2012-2017, Ken Moore (moorekou@gmail.com)
# All rights reserved
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the organization nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
*/

//===========================================
//  Lumina-DE source code
//  Copyright (c) 2012-2015, Ken Moore
//  Available under the 3-clause BSD license
//===========================================

#ifndef DESKTOP_LDESKTOP_H
#define DESKTOP_LDESKTOP_H

#include <QCoreApplication>
#include <QSettings>
#include <QFile>
#include <QList>
#include <QDebug>
#include <QTimer>
#include <QFileSystemWatcher>
#include <QLabel>
#include <QWidgetAction>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QRegion>
#include <QInputDialog>

#include "LuminaXDG.h"
#include "LPanel.h"
#include "AppMenu.h"
#include "LDesktopPluginSpace.h"
#include "desktop-plugins/LDPlugin.h"
#include "LDesktopBackground.h"

class LDesktop : public QObject
{
    Q_OBJECT

public:
    LDesktop(int deskNum=0, bool setdefault = false);
    ~LDesktop();
    int Screen(); // return the screen number this object is managing
    void show();
    void hide();
    void prepareToClose();
    WId backgroundID();
    QRect availableScreenGeom();
    void UpdateGeometry();

public slots:
    void SystemAbout();
    void WallpaperSettings();
    void SystemLock();
    void SystemLogout();
    void SystemTerminal();
    //void SystemFileManager();
    void SystemApplication(QAction*);
    void checkResolution();

private:
    QSettings *settings;
    QTimer *bgtimer;
    QString DPREFIX, screenID;
    QRegion availDPArea;
    bool defaultdesktop, issyncing, usewinmenu, bgupdating;
    QStringList oldBGL;
    QList<LPanel*> PANELS;
    LDesktopPluginSpace *bgDesktop; // desktop plugin area
    QMenu *deskMenu, *winMenu, *desktopFolderActionMenu;
    QLabel *workspacelabel;
    QWidgetAction *wkspaceact;
    QList<LDPlugin*> PLUGINS;
    QString CBG; // current background
    QRect globalWorkRect;
    bool i_dlg_folder; // folder/file switch
    QInputDialog *inputDLG;

private slots:
    void InitDesktop();
    void SettingsChanged();
    void UnlockSettings() { issyncing=false; }
    void LocaleChanged();

    // Menu functions
    void UpdateMenu(bool fast = false);
    void ShowMenu()
    {
      UpdateMenu(true); // run the fast version
      deskMenu->popup(QCursor::pos());
    }
    void UpdateWinMenu();
    void winClicked(QAction*);

    // Desktop plugin system functions
    void UpdateDesktop();
    void RemoveDeskPlugin(QString);
    void IncreaseDesktopPluginIcons();
    void DecreaseDesktopPluginIcons();

    void UpdatePanels();
    void UpdateDesktopPluginArea(); // make sure the area is not underneath any panels
    void UpdateBackground();

    // Desktop Folder Interactions
    void i_dlg_finished(int ret);
    void NewDesktopFolder(QString name = "");
    void NewDesktopFile(QString name = "");
    void PasteInDesktop();
};
#endif
