//===========================================
//  Lumina-DE source code
//  Copyright (c) 2014-2015, Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "UserItemWidget.h"
#include <LuminaUtils.h>

#define TEXTCUTOFF 165
UserItemWidget::UserItemWidget(QWidget *parent, QString itemPath, QString type, bool goback) : QFrame(parent){
  createWidget();
  //Now fill it appropriately
  bool inHome = type.endsWith("-home"); //internal code
  if(inHome){ type = type.remove("-home"); }
  if(itemPath.endsWith(".desktop") || type=="app"){
    bool ok = false;
    XDGDesktop item = LXDG::loadDesktopFile(itemPath, ok);
    if(ok){
      icon->setPixmap( LXDG::findIcon(item.icon, "preferences-system-windows-actions").pixmap(32,32) );
      name->setText( this->fontMetrics().elidedText(item.name, Qt::ElideRight, TEXTCUTOFF) );
    }else{
      icon->setPixmap( LXDG::findIcon("unknown","").pixmap(32,32) );
      name->setText( this->fontMetrics().elidedText(itemPath.section("/",-1), Qt::ElideRight, TEXTCUTOFF) );
    }
  }else if(type=="dir"){
    if(itemPath.endsWith("/")){ itemPath.chop(1); }
    if(goback){
      icon->setPixmap( LXDG::findIcon("go-previous","").pixmap(32,32) );
      name->setText( tr("Go Back") );
    }else{
      icon->setPixmap( LXDG::findIcon("folder","").pixmap(32,32) );
      name->setText( this->fontMetrics().elidedText(itemPath.section("/",-1), Qt::ElideRight, TEXTCUTOFF) ); 
    }
  }else{
    if(itemPath.endsWith("/")){ itemPath.chop(1); }
    if(QFileInfo(itemPath).isDir()){
      type = "dir";
      icon->setPixmap( LXDG::findIcon("folder","").pixmap(32,32) );
    }else if(LUtils::imageExtensions().contains(itemPath.section("/",-1).section(".",-1).toLower()) ){
      icon->setPixmap( QIcon(itemPath).pixmap(32,32) );
    }else{
      icon->setPixmap( LXDG::findMimeIcon(type).pixmap(32,32) );
    }
    name->setText( this->fontMetrics().elidedText(itemPath.section("/",-1), Qt::ElideRight, TEXTCUTOFF) ); 
  }
  icon->setWhatsThis(itemPath);
  if(!goback){ this->setWhatsThis(name->text()); }
  isDirectory = (type=="dir"); //save this for later
  if(LUtils::isFavorite(itemPath)){
    linkPath = itemPath;
    isShortcut=true;
  }else if( inHome ){//|| itemPath.section("/",0,-2)==QDir::homePath()+"/Desktop" ){
    isShortcut = true;
  }else{
    isShortcut = false;
  }
  //Now setup the button appropriately
  setupButton(goback);
}

UserItemWidget::UserItemWidget(QWidget *parent, XDGDesktop item) : QFrame(parent){
  createWidget();
  isDirectory = false;
  if(LUtils::isFavorite(item.filePath)){
    linkPath = item.filePath;
    isShortcut=true;
  }else if( item.filePath.section("/",0,-2)==QDir::homePath()+"/Desktop" ){
    isShortcut = true;
  }else{
    isShortcut = false;
  }
  //Now fill it appropriately
  icon->setPixmap( LXDG::findIcon(item.icon,"preferences-system-windows-actions").pixmap(32,32) );
  name->setText( this->fontMetrics().elidedText(item.name, Qt::ElideRight, TEXTCUTOFF) ); 
  this->setWhatsThis(name->text());
  icon->setWhatsThis(item.filePath);
  //Now setup the button appropriately
  setupButton();

}

UserItemWidget::~UserItemWidget(){ 
  delete button;
  delete icon;
  delete name;
}


void UserItemWidget::createWidget(){
  //Initialize the widgets
  this->setContentsMargins(0,0,0,0);
  button = new QToolButton(this);
    button->setIconSize( QSize(14,14) );
    button->setAutoRaise(true);
  icon = new QLabel(this);
    icon->setFixedSize( QSize(34,34) );
  name = new QLabel(this);
  //Add them to the layout
  this->setLayout(new QHBoxLayout());
    this->layout()->setContentsMargins(1,1,1,1);
    this->layout()->addWidget(icon);
    this->layout()->addWidget(name);
    this->layout()->addWidget(button);
  //Set a custom object name so this can be tied into the Lumina Theme stylesheets
  this->setObjectName("LuminaUserItemWidget");
  //Install the stylesheet
  //this->setStyleSheet("UserItemWidget{ background: transparent; border-radius: 5px;} UserItemWidget::hover{ background: rgba(255,255,255,200); border-radius: 5px; }");
}

void UserItemWidget::setupButton(bool disable){
  //if(isDirectory){ qDebug() << "Directory Entry:" << isShortcut << linkPath << icon->whatsThis(); }
	
  if(disable){
    button->setVisible(false);
  }else if(isShortcut && !linkPath.isEmpty()){ //Favorite Item - can always remove this
      button->setWhatsThis("remove");
      button->setIcon( LXDG::findIcon("list-remove","") );
      button->setToolTip(tr("Remove Shortcut"));
      connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
  }else if(isShortcut){ //Physical File/Dir - can remove
      button->setWhatsThis("remove");
      button->setIcon( LXDG::findIcon("user-trash","") );
      button->setToolTip(tr("Delete File"));
      connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
  }else if(!isShortcut){// if( !QFile::exists( QDir::homePath()+"/Desktop/"+icon->whatsThis().section("/",-1) ) && !LUtils::isFavorite(icon->whatsThis() ) ){
    //This file does not have a shortcut yet -- allow the user to add it
    button->setWhatsThis("add");
    button->setIcon( LXDG::findIcon("bookmark-toolbar","") );    
    button->setToolTip(tr("Create Shortcut"));
    connect(button, SIGNAL(clicked()), this, SLOT(buttonClicked()) );
  }else{
    //This already has a desktop shortcut -- no special actions
    button->setVisible(false);
  }
  if(isShortcut){
    name->setToolTip(icon->whatsThis()); //also allow the user to see the full shortcut path
  }
}

void UserItemWidget::buttonClicked(){
  button->setVisible(false);
  if(button->whatsThis()=="add"){ 
    LUtils::addFavorite(icon->whatsThis());
    //QFile::link(icon->whatsThis(), QDir::homePath()+"/.lumina/favorites/"+icon->whatsThis().section("/",-1) );
    emit NewShortcut(); 
  }else if(button->whatsThis()=="remove"){ 
    if(linkPath.isEmpty()){ 
      //This is a desktop file
      if(isDirectory){
	QProcess::startDetached("rm -r \""+icon->whatsThis()+"\"");
      }else{
	QFile::remove(icon->whatsThis()); 
      } 
    }else{ LUtils::removeFavorite(icon->whatsThis()); } //This is a favorite
    emit RemovedShortcut(); 
  }
}

void UserItemWidget::ItemClicked(){
  if(!linkPath.isEmpty()){ emit RunItem(linkPath); }
  else{ emit RunItem(icon->whatsThis()); }
}