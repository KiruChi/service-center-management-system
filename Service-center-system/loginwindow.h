#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QWidget>
#include "DataBase.h"
#include "mainwindow.h"
#include <QMessageBox>
#include "DataBase.h"

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit LoginWindow(DataBase* dbСonnect,QWidget *parent = nullptr);
    ~LoginWindow();

private slots:
    void on_LoginButton_clicked();

private:
    Ui::LoginWindow *ui;
    DataBase* db;
};

#endif // LOGINWINDOW_H
