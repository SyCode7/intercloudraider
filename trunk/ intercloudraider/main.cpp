#include <QtGui/QApplication>
#include "DBContr/databasecontrollersetting.h"
#include "GUI/mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    DatabaseControllerSetting dbcontr;
    if(!dbcontr.checkParameterKeySecret())
    {
        QStringList key_secret;
        key_secret = dbcontr.getKeySecret();
        dbcontr.createParameterKeySecret(key_secret[0], key_secret[1],
                                         key_secret[2], key_secret[3],
                                         key_secret[4], key_secret[5]);
    }

    MainWindow window;
    window.show();
    
    return app.exec();
}
