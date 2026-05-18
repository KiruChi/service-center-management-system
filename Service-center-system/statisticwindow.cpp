#include "statisticwindow.h"
#include "ui_statisticwindow.h"

StatisticWindow::StatisticWindow(DataBase* dbСonnect, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::StatisticWindow)
    , db(dbСonnect)
{
    ui->setupUi(this);
    ui->date1Edit->setDate(QDate::currentDate());
    ui->date2Edit->setDate(QDate::currentDate());
}

StatisticWindow::~StatisticWindow()
{
    delete ui;
}

// Обробка натискання кнопки "Show"
void StatisticWindow::on_showButton_clicked()
{
    auto date1 = ui->date1Edit->date().toString("yyyy-MM-dd").toStdString();
    auto date2 = ui->date2Edit->date().toString("yyyy-MM-dd").toStdString();

    if(ui->date1Edit->date() > ui->date2Edit->date())
    {
        QMessageBox::warning(this, "Error", "Your second date cannot be earlier than the first one");
        return;
    }

    // Total
    auto total = db->getTotalStatistic(date1, date2);
    ui->TotalOrdersLineEdit->setText(QString::number(total[0]));
    ui->TotalActiveLineEdit->setText(QString::number(total[1]));
    ui->TotalCompletedlineEdit->setText(QString::number(total[2]));
    ui->TotalMoneyEarnedLineEdit->setText(QString::number(total[3]) + "₴");

    // Orders by month chart
    auto ordersByMonth = db->getOrdersByMonth(date1, date2);
    showOrdersByMonth(ordersByMonth);

    // Technician statistics
    auto techStats = db->getTechniciansStats(date1, date2);
    showTechnicians(techStats);
    showBestTechnician(techStats);

    // Receptionist statistics
    auto recStats = db->getReceptionistsStats(date1, date2);
    showReceptionists(recStats);
    showBestReceptionist(recStats);
}

// Побудова стовпчикової діаграми "Заявки по місяцях"
void StatisticWindow::showOrdersByMonth(std::map<QString, int>& data)
{
    QBarSet* set = new QBarSet("Orders");
    QStringList categories;

    int maxValue = 0;
    for (auto& pair : data)
    {
        categories << pair.first;
        *set << pair.second;
        if (pair.second > maxValue)
            maxValue = pair.second;
    }
    if (maxValue == 0) maxValue = 1;

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Orders by month");
    chart->setAnimationOptions(QChart::SeriesAnimations);
    chart->legend()->setAlignment(Qt::AlignBottom);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, maxValue);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->ordersChartView->setChart(chart);
    ui->ordersChartView->setRenderHint(QPainter::Antialiasing);
}

// Побудова графіка продуктивності техніків
void StatisticWindow::showTechnicians(std::map<QString, int>& data)
{
    QBarSet* set = new QBarSet("Completed orders");
    QStringList categories;

    int maxValue = 0;

    for (auto& pair : data)
    {
        categories << pair.first;   // ім'я техніка
        *set << pair.second;        // кількість виконаних робіт
        if (pair.second > maxValue)
            maxValue = pair.second;
    }

    if (maxValue == 0) maxValue = 1;

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Technician performance");
    chart->legend()->setAlignment(Qt::AlignBottom);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, maxValue);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->technicianChartView->setChart(chart);
    ui->technicianChartView->setRenderHint(QPainter::Antialiasing);
}

// Визначення найкращого техніка
void StatisticWindow::showBestTechnician(std::map<QString, int>& data)
{
    QString best;
    int bestCount = -1;

    for (auto& pair : data)
    {
        if (pair.second > bestCount)
        {
            best = pair.first;
            bestCount = pair.second;
        }
    }

    if (bestCount <= 0)
        ui->BestTechnicianLineEdit->setText("No data");
    else
        ui->BestTechnicianLineEdit->setText(best + " (" + QString::number(bestCount) + ")");
}

// Побудова графіка продуктивності приймальників
void StatisticWindow::showReceptionists(std::map<QString, int>& data)
{
    QBarSet* set = new QBarSet("Accepted orders");
    QStringList categories;

    int maxValue = 0;

    for (auto& pair : data)
    {
        categories << pair.first;    // ім'я приймальника
        *set << pair.second;         // кількість оформлених заявок
        if (pair.second > maxValue)
            maxValue = pair.second;
    }
    if (maxValue == 0) maxValue = 1;

    QBarSeries* series = new QBarSeries();
    series->append(set);

    QChart* chart = new QChart();
    chart->addSeries(series);
    chart->setTitle("Receptionist performance");
    chart->legend()->setAlignment(Qt::AlignBottom);

    QBarCategoryAxis* axisX = new QBarCategoryAxis();
    axisX->append(categories);
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    QValueAxis* axisY = new QValueAxis();
    axisY->setRange(0, maxValue);
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    ui->receptionistChartView->setChart(chart);
    ui->receptionistChartView->setRenderHint(QPainter::Antialiasing);
}

// Визначення найкращого приймальника
void StatisticWindow::showBestReceptionist(std::map<QString, int>& data)
{
    QString best;
    int bestCount = -1;

    for (auto& pair : data)
    {
        if (pair.second > bestCount)
        {
            best = pair.first;
            bestCount = pair.second;
        }
    }
    if (bestCount <= 0)
        ui->BestReceptionistLineEdit->setText("No data");
    else
        ui->BestReceptionistLineEdit->setText(best + " (" + QString::number(bestCount) + ")");
}
