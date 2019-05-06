#include <QApplication>
#include "org.dracolinux.Power.KeyboardDialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    KeyboardDialog dialog;
    dialog.show();
    return a.exec();
}
