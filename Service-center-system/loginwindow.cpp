#include "loginwindow.h"
#include "ui_loginwindow.h"

LoginWindow::LoginWindow(DataBase* dbConnect, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWindow)
    , db(dbConnect)
{
    ui->setupUi(this);
}

LoginWindow::~LoginWindow()
{
    delete ui;
}

// Обробник натискання кнопки "Login"
void LoginWindow::on_LoginButton_clicked()
{
    QString name = ui->UserNameLineEdit->text();
    QString password = ui->PasswordLineEdit->text();
    try
    {
        if(db->loginUser(name, password)) {
            auto mainWin = new MainWindow(db);
            mainWin->show();
            this->close();
        } else {
            ui->ErrorLabel->setText("Invalid login or password");
        }
    } catch(...)
    {
        ui->ErrorLabel->setText("Cannot connect to database");
    }
}

