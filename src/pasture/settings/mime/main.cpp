#include "settingsdialog.h"
#include "mimeutils.h"
#include "common.h"

#include <QApplication>
#include <QSettings>
#include <QString>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MimeUtils mimeUtils(Q_NULLPTR);
    QSettings settings(Common::configFile(), QSettings::IniFormat);
    QString name = settings.value("defMimeAppsFile", MIME_APPS).toString();
    mimeUtils.setDefaultsFileName(name);

    SettingsDialog d(&settings, &mimeUtils, Q_NULLPTR);
    d.show();
    return a.exec();
}
