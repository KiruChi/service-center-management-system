#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "DataBase.h"
#include <QStandardItemModel>
#include "orderwindow.h"
#include "statisticwindow.h"
#include "loginwindow.h"
#include "informationwindow.h"
#include "DataBase.h"
#include <QMessageBox>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(DataBase* dbСonnect,QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void CreateTable();

    void UpdateTable(QString search = "");

    void on_AddButton_clicked();
    void on_EditButton_clicked();
    void on_DeleteButton_clicked();
    void on_SearchButton_clicked();
    void on_actionShow_statistic_triggered();
    void on_actionLog_out_triggered();
    void on_actionAdd_order_triggered();
    void on_actionDelete_order_triggered();
    void on_actionShow_info_triggered();
    void on_actionEdit_order_triggered();

private:
    Ui::MainWindow *ui;
    DataBase* db;
    QStandardItemModel *ItemModel;
};
#endif // MAINWINDOW_H
