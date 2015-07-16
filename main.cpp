#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setOrganizationName("Aloshi");
    QCoreApplication::setOrganizationDomain("aloshi.com");
    QCoreApplication::setApplicationName("Bugel");
    MainWindow w;
    w.show();

    return a.exec();
}
