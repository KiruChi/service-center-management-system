#include "mainwindow.h"
#include "./ui_mainwindow.h"


MainWindow::MainWindow(DataBase* dbСonnect,QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , db(dbСonnect)
{
    ui->setupUi(this);
    QMessageBox::information(this, "Success!", " You have logged in as: " + db->getUserRole()); // Повідомлення про успішну авторизацію та роль користувача
    // Обмеження доступу до кнопок відповідно до ролі
    if (db->getUserRole() == "Technician")
    {
        ui->AddButton->hide();
        ui->DeleteButton->hide();
    }
    if (db->getUserRole() == "Receptionist")
    {
        ui->DeleteButton->hide();
    }
    // Ініціалізація таблиці та завантаження даних
    CreateTable();
    UpdateTable();
}

// Створення та налаштування таблиці заявок
void MainWindow::CreateTable()
{
    ItemModel = new QStandardItemModel(this);
    ItemModel->setHorizontalHeaderLabels({"Order ID", "Status", "Customer Name", "Device Brand", "Device Model","Price Estimate",
                                          "Price Final","Date Received", "Date Started", "Date Completed","Received By", "Repair By"});
    ui->tableView->setModel(ItemModel);
    ui->tableView->setColumnHidden(0, true);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
}

// Оновлення таблиці заявок (усі заявки або пошук)
void MainWindow::UpdateTable(QString search)
{
    ItemModel->removeRows(0, ItemModel->rowCount());
    std::vector<bsoncxx::document::value> items{};
    if(search == ""){
        items = db->findAllOrders();
    }
    else
    {
        items = db->findOrders(search);
    }
    for (const bsoncxx::document::value& item : items) {
        QList<QStandardItem*> row;
        row << new QStandardItem(QString::fromStdString(std::string(item["_id"].get_oid().value.to_string())));
        row << new QStandardItem(QString::fromStdString(std::string(item["state"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["name"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["deviceBrand"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["deviceModel"].get_string().value)));
        row << new QStandardItem(QString::number(item["priceEstimate"].get_double().value));
        row << new QStandardItem(QString::number(item["priceFinal"].get_double().value));
        row << new QStandardItem(QString::fromStdString(std::string(item["dateReceived"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["dateStarted"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["dateCompleted"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["receivedByUser"].get_string().value)));
        row << new QStandardItem(QString::fromStdString(std::string(item["repairByUser"].get_string().value)));
        ItemModel->appendRow(row);
    }
    auto total = db->getTotalStatistic();
    ui->TotalLine->setText(QString::number(total[0]));
    ui->ActiveLine->setText(QString::number(total[1]));
    ui->CompletedLine->setText(QString::number(total[2]));
}

MainWindow::~MainWindow()
{
    delete ui;
}

// Натискання кнопки "Add"
void MainWindow::on_AddButton_clicked()
{
    auto orderWin = new OrderWindow(db,"Add");
    orderWin->show();
}

// Натискання кнопки "Edit"
void MainWindow::on_EditButton_clicked()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (!selectedRows.isEmpty())
    {
        int row = selectedRows.first().row();
        QString orderId = ItemModel->data(ItemModel->index(row, 0)).toString();
        auto orderWin = new OrderWindow(db,"Edit",orderId);
        orderWin->show();
    }
}

// Натискання кнопки "Delete"
void MainWindow::on_DeleteButton_clicked()
{
    QModelIndexList selectedRows = ui->tableView->selectionModel()->selectedRows();
    if (!selectedRows.isEmpty())
    {
        int row = selectedRows.first().row();
        QString orderId = ItemModel->data(ItemModel->index(row, 0)).toString();
        qDebug() << "Selected Order ID:" << orderId;
        db->deleteOrder(orderId);
        UpdateTable();
    }
}

// Пошук заявок
void MainWindow::on_SearchButton_clicked()
{
    try{
        UpdateTable(ui->Searchline->text());
    }
    catch(...)
    {
        QMessageBox::information(this, "Error", "Not found");
    }
}

// Перехід до вікна статистики
void MainWindow::on_actionShow_statistic_triggered()
{
    if(db->getUserRole() == "Technician" || db->getUserRole() == "Receptionist")
    {
        QMessageBox::warning(this, "Error", "You cant see full statistic");
    }
    else
    {
        auto statisticWin = new StatisticWindow(db);
        statisticWin->show();
    }
}

// Вихід з облікового запису
void MainWindow::on_actionLog_out_triggered()
{
    auto loginWin = new LoginWindow(db);
    loginWin->show();
    this->close();
}

// Меню: додавання заявки
void MainWindow::on_actionAdd_order_triggered()
{
    if(db->getUserRole() == "Technician")
    {
        QMessageBox::warning(this, "Error", "You cant add orders as Technician");
    }
    else on_AddButton_clicked();
}

// Меню: редагування заявки
void MainWindow::on_actionEdit_order_triggered()
{
    on_EditButton_clicked();
}

// Меню: видалення заявки
void MainWindow::on_actionDelete_order_triggered()
{
    if(db->getUserRole() == "Technician" || db->getUserRole() == "Receptionist")
    {
        QMessageBox::warning(this, "Error", "You cant delete orders");
    }
    else on_DeleteButton_clicked();
}

// Меню: інформація про програму
void MainWindow::on_actionShow_info_triggered()
{
    auto informationWin = new InformationWindow();
    informationWin->show();
}

