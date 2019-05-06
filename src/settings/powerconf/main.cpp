#include <QApplication>
#include "org.dracolinux.Power.SettingsDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Dialog dialog;
    dialog.show();
    return a.exec();
}
