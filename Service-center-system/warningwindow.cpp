#include "warningwindow.h"
#include "ui_warningwindow.h"

WarningWindow::WarningWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::WarningWindow)
{
    ui->setupUi(this);
}

WarningWindow::~WarningWindow()
{
    delete ui;
}
