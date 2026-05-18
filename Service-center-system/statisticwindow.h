#ifndef STATISTICWINDOW_H
#define STATISTICWINDOW_H

#include <QWidget>
#include "DataBase.h"
#include <QtCharts/QChartView>
#include <QtCharts/QChart>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QValueAxis>
#include <QMessageBox>

namespace Ui {
class StatisticWindow;
}

class StatisticWindow : public QWidget
{
    Q_OBJECT

public:
    explicit StatisticWindow(DataBase* dbСonnect, QWidget *parent = nullptr);
    ~StatisticWindow();

private slots:
    void on_showButton_clicked();
    void showOrdersByMonth(std::map<QString, int>& data);
    void showTechnicians(std::map<QString, int>& data);
    void showBestTechnician(std::map<QString, int>& data);
    void showReceptionists(std::map<QString, int>& data);
    void showBestReceptionist(std::map<QString, int>& data);

private:
    DataBase* db;
    Ui::StatisticWindow *ui;
};

#endif // STATISTICWINDOW_H
