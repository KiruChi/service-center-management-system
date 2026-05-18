#ifndef ORDERWINDOW_H
#define ORDERWINDOW_H

#include <QDialog>
#include "DataBase.h"
#include <QMessageBox>

namespace Ui {
class OrderWindow;
}

class OrderWindow : public QDialog
{
    Q_OBJECT

public:
    explicit OrderWindow(DataBase* dbСonnect,QString type, QString orderId = "", QWidget *parent = nullptr);
    ~OrderWindow();

private slots:
    void SetFields(QString orderId);
    void SetRestrict();
    bool Check();
    void on_AcceptButton_clicked();
    void on_CancelButton_clicked();

private:
    QString type;
    QString previousStatus;
    Ui::OrderWindow *ui;
    DataBase* db;
};

#endif // ORDERWINDOW_H
