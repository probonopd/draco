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
//  Copyright (c) 2012-2015, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================

#include "LDesktop.h"
#include "LSession.h"
#include <QClipboard>
#include <QMimeData>

#include "LuminaX11.h"
#include "LWinInfo.h"
#include <QScreen>
#include "draco.h"

#include "qtcopydialog.h"
#include "qtfilecopier.h"

LDesktop::LDesktop(int deskNum, bool setdefault) : QObject()
  , settings(Q_NULLPTR)
  , bgtimer(Q_NULLPTR)
  , defaultdesktop(false)
  , issyncing(false)
  , usewinmenu(false)
  , bgupdating(false)
  , bgDesktop(Q_NULLPTR)
  , deskMenu(Q_NULLPTR)
  , winMenu(Q_NULLPTR)
  , desktopFolderActionMenu(Q_NULLPTR)
  , workspacelabel(Q_NULLPTR)
  , wkspaceact(Q_NULLPTR)
  , i_dlg_folder(true)
  , inputDLG(Q_NULLPTR)
{
    screenID = QApplication::screens().at(deskNum)->name();
    DPREFIX = "desktop-"+screenID+"/";

    if (deskNum==0) { setdefault = true; }
    defaultdesktop = setdefault; //(desktop->screenGeometry(desktopnumber).x()==0);

    // Setup the internal variables
    settings = new QSettings(QSettings::UserScope,
                             DESKTOP_APP,
                             DE_DESKTOP_SETTINGS,
                             this);
    qDebug() << " - Desktop Settings File:" << settings->fileName();
    if (!QFile::exists(settings->fileName())) {
        settings->setValue(QString("%1background/filelist").arg(DPREFIX),
                           QStringList()<<"default");
        settings->sync();
    }

    QTimer::singleShot(1, this, SLOT(InitDesktop()));
}

LDesktop::~LDesktop()
{
    delete deskMenu;
    delete winMenu;
    delete desktopFolderActionMenu;
    delete workspacelabel;
    delete wkspaceact;
}

int LDesktop::Screen()
{
    QList<QScreen*> scrns = QApplication::screens();
    for (int i=0; i<scrns.length(); i++) {
        if (scrns[i]->name()==screenID) { return i; }
    }
    return -1;
}

void LDesktop::show()
{
    if (bgDesktop!=Q_NULLPTR){ bgDesktop->show(); }
    for (int i=0; i<PANELS.length(); i++) { PANELS[i]->show(); }
}

void LDesktop::hide()
{
    if (bgDesktop!=Q_NULLPTR){ bgDesktop->hide(); }
    for (int i=0; i<PANELS.length(); i++) { PANELS[i]->hide(); }
}

void LDesktop::prepareToClose()
{
    // Get any panels ready to close
    issyncing = true; // Stop handling any watcher events
    for (int i=0; i<PANELS.length(); i++) { PANELS[i]->prepareToClose(); PANELS.takeAt(i)->deleteLater(); i--; }
    // Now close down any desktop plugins
    // Remove all the current containers
    bgDesktop->cleanup();
}

WId LDesktop::backgroundID()
{
    if (bgDesktop!=Q_NULLPTR) { return bgDesktop->winId(); }
    else { return QX11Info::appRootWindow(); }
}

QRect LDesktop::availableScreenGeom()
{
    // Return a QRect containing the (global) screen area that is available (not under any panels)
    if (bgDesktop!=Q_NULLPTR) {
        return globalWorkRect; // saved from previous calculations
    } else {
        return LSession::handle()->screenGeom( Screen() );
    }
}

void LDesktop::UpdateGeometry()
{
    QTimer::singleShot(0, this, SLOT(UpdateBackground()));
    QTimer::singleShot(0, this, SLOT(UpdatePanels()));
}

void LDesktop::SystemLock()
{
    QProcess::startDetached("xscreensaver-command -lock");
}

void LDesktop::SystemLogout()
{
    LSession::handle()->systemWindow();
}

void LDesktop::SystemTerminal()
{
    LSession::handle()->sessionSettings()->sync(); // make sure it is up to date
    QString term = LXDG::findDefaultAppForMime("application/terminal"); // LSession::handle()->sessionSettings()->value("default-terminal","xterm").toString();
    if (term.isEmpty() ||(!term.endsWith(".desktop") && !LUtils::isValidBinary(term)) ) { term = "xterm"; }
    LSession::LaunchApplication(QString("%1 \"%2\"").arg(Draco::launcherApp()).arg(term));
}

void LDesktop::SystemFileManager()
{
    QProcess::startDetached(QString("qtfm %1").arg(QDir::homePath()));
}

void LDesktop::SystemApplication(QAction* act)
{
    if (!act->whatsThis().isEmpty() && act->parent()==deskMenu) {
        qDebug() << "system application" << act;
        LSession::LaunchApplication(QString("%1 \"%2\"")
                                    .arg(Draco::launcherApp())
                                    .arg(act->whatsThis()));
    }
}

void LDesktop::checkResolution()
{
    // Compare the current screen resolution with the last one used/saved and adjust config values *only*
    // NOTE: This is only run the first time this desktop is created (before loading all the interface) - not on each desktop change
    int oldWidth = settings->value(QString("%1screen/lastWidth").arg(DPREFIX), -1).toInt();
    int oldHeight = settings->value(QString("%1screen/lastHeight").arg(DPREFIX), -1).toInt();
    QRect scrn = LSession::handle()->screenGeom(Screen());

    if (scrn.isNull()) { return; }
    issyncing = true;

    settings->setValue(QString("%1screen/lastWidth").arg(DPREFIX), scrn.width());
    settings->setValue(QString("%1screen/lastHeight").arg(DPREFIX), scrn.height());

    if (oldWidth<1 || oldHeight<1 || scrn.width()<1 || scrn.height()<1) {
        // nothing to do - something invalid
    } else if (scrn.width()==oldWidth && scrn.height()==oldHeight) {
        // nothing to do - same as before
    } else {
        // Calculate the scale factor between the old/new sizes in each dimension
        // and forward that on to all the interface elements
        double xscale = scrn.width()/((double) oldWidth);
        double yscale = scrn.height()/((double) oldHeight);

        qDebug() << "Screen Resolution Changed:" << screenID;
        qDebug() << " - Old:" << QString::number(oldWidth)+"x"+QString::number(oldHeight);
        qDebug() << " - New:" << QString::number(scrn.width())+"x"+QString::number(scrn.height());
        qDebug() << " - Scale Factors:" << xscale << yscale;

        // Update any panels in the config file
        for (int i=0; i<4; i++) {
            QString PPREFIX = "panel"+QString::number(Screen())+"."+QString::number(i)+"/";
            int ht = settings->value(QString("%1height").arg(PPREFIX), -1).toInt();
            if (ht<1) { continue; } // no panel height defined
            QString loc = settings->value(QString("%1location").arg(PPREFIX), "top").toString().toLower();
            if (loc=="top" || loc=="bottom") {
                settings->setValue(QString("%1height").arg(PPREFIX), (int) ht*yscale); // vertical dimension
            } else {
                settings->setValue(QString("%1height").arg(PPREFIX), (int) ht*xscale); // horizontal dimension
            }
        }

        // Update any desktop plugins
        QStringList plugs = settings->value(QString("%1pluginlist").arg(DPREFIX)).toStringList();
        QFileInfoList files = LSession::handle()->DesktopFiles();
        for (int i=0; i<files.length(); i++) {
            plugs << "applauncher::"+files[i].absoluteFilePath()+"---"+DPREFIX;
        }

        //QString pspath = QDir::homePath()+"/.lumina/desktop-plugins/%1.conf";
        QSettings *DP = LSession::handle()->DesktopPluginSettings();
        QStringList keys = DP->allKeys();
        for (int i=0; i<plugs.length(); i++) {
            QStringList filter = keys.filter(plugs[i]);
            for (int j=0; j<filter.length(); j++) {
                // Has existing settings - need to adjust it
                if (filter[j].endsWith("location/height")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*yscale) ); }
                if (filter[j].endsWith("location/width")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*xscale) ); }
                if (filter[j].endsWith("location/x")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*xscale) ); }
                if (filter[j].endsWith("location/y")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*yscale) ); }
                if (filter[j].endsWith("IconSize")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*yscale) ); }
                if (filter[j].endsWith("iconsize")) { DP->setValue( filter[j], qRound(DP->value(filter[j]).toInt()*yscale) ); }
            }
        }
        DP->sync(); // make sure it gets saved to disk right away
    }
    issyncing = false;
}

void LDesktop::InitDesktop()
{
    // This is called *once* during the main initialization routines
    checkResolution(); // Adjust the desktop config file first (if necessary)
    qDebug() << "Init Desktop:" << Screen();
    qDebug() << "Desktop #"<<Screen()<<" -> "<< LSession::desktop()->screenGeometry(Screen()) << LSession::handle()->screenGeom(Screen());

    deskMenu = new QMenu(Q_NULLPTR);
    connect(deskMenu,
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(SystemApplication(QAction*)) );

    winMenu = new QMenu(Q_NULLPTR);
    winMenu->setTitle(tr("Window List"));
    winMenu->setIcon(LXDG::findIcon("preferences-system-windows",""));
    connect(winMenu,
            SIGNAL(triggered(QAction*)),
            this,
            SLOT(winClicked(QAction*)));

    workspacelabel = new QLabel(Q_NULLPTR);
    workspacelabel->setAlignment(Qt::AlignCenter);
    workspacelabel->setContentsMargins(3, 3, 3, 3);

    wkspaceact = new QWidgetAction(Q_NULLPTR);
    wkspaceact->setDefaultWidget(workspacelabel);

    bgtimer = new QTimer(this);
    bgtimer->setSingleShot(true);
    connect(bgtimer,
            SIGNAL(timeout()),
            this,
            SLOT(UpdateBackground()));

    connect(QApplication::instance(),
            SIGNAL(DesktopConfigChanged()),
            this,
            SLOT(SettingsChanged()) );
    connect(QApplication::instance(),
            SIGNAL(DesktopFilesChanged()),
            this,
            SLOT(UpdateDesktop()) );
    connect(QApplication::instance(),
            SIGNAL(MediaFilesChanged()),
            this,
            SLOT(UpdateDesktop()) );
    connect(QApplication::instance(),
            SIGNAL(LocaleChanged()),
            this,
            SLOT(LocaleChanged()) );
    connect(QApplication::instance(),
            SIGNAL(WorkspaceChanged()),
            this,
            SLOT(UpdateBackground()) );

    qDebug() << "Create bgDesktop";
    bgDesktop = new LDesktopPluginSpace();
    int grid = settings->value(QString("%1GridSize").arg(DPREFIX), -1).toInt();
    if (grid<0 && LSession::desktop()->screenGeometry(Screen()).height() > 2000) { grid = 200; }
    else if (grid<0) { grid = 100; }

    bgDesktop->SetIconSize( grid );
    bgDesktop->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(bgDesktop,
            SIGNAL(PluginRemovedByUser(QString)),
            this,
            SLOT(RemoveDeskPlugin(QString)));
    connect(bgDesktop,
            SIGNAL(IncreaseIcons()),
            this,
            SLOT(IncreaseDesktopPluginIcons()));
    connect(bgDesktop,
            SIGNAL(DecreaseIcons()),
            this,
            SLOT(DecreaseDesktopPluginIcons()));
    connect(bgDesktop,
            SIGNAL(HideDesktopMenu()),
            deskMenu,
            SLOT(hide()));
    connect(bgDesktop,
            SIGNAL(customContextMenuRequested(const QPoint&)),
            this,
            SLOT(ShowMenu()));

    desktopFolderActionMenu = new QMenu(Q_NULLPTR);
    desktopFolderActionMenu->setTitle(tr("Desktop"));
    desktopFolderActionMenu->setIcon(LXDG::findIcon("user-desktop", ""));
    desktopFolderActionMenu->addAction(LXDG::findIcon("folder-new", ""),
                                       tr("New Folder"),
                                       this,
                                       SLOT(NewDesktopFolder()));
    desktopFolderActionMenu->addAction(LXDG::findIcon("document-new", ""),
                                       tr("New File"),
                                       this,
                                       SLOT(NewDesktopFile()));
    desktopFolderActionMenu->addAction(LXDG::findIcon("edit-paste", ""),
                                       tr("Paste"),
                                       this,
                                       SLOT(PasteInDesktop()));

    qDebug() << " - Desktop Init Done:" << screenID;

    // Start the update processes
    QTimer::singleShot(10,
                       this,
                       SLOT(UpdateMenu()));
    QTimer::singleShot(0,
                       this,
                       SLOT(UpdateBackground()));
    QTimer::singleShot(1,
                       this,
                       SLOT(UpdateDesktop()));
    QTimer::singleShot(2,
                       this,
                       SLOT(UpdatePanels()));
}

void LDesktop::SettingsChanged()
{
    if (issyncing){ return; } // don't refresh for internal modifications to the
    issyncing = true;

    qDebug() << "Found Settings Change:" << screenID;
    settings->sync(); // make sure to sync with external settings changes
    UpdateBackground();
    UpdateDesktop();
    UpdatePanels();
    UpdateMenu();
    issyncing = false;

    QTimer::singleShot(50,
                       this,
                       SLOT(UnlockSettings())); // give it a few moments to settle before performing another sync
}

void LDesktop::LocaleChanged()
{
    // Update any elements which require a re-translation
    UpdateMenu(false); // do the full menu refresh
}

void LDesktop::UpdateMenu(bool fast)
{
    qDebug() << " - Update Menu:" << screenID;

    // Put a label at the top
    int num = LSession::handle()->XCB->CurrentWorkspace(); //LX11::GetCurrentDesktop();
    qDebug() << "Found workspace number:" << num;
    workspacelabel->setText(QString("%1 Desktop %2").arg(DESKTOP_APP_NAME).arg(DESKTOP_APP_VERSION));

    if (fast && usewinmenu) { UpdateWinMenu(); }
    if (fast) { return; } // already done
    deskMenu->clear(); // clear it for refresh
    deskMenu->addAction(wkspaceact);
    deskMenu->addSeparator();

    // Now load the user's menu setup and fill the menu
    QStringList items = settings->value("menu/itemlist", QStringList() << "windowlist" ).toStringList();
    usewinmenu=false;
    for (int i=0; i<items.length(); i++) {
        if (items[i]=="lockdesktop") { deskMenu->addAction(LXDG::findIcon("system-lock-screen",""), tr("Lock Session"), this, SLOT(SystemLock()) ); }
        else if (items[i]=="line") { deskMenu->addSeparator(); }
        else if (items[i]=="windowlist") {
            deskMenu->addMenu( winMenu);
            usewinmenu=true;
        } else if (items[i].startsWith("app::::") && items[i].endsWith(".desktop")) {
            // Custom *.desktop application
            QString file = items[i].section("::::",1,1).simplified();
            XDGDesktop xdgf(file); // = LXDG::loadDesktopFile(file, ok);
            if (xdgf.type!=XDGDesktop::BAD) {
                deskMenu->addAction(LXDG::findIcon(xdgf.icon,""),
                                    xdgf.name)->setWhatsThis(file);
            } else {
                qDebug() << "Could not load application file:" << file;
            }
        }
    }

    // Now add the desktop folder options (if desktop is icon-enabled)
    if (settings->value(QString("%1generateDesktopIcons").arg(DPREFIX), false).toBool()) {
        //deskMenu->addSeparator();
        deskMenu->addMenu(desktopFolderActionMenu);
    }

    // Now add the system quit options
    deskMenu->addSeparator();
    deskMenu->addAction(LXDG::findIcon("system-log-out",""),
                        tr("Leave"),
                        this,
                        SLOT(SystemLogout()) );
}

void LDesktop::UpdateWinMenu()
{
    winMenu->clear();
    // Get the current list of windows
    QList<WId> wins = LSession::handle()->XCB->WindowList();
    // Now add them to the menu
    for (int i=0; i<wins.length(); i++) {
        LWinInfo info(wins[i]);
        bool junk;
        QAction *act = winMenu->addAction(info.icon(junk), info.text());
        act->setData(QString::number(wins[i]));
    }
}

void LDesktop::winClicked(QAction* act)
{
    LSession::handle()->XCB->ActivateWindow(act->data().toString().toULong());
}

void LDesktop::UpdateDesktop()
{
    qDebug() << " - Update Desktop Plugins for screen:" << screenID;

    QStringList plugins = settings->value(DPREFIX+"pluginlist", QStringList()).toStringList();
    if (defaultdesktop && plugins.isEmpty()) {
        //plugins << "sample" << "sample" << "sample";
    }
    bool changed=false; //in case the plugin list needs to be changed

    // First make sure all the plugin names are unique
    for (int i=0; i<plugins.length(); i++) {
        if (!plugins[i].contains("---") ) {
            int num=1;
            while( plugins.contains(plugins[i]+"---"+QString::number(Screen())+"."+QString::number(num)) ) {
                num++;
            }
            plugins[i] = plugins[i]+"---"+screenID+"."+QString::number(num);
            //plugins[i] = plugins[i]+"---"+QString::number(Screen())+"."+QString::number(num);
            changed=true;
        }
    }
    if (changed) {
        // save the modified plugin list to file (so per-plugin settings are preserved)
        issyncing=true; //don't let the change cause a refresh
        settings->setValue(DPREFIX+"pluginlist", plugins);
        settings->sync();
        QTimer::singleShot(200, this, SLOT(UnlockSettings()) );
    }
    // If generating desktop file launchers, add those
    QStringList filelist;
    if (settings->value(DPREFIX+"generateDesktopIcons",false).toBool()) {
        QFileInfoList files = LSession::handle()->DesktopFiles();
        for (int i=0; i<files.length(); i++) {
            filelist << files[i].absoluteFilePath();
        }
    }

    // Also show anything available in the /run/media/USERNAME directory
    if (settings->value(DPREFIX+"generateMediaIcons",true).toBool()) {
        QDir userMedia(QString("/run/media/%1").arg(QDir::homePath().split("/").takeLast()));
        QStringList userMediadirs = userMedia.entryList(QDir::Dirs | QDir::NoDotAndDotDot, QDir::Name);
        for (int i=0; i<userMediadirs.length(); i++) {
            filelist << userMedia.absoluteFilePath(userMediadirs[i]);
        }
    }

    UpdateDesktopPluginArea();
    bgDesktop->LoadItems(plugins, filelist);
}

void LDesktop::RemoveDeskPlugin(QString ID)
{
    // This is called after a plugin is manually removed by the user
    // just need to ensure that the plugin is also removed from the settings file
    QStringList plugs = settings->value(DPREFIX+"pluginlist", QStringList()).toStringList();
    if (plugs.contains(ID)) {
        plugs.removeAll(ID);
        issyncing=true; //don't let the change cause a refresh
        settings->setValue(DPREFIX+"pluginlist", plugs);
        settings->sync();
        QTimer::singleShot(200, this, SLOT(UnlockSettings()));
    }
}

void LDesktop::IncreaseDesktopPluginIcons()
{
    int cur = settings->value(DPREFIX+"GridSize",-1).toInt();
    if (cur<0 &&LSession::desktop()->screenGeometry(Screen()).height() > 2000) { cur = 200; }
    else if (cur<0) { cur = 100; }
    cur+=16;
    issyncing=true; //don't let the change cause a refresh
    settings->setValue(DPREFIX+"GridSize", cur);
    settings->sync();
    QTimer::singleShot(200, this, SLOT(UnlockSettings()));
    bgDesktop->SetIconSize(cur);
}

void LDesktop::DecreaseDesktopPluginIcons()
{
    int cur = settings->value(DPREFIX+"GridSize",-1).toInt();
    if (cur<0 &&LSession::desktop()->screenGeometry(Screen()).height() > 2000) { cur = 200; }
    else if (cur<0) { cur = 100; }
    if (cur<32) { return; } //cannot get smaller than 16x16
    cur-=16;
    issyncing=true; //don't let the change cause a refresh
    settings->setValue(DPREFIX+"GridSize", cur);
    settings->sync();
    QTimer::singleShot(200, this, SLOT(UnlockSettings()));
    bgDesktop->SetIconSize(cur);
}

void LDesktop::UpdatePanels()
{
    qDebug() << " - Update Panels For Screen:" << Screen();
    int panels = settings->value(DPREFIX+"panels", -1).toInt();
    qDebug() << "PANELS?" << panels;
    if (panels==-1 && defaultdesktop) { panels=1; } //need at least 1 panel on the primary desktop
    // Remove all extra panels
    for (int i=0; i<PANELS.length(); i++) {
        if (panels <= PANELS[i]->number()) {
            qDebug() << " -- Remove Panel:" << PANELS[i]->number();
            PANELS[i]->prepareToClose();
            PANELS.takeAt(i)->deleteLater();
            i--;
        }
    }
    for (int i=0; i<panels; i++) {
        // Check for a panel with this number
        bool found = false;
        for (int p=0; p<PANELS.length() && !found; p++) {
            if (PANELS[p]->number() == i) {
                found = true;
                qDebug() << " -- Update panel "<< i;
                //panel already exists - just update it
                QTimer::singleShot(0, PANELS[p], SLOT(UpdatePanel()));
            }
        }
        if (!found) {
            qDebug() << " -- Create panel "<< i;
            // New panel
            LPanel *pan = new LPanel(settings, screenID, i, bgDesktop);
            PANELS << pan;
            pan->show();
        }
    }

    // Give it a 1/2 second before ensuring that the visible desktop area is correct
    QTimer::singleShot(1500, this, SLOT(UpdateDesktopPluginArea()));
}

void LDesktop::UpdateDesktopPluginArea()
{
    QRegion visReg(LSession::desktop()->screenGeometry(Screen())); // visible region (not hidden behind a panel)
    QRect rawRect = visReg.boundingRect(); // initial value (screen size)

    for (int i=0; i<PANELS.length(); i++) {
        QRegion shifted = visReg;
        QString loc = settings->value(PANELS[i]->prefix()+"location", "top").toString().toLower();
        int vis = PANELS[i]->visibleWidth();
        if (loc=="top") {
            if (!shifted.contains(QRect(rawRect.x(), rawRect.y(), rawRect.width(), vis))) { continue; }
            shifted.translate(0, (rawRect.top()+vis)-shifted.boundingRect().top());
        } else if (loc=="bottom") {
            if (!shifted.contains(QRect(rawRect.x(), rawRect.bottom()-vis, rawRect.width(), vis))) { continue; }
            shifted.translate(0, (rawRect.bottom()-vis)-shifted.boundingRect().bottom());
        } else if (loc=="left") {
            if (!shifted.contains(QRect(rawRect.x(), rawRect.y(), vis,rawRect.height()))) { continue; }
            shifted.translate((rawRect.left()+vis)-shifted.boundingRect().left() ,0);
        } else {  // right
            if (!shifted.contains(QRect(rawRect.right()-vis, rawRect.y(), vis,rawRect.height()))) { continue; }
            shifted.translate((rawRect.right()-vis)-shifted.boundingRect().right(),0);
        }
        visReg = visReg.intersected( shifted );
    }
    // Now make sure the desktop plugin area is only the visible area
    QRect rec = visReg.boundingRect();

    //Now remove the X offset to place it on the current screen (needs widget-coords, not global)
    globalWorkRect = rec; //save this for later
    rec.moveTopLeft(QPoint( rec.x()-LSession::desktop()->screenGeometry(Screen()).x() , rec.y()-LSession::desktop()->screenGeometry(Screen()).y()));

    if (rec.size().isNull()) { return; } // nothing changed

    bgDesktop->setGeometry(LSession::desktop()->screenGeometry(Screen()));
    bgDesktop->setDesktopArea(rec);
    bgDesktop->UpdateGeom(); //just in case the plugin space itself needs to do anything

    QTimer::singleShot(10, this, SLOT(UpdateBackground()));

    //Make sure to re-disable any WM control flags and reset geometry again
    LSession::handle()->XCB->SetDisableWMActions(bgDesktop->winId());
}

void LDesktop::UpdateBackground()
{
    // Get the current Background
    if (bgupdating || bgDesktop==Q_NULLPTR) { return; } //prevent multiple calls to this at the same time
    bgupdating = true;

    qDebug() << " - Update Desktop Background for screen:" << Screen();
    // Get the list of background(s) to show
    QStringList bgL = settings->value(DPREFIX+"background/filelist-workspace-"+QString::number( LSession::handle()->XCB->CurrentWorkspace()), QStringList()).toStringList();
    if (bgL.isEmpty()) { bgL = settings->value(DPREFIX+"background/filelist", QStringList()).toStringList(); }
    qDebug() << "BG?" << bgL;

    // Remove any invalid files
    for (int i=0; i<bgL.length(); i++) {
        if (bgL[i]=="default" || bgL[i].startsWith("rgb(") ) { continue; } // built-in definitions - treat them as valid
        if (bgL[i].isEmpty()) { bgL.removeAt(i); i--; }
        if (!QFile::exists(bgL[i])) {
            bgL.removeAt(i); i--;
        }
    }

    if (bgL.isEmpty()) { bgL << "default"; } // always fall back on the default
    // Determine if the background needs to be changed
    if (bgL==oldBGL && !CBG.isEmpty() && bgtimer->isActive()) {
        // No background change scheduled - just update the widget
        bgDesktop->update();
        bgupdating=false;
        return;
    }
    oldBGL = bgL; // save this for later

    // Determine which background to use next
    QString bgFile;
    while (bgFile.isEmpty() || QFileInfo(bgFile).isDir()) {
        QString prefix;
        if (!bgFile.isEmpty()) {
            // Got a directory - update the list of files and re-randomize the selection
            QStringList imgs = LUtils::imageExtensions();
            for (int i=0; i<imgs.length(); i++) { imgs[i].prepend("*."); }
            QDir tdir(bgFile);
            prefix=bgFile+"/";
            bgL = tdir.entryList(imgs, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);
            // If directory no longer has any valid images - remove it from list and try again
            if (bgL.isEmpty()) {
                oldBGL.removeAll(bgFile); // invalid directory - remove it from the list for the moment
                bgL = oldBGL; // reset the list back to the original list (not within a directory)
            }
        }
        // Verify that there are files in the list - otherwise use the default
        if (bgL.isEmpty()) { bgFile="default"; break; }
        int index = ( qrand() % bgL.length() );
        if (index== bgL.indexOf(CBG)) { // if the current wallpaper was selected by the randomization again
            // Go to the next in the list
            if (index < 0 || index >= bgL.length()-1) { index = 0; } // if invalid or last item in the list - go to first
            else { index++; } //go to next
        }
        bgFile = prefix+bgL[index];
    }

    //Save this file as the current background
    CBG = bgFile;

    // Now set this file as the current background
    QString format = settings->value(DPREFIX+"background/format", "stretch").toString();
    QPixmap backPix = LDesktopBackground::setBackground(bgFile, format, LSession::handle()->screenGeom(Screen()));
    bgDesktop->setBackground(backPix);
    // Now reset the timer for the next change (if appropriate)
    if (bgtimer->isActive()) { bgtimer->stop(); }
    if (bgL.length()>1 || oldBGL.length()>1) {
        // get the length of the timer (in minutes)
        int min = settings->value(DPREFIX+"background/minutesToChange", 5).toInt();
        // restart the internal timer
        if (min > 0){
            bgtimer->start(min*60000); // convert from minutes to milliseconds
        }
    }

    // Now update the panel backgrounds
    for (int i=0; i<PANELS.length(); i++) {
        PANELS[i]->update();
        PANELS[i]->show();
    }
    bgupdating=false;
}

// Desktop Folder Interactions
void LDesktop::i_dlg_finished(int ret)
{
    if(inputDLG==Q_NULLPTR) { return; }
    QString name = inputDLG->textValue();
    inputDLG->deleteLater();
    inputDLG = Q_NULLPTR;
    if (name.isEmpty() || ret!=QDialog::Accepted) { return; } // do nothing
    if (i_dlg_folder) { NewDesktopFolder(name); }
    else { NewDesktopFile(name); }
}

void LDesktop::NewDesktopFolder(QString name)
{
    if (name.isEmpty()){
        i_dlg_folder = true; // creating a new folder
        if (inputDLG == Q_NULLPTR) {
            inputDLG = new QInputDialog(Q_NULLPTR, Qt::Dialog | Qt::WindowStaysOnTopHint);
            inputDLG->setInputMode(QInputDialog::TextInput);
            inputDLG->setTextValue("");
            inputDLG->setTextEchoMode(QLineEdit::Normal);
            inputDLG->setLabelText(tr("New Folder"));
            connect(inputDLG, SIGNAL(finished(int)), this, SLOT(i_dlg_finished(int)));
        }
        inputDLG->showNormal();
    } else {
        QDir desktop(QDir::homePath());
        if (desktop.exists(tr("Desktop"))) { desktop.cd(tr("Desktop")); } // translated folder
        else { desktop.cd("Desktop"); } // default/english folder
        if (!desktop.exists(name)) { desktop.mkpath(name); }
    }
}

void LDesktop::NewDesktopFile(QString name)
{
    if (name.isEmpty()) {
        i_dlg_folder = false; // creating a new file
        if (inputDLG == Q_NULLPTR) {
            inputDLG = new QInputDialog(Q_NULLPTR, Qt::Dialog | Qt::WindowStaysOnTopHint);
            inputDLG->setInputMode(QInputDialog::TextInput);
            inputDLG->setTextValue("");
            inputDLG->setTextEchoMode(QLineEdit::Normal);
            inputDLG->setLabelText(tr("New File"));
            connect(inputDLG, SIGNAL(finished(int)), this, SLOT(i_dlg_finished(int)));
        }
        inputDLG->showNormal();
    } else {
        QDir desktop(QDir::homePath());
        if (desktop.exists(tr("Desktop"))) { desktop.cd(tr("Desktop")); } // translated folder
        else { desktop.cd("Desktop"); } // default/english folder
        if (!desktop.exists(name)) {
            QFile file(desktop.absoluteFilePath(name));
            if (file.open(QIODevice::WriteOnly)) { file.close(); }
        }
    }
}

void LDesktop::PasteInDesktop()
{
    const QMimeData *mime = QApplication::clipboard()->mimeData();
    QStringList files;
    if (mime->hasFormat("x-special/lumina-copied-files")) {
        files = QString(mime->data("x-special/lumina-copied-files")).split("\n");
    } else if (mime->hasUrls()) {
        QList<QUrl> urls = mime->urls();
        for (int i=0; i<urls.length(); i++) {
            files << QString("copy::::")+urls[i].toLocalFile();
        }
    }
    qDebug() << "PASTE IN DESKTOP" << files;

    // Now go through and paste all the designated files
    QString desktop = LUtils::standardDirectory(LUtils::Desktop);
    QStringList copyFileList,copyDirList,moveFileList,moveDirList;
    for (int i=0; i<files.length(); i++) {
        QString path = files[i].section("::::",1,-1);
        if (!QFile::exists(path)) { continue; } // does not exist any more - move on to next
        bool isDir = QFileInfo(path).isDir();
        if (files[i].section("::::",0,0)=="cut") { // move
            if (isDir) { moveDirList << path; }
            else { moveFileList << path; }
        } else { // copy
            if (isDir) { copyDirList << path; }
            else { copyFileList << path; }
        }

        if (copyFileList.size()>0 ||
        copyDirList.size()>0 ||
        moveFileList.size()>0 ||
        moveDirList.size()>0)
        {
            // Copy/move files/folders to Desktop
            QtFileCopier *copyHander = new QtFileCopier(this);
            QtCopyDialog *copyDialog = new QtCopyDialog(copyHander);
            copyDialog->setMinimumDuration(100);
            copyDialog->setAutoClose(true);

            if (copyFileList.size()>0) { copyHander->copyFiles(copyFileList, desktop); }
            if (moveFileList.size()>0) { copyHander->moveFiles(moveFileList, desktop); }
            for (int i=0;i<copyDirList.size();++i) {
                copyHander->copyDirectory(copyDirList.at(i), desktop);
            }
            for (int i=0;i<moveDirList.size();++i) {
                copyHander->moveDirectory(moveDirList.at(i), desktop);
            }
        }
    }
}
