#include "loginwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DataBase db;
    LoginWindow login_window(&db);
    login_window.show();
    return a.exec();
}
