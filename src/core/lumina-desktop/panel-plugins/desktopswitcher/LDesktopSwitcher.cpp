//===========================================
//  Lumina-DE source code
//  Copyright (c) 2014, Susanne Jaeckel
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "LDesktopSwitcher.h"
#include <LSession.h>

LDesktopSwitcher::LDesktopSwitcher(QWidget *parent, QString id, bool horizontal) : LPPlugin(parent, id, horizontal) {
  iconOld = -1;
  this->setStyleSheet( "QToolButton::menu-indicator{ image: none; } QToolButton{padding: 0px;}");
  //Setup the widget
  label = new QToolButton(this);
  label->setPopupMode(QToolButton::DelayedPopup);
  label->setAutoRaise(true);
  label->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
  label->setIcon( LXDG::findIcon("format-view-carousel", "preferences-desktop-display") );
  label->setToolTip(QString(tr("Workspace 1")));
  connect(label, SIGNAL(clicked()), this, SLOT(openMenu()));
  menu = new QMenu(this);
  connect(menu, SIGNAL(triggered(QAction*)), this, SLOT(menuActionTriggered(QAction*)));
  connect(menu, SIGNAL(aboutToHide()), this, SIGNAL(MenuClosed()));
  label->setMenu(menu);
  this->layout()->addWidget(label);

  // Maybe a timer should be set to set the toolTip of the button,
  // becasue the workspace could be switched via Keyboard-shortcuts ...

  QTimer::singleShot(500, this, SLOT(createMenu()) ); //needs a delay to make sure it works right the first time
  QTimer::singleShot(0,this, SLOT(OrientationChange()) ); //adjust icon size

  //Process the signal which is sent when the workspace is changed via keyboard-shortcuts
  connect(QApplication::instance(), SIGNAL(WorkspaceChanged()), this, SLOT(updateWorkspaceMenu()));
}

LDesktopSwitcher::~LDesktopSwitcher(){
}
/*  MOVED THESE FUNCTIONS TO LIBLUMINA (LuminaX11.h) -- Ken Moore 5/9/14
void LDesktopSwitcher::setNumberOfDesktops(int number) {
  Display *display = QX11Info::display();
  Window rootWindow = QX11Info::appRootWindow();

  Atom atom = XInternAtom(display, "_NET_NUMBER_OF_DESKTOPS", False);
  XEvent xevent;
  xevent.type                 = ClientMessage;
  xevent.xclient.type         = ClientMessage;
  xevent.xclient.display      = display;
  xevent.xclient.window       = rootWindow;
  xevent.xclient.message_type = atom;
  xevent.xclient.format       = 32;
  xevent.xclient.data.l[0]    = number;
  xevent.xclient.data.l[1]    = CurrentTime;
  xevent.xclient.data.l[2]    = 0;
  xevent.xclient.data.l[3]    = 0;
  xevent.xclient.data.l[4]    = 0;
  XSendEvent(display, rootWindow, False, SubstructureNotifyMask | SubstructureRedirectMask, &xevent);

  XFlush(display);
}

void LDesktopSwitcher::setCurrentDesktop(int number) {
  Display *display = QX11Info::display();
  Window rootWindow = QX11Info::appRootWindow();

  Atom atom = XInternAtom(display, "_NET_CURRENT_DESKTOP", False);
  XEvent xevent;
  xevent.type                 = ClientMessage;
  xevent.xclient.type         = ClientMessage; 
  xevent.xclient.display      = display;
  xevent.xclient.window       = rootWindow;
  xevent.xclient.message_type = atom;
  xevent.xclient.format       = 32;
  xevent.xclient.data.l[0]    = number;
  xevent.xclient.data.l[1]    = CurrentTime;
  xevent.xclient.data.l[2]    = 0;
  xevent.xclient.data.l[3]    = 0;
  xevent.xclient.data.l[4]    = 0;
  XSendEvent(display, rootWindow, False, SubstructureNotifyMask | SubstructureRedirectMask, &xevent);

  XFlush(display);
}

int LDesktopSwitcher::getNumberOfDesktops() {
  int number = -1;
  Atom a = XInternAtom(QX11Info::display(), "_NET_NUMBER_OF_DESKTOPS", true);
  Atom realType;
  int format;
  unsigned long num, bytes;
  unsigned char *data = 0;
  int status = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), a, 0L, (~0L),
             false, AnyPropertyType, &realType, &format, &num, &bytes, &data);
  if( (status >= Success) && (num > 0) ){
    number = *data;
    XFree(data);
  }
  return number;
}

int LDesktopSwitcher::getCurrentDesktop() {
  int number = -1;
  Atom a = XInternAtom(QX11Info::display(), "_NET_CURRENT_DESKTOP", true);
  Atom realType;
  int format;
  unsigned long num, bytes;
  unsigned char *data = 0;
  int status = XGetWindowProperty(QX11Info::display(), QX11Info::appRootWindow(), a, 0L, (~0L),
             false, AnyPropertyType, &realType, &format, &num, &bytes, &data);
  if( (status >= Success) && (num > 0) ){
    number = *data;
    XFree(data);
  }
  return number;
} */

void LDesktopSwitcher::openMenu(){
  //Make sure the menu is refreshed right before it opens
  createMenu();
  label->showMenu();
}

QAction* LDesktopSwitcher::newAction(int what, QString name) {
  QAction *act = new QAction(LXDG::findIcon("preferences-desktop-display", ""), name, this);
  act->setWhatsThis(QString::number(what));
  return act;
}

void LDesktopSwitcher::createMenu() {
  int cur = LSession::handle()->XCB->CurrentWorkspace(); //current desktop number
  int tot = LSession::handle()->XCB->NumberOfWorkspaces(); //total number of desktops
  //qDebug() << "-- vor getCurrentDesktop SWITCH";
  //qDebug() << "Virtual Desktops:" << tot << cur;
  menu->clear();
  for (int i = 0; i < tot; i++) {
    QString name = QString(tr("Workspace %1")).arg( QString::number(i+1) );
    if(i == cur){ name.prepend("*"); name.append("*");} //identify which desktop this is currently
    menu->addAction(newAction(i, name));
  }
  label->setToolTip(QString(tr("Workspace %1")).arg(QString::number(cur + 1)));
  label->setText( QString::number(cur+1) );
}

void LDesktopSwitcher::menuActionTriggered(QAction* act) {
  LSession::handle()->XCB->SetCurrentWorkspace(act->whatsThis().toInt());
}

void LDesktopSwitcher::updateWorkspaceMenu() {
  createMenu();
}
