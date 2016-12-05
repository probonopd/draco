//===========================================
//  Lumina-DE source code
//  Copyright (c) 2016, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include <QDebug>
#include <QCoreApplication>
#include <QProcess>
#include <QString>

#include "session.h"
#include <LUtils.h>
#include <LDesktopUtils.h>
#include <LuminaOS.h>
#include <LuminaThemes.h>
#include <LuminaXDG.h>

#define DEBUG 0

int main(int argc, char ** argv)
{
    if (argc > 1) {
      if (QString(argv[1]) == QString("--version")){
        qDebug() << LDesktopUtils::LuminaDesktopVersion();
        return 0;
      }
    }
    if(!QFile::exists(LOS::LuminaShare())){
      qDebug() << "Lumina does not appear to be installed correctly. Cannot find: " << LOS::LuminaShare();
      return 1;
    }
    //Start X11 if needed
    QString disp = QString(getenv("DISPLAY")).simplified();
    if(disp.isEmpty()){
      qDebug() << "No X11 session detected: Lumina will try to start one...";
      //No X session found. Go ahead and re-init this binary within an xinit call
      QString prog = QString(argv[0]).section("/",-1);
      LUtils::isValidBinary(prog); //will adjust the path to be absolute
      QStringList args; args << prog;
      //if(LUtils::isValidBinary("x11vnc")){ args << "--" << "-listen" << "tcp"; } //need to be able to VNC into this session
      return QProcess::execute("xinit", args);
    }
    qDebug() << "Starting the Lumina desktop on current X11 session:" << disp;
    //Setup any initialization values
    LTHEME::LoadCustomEnvSettings();
    LXDG::setEnvironmentVars();
    setenv("DESKTOP_SESSION","Lumina",1);
    setenv("XDG_CURRENT_DESKTOP","Lumina",1);
    unsetenv("QT_QPA_PLATFORMTHEME"); //causes issues with Lumina themes - not many people have this by default...
    //Check for any missing user config files
    


    //Configure X11 monitors if needed
    if(LUtils::isValidBinary("lumina-xconfig")){ 
      qDebug() << " - Resetting monitor configuration to last-used settings";
      QProcess::execute("lumina-xconfig --reset-monitors");
    }
    qDebug() << " - Starting the session...";
    //Startup the session
    QCoreApplication a(argc, argv);
    LSession sess;
      sess.start();
    int retCode = a.exec();
    qDebug() << "Finished Closing Down Lumina";
    return retCode;
}
