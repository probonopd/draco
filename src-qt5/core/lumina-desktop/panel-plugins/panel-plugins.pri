#Lumina Desktop Panel Plugin files

SOURCES += $$PWD/userbutton/LUserButton.cpp \
	$$PWD/userbutton/UserWidget.cpp \
	$$PWD/userbutton/UserItemWidget.cpp \
	$$PWD/desktopbar/LDeskBar.cpp \
	$$PWD/taskmanager/LTaskManagerPlugin.cpp \
	$$PWD/taskmanager/LTaskButton.cpp \
	$$PWD/systemtray/LSysTray.cpp \
	$$PWD/systemtray/TrayIcon.cpp \
	$$PWD/clock/LClock.cpp \
	$$PWD/desktopswitcher/LDesktopSwitcher.cpp \
	$$PWD/systemdashboard/LSysDashboard.cpp \
	$$PWD/showdesktop/LHomeButton.cpp \
	$$PWD/appmenu/LAppMenuPlugin.cpp \
	$$PWD/applauncher/AppLaunchButton.cpp \
	$$PWD/systemstart/ItemWidget.cpp \
	$$PWD/jsonmenu/PPJsonMenu.cpp
	
HEADERS += $$PWD/RotateToolButton.h \
	$$PWD/userbutton/LUserButton.h \
	$$PWD/userbutton/UserWidget.h \
	$$PWD/userbutton/UserItemWidget.h \
	$$PWD/desktopbar/LDeskBar.h \
	$$PWD/systemtray/LSysTray.h \
	$$PWD/systemtray/TrayIcon.h \
	$$PWD/spacer/LSpacer.h \
	$$PWD/line/LLine.h \
	$$PWD/clock/LClock.h \
	$$PWD/desktopswitcher/LDesktopSwitcher.h \
	$$PWD/taskmanager/LTaskManagerPlugin.h \
	$$PWD/taskmanager/LTaskButton.h \
	$$PWD/systemdashboard/LSysDashboard.h \
	$$PWD/showdesktop/LHomeButton.h \
	$$PWD/appmenu/LAppMenuPlugin.h \
	$$PWD/applauncher/AppLaunchButton.h \
	$$PWD/systemstart/ItemWidget.h \
	$$PWD/jsonmenu/PPJsonMenu.h

FORMS +=	 $$PWD/userbutton/UserWidget.ui
	
