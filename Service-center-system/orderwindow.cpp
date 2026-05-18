#include "orderwindow.h"
#include "ui_orderwindow.h"

OrderWindow::OrderWindow(DataBase* dbСonnect,QString windowType, QString orderId, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::OrderWindow)
    , db(dbСonnect)
{
    ui->setupUi(this);
    type = windowType;
    SetFields(orderId);
    SetRestrict();
}

OrderWindow::~OrderWindow()
{
    delete ui;
}

// Налаштування та заповнення полів форми
void OrderWindow::SetFields(QString orderId)
{
    if (type == "Add")
    {
        ui->StatusComboBox->setCurrentIndex(ui->StatusComboBox->findText("Accepted"));
        ui->ReceivedDateEdit->setDate(QDate::currentDate());
        ui->DiagnosticInfoTextEdit->setVisible(false);
        ui->DiagnosticInfoLabel->setVisible(false);
        ui->WorksDoneTextEdit->setVisible(false);
        ui->WorksDoneLabel->setVisible(false);
        ui->StartedDateEdit->setVisible(false);
        ui->StartedDateLabel->setVisible(false);
        ui->CompletedDateEdit->setVisible(false);
        ui->CompletedDateLabel->setVisible(false);
        ui->FinalPriceSpinBox->setVisible(false);
        ui->FinalPriceLabel->setVisible(false);
        ui->RepairByLineEdit->setVisible(false);
        ui->RepairByLabel->setVisible(false);
        ui->StatusComboBox->setEnabled(false);
        ui->EstimatePriceSpinBox->setValue(0);
        ui->FinalPriceSpinBox->setValue(0);
        ui->PrepaymentSpinBox->setValue(0);
        if (db->getUserRole() == "Receptionist") ui->ReceivedByLineEdit->setText(db->getUserName());
    }
    if (type == "Edit")
    {
        bsoncxx::document::value doc = db->findOrder(orderId);
        previousStatus = QString::fromStdString(std::string(doc["state"].get_string().value));
        ui->OrderIDLineEdit->setText(
            QString::fromStdString(std::string(doc["_id"].get_oid().value.to_string())));
        ui->StatusComboBox->setCurrentText(
            QString::fromStdString(std::string(doc["state"].get_string().value)));
        ui->CustomerNameLineEdit->setText(
            QString::fromStdString(std::string(doc["name"].get_string().value)));
        ui->CustomerPhoneLineEdit->setText(
            QString::fromStdString(std::string(doc["phone"].get_string().value)));
        ui->ProvidedDevicesLineEdit->setText(
            QString::fromStdString(std::string(doc["providedDevices"].get_string().value)));
        ui->BrandLineEdit->setText(
            QString::fromStdString(std::string(doc["deviceBrand"].get_string().value)));
        ui->ModelLineEdit->setText(
            QString::fromStdString(std::string(doc["deviceModel"].get_string().value)));
        ui->SerialNumberLineEdit->setText(
            QString::fromStdString(std::string(doc["serialNumber"].get_string().value)));
        ui->IMEILineEdit->setText(
            QString::fromStdString(std::string(doc["imei"].get_string().value)));
        ui->IssueDescriptionTextEdit->setPlainText(
            QString::fromStdString(std::string(doc["issueDescription"].get_string().value)));
        ui->DiagnosticInfoTextEdit->setPlainText(
            QString::fromStdString(std::string(doc["diagnosticInfo"].get_string().value)));
        ui->WorksDoneTextEdit->setPlainText(
            QString::fromStdString(std::string(doc["worksDone"].get_string().value)));
        ui->ReceivedDateEdit->setDate(
            QDate::fromString(QString::fromStdString(std::string(doc["dateReceived"].get_string().value)), "yyyy-MM-dd"));
        ui->StartedDateEdit->setDate(
            QDate::fromString(QString::fromStdString(std::string(doc["dateStarted"].get_string().value)), "yyyy-MM-dd"));
        ui->CompletedDateEdit->setDate(
            QDate::fromString(QString::fromStdString(std::string(doc["dateCompleted"].get_string().value)), "yyyy-MM-dd"));

        ui->EstimatePriceSpinBox->setValue(doc["priceEstimate"].get_double().value);
        ui->FinalPriceSpinBox->setValue(doc["priceFinal"].get_double().value);
        ui->PrepaymentSpinBox->setValue(doc["prepayment"].get_double().value);
        ui->ReceivedByLineEdit->setText(
            QString::fromStdString(std::string(doc["receivedByUser"].get_string().value)));
        ui->RepairByLineEdit->setText(
            QString::fromStdString(std::string(doc["repairByUser"].get_string().value)));
        if (db->getUserRole() == "Technician" && ui->RepairByLineEdit->text().toStdString() == "") ui->RepairByLineEdit->setText(db->getUserName());
    }
}

// Обмеження редагування полів відповідно до ролі користувача
void OrderWindow::SetRestrict()
{
    if (db->getUserRole() == "Receptionist")
    {
        ui->StatusComboBox->removeItem(ui->StatusComboBox->findText("Under repair"));
        ui->StatusComboBox->removeItem(ui->StatusComboBox->findText("Ready"));
        ui->StatusComboBox->removeItem(ui->StatusComboBox->findText("Under diagnostics"));
        ui->DiagnosticInfoTextEdit->setReadOnly(true);
        ui->WorksDoneTextEdit->setReadOnly(true);
        ui->StartedDateEdit->setReadOnly(true);
        ui->CompletedDateEdit->setReadOnly(true);
        ui->FinalPriceSpinBox->setReadOnly(true);
        ui->RepairByLineEdit->setReadOnly(true);
    }
    else if (db->getUserRole() == "Technician")
    {
        ui->StatusComboBox->removeItem(ui->StatusComboBox->findText("Issued"));
        ui->CustomerNameLineEdit->setReadOnly(true);
        ui->CustomerPhoneLineEdit->setReadOnly(true);
        ui->ProvidedDevicesLineEdit->setReadOnly(true);
        ui->BrandLineEdit->setReadOnly(true);
        ui->ModelLineEdit->setReadOnly(true);
        ui->SerialNumberLineEdit->setReadOnly(true);
        ui->IMEILineEdit->setReadOnly(true);
        ui->IssueDescriptionTextEdit->setReadOnly(true);
        ui->ReceivedDateEdit->setReadOnly(true);
        ui->EstimatePriceSpinBox->setReadOnly(true);
        ui->PrepaymentSpinBox->setReadOnly(true);
        ui->ReceivedByLineEdit->setReadOnly(true);
    }
}

// Перевірка коректності введених даних
bool OrderWindow::Check()
{
    if (ui->CustomerNameLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Customer name cannot be empty.");
        return false;
    }
    QString phone = ui->CustomerPhoneLineEdit->text();
    if (phone.isEmpty())
    {
        QMessageBox::warning(this, "Error", "Phone number cannot be empty.");
        return false;
    }
    QRegularExpression re("^[0-9]{10}$");
    if (!re.match(phone).hasMatch())
    {
        QMessageBox::warning(this, "Error", "Phone number must contain exactly 10 digits.");
        return false;
    }
    if (ui->ProvidedDevicesLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Provided device field cannot be empty.");
        return false;
    }
    if (ui->BrandLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Device brand cannot be empty.");
        return false;
    }
    if (ui->ModelLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Device model cannot be empty.");
        return false;
    }
    if (ui->IssueDescriptionTextEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Issue description cannot be empty.");
        return false;
    }
    if (db->getUserRole() == "Technician" &&  ui->StatusComboBox->currentText() == "Under repair" && ui->DiagnosticInfoTextEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Diagnostic info must be filled by Technician before setting status to Under repair.");
        return false;
    }
    if (db->getUserRole() == "Technician" && ui->StatusComboBox->currentText() == "Ready" && ui->WorksDoneTextEdit->toPlainText().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Technician must describe works done before setting status to Ready.");
        return false;
    }
    if (ui->ReceivedByLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Received by user cannot be empty.");
        return false;
    }
    if (ui->RepairByLineEdit->text().isEmpty() && (ui->StatusComboBox->currentText() == "Under repair" || ui->StatusComboBox->currentText() == "Ready"))
    {
        QMessageBox::warning(this, "Error", "Repair technician must be specified.");
        return false;
    }
    if (ui->SerialNumberLineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "Serial number cannot be empty.");
        return false;
    }
    if (ui->IMEILineEdit->text().isEmpty())
    {
        QMessageBox::warning(this, "Error", "IMEI number cannot be empty.");
        return false;
    }
    if (ui->IMEILineEdit->text().length() < 10)
    {
        QMessageBox::warning(this, "Error", "IMEI number is too short.");
        return false;
    }
    QDate dateReceived = ui->ReceivedDateEdit->date();
    QDate dateStarted  = ui->StartedDateEdit->date();
    QDate dateCompleted = ui->CompletedDateEdit->date();
    if (dateStarted < dateReceived)
    {
        QMessageBox::warning(this, "Error", "Start date cannot be earlier than received date.");
        return false;
    }
    if (dateCompleted < dateReceived)
    {
        QMessageBox::warning(this, "Error", "Completion date cannot be earlier than received date.");
        return false;
    }
    if (dateCompleted < dateStarted)
    {
        QMessageBox::warning(this, "Error", "Completion date cannot be earlier than start date.");
        return false;
    }
    if(type == "Edit")
    {
        QString oldStatus = previousStatus;
        QString newStatus = ui->StatusComboBox->currentText();
        // Map allowed transitions
        bool allowed = false;
        // Accepted → Under diagnostics
        if (oldStatus == "Accepted" && newStatus == "Under diagnostics") allowed = true;
        // Under diagnostics → Under repair
        if (oldStatus == "Under diagnostics" && newStatus == "Under repair") allowed = true;
        // Under repair → Ready
        if (oldStatus == "Under repair" && newStatus == "Ready") allowed = true;
        // Ready → Issued
        if (oldStatus == "Ready" && newStatus == "Issued") allowed = true;
        // Issued → (no transitions allowed)
        if (oldStatus == "Issued" && newStatus == "Issued") allowed = true;
        if (!allowed && oldStatus != newStatus)
        {
            QMessageBox::warning(this, "Error", "Invalid status transition:\n\n" + oldStatus + " → " + newStatus +
                    "\n\nThis transition is not allowed by workflow.");
            return false;
        }
    }
    return true;
}

// Обробка кнопки "Accept"
void OrderWindow::on_AcceptButton_clicked()
{
    if(type == "Add")
    {
        ui->StartedDateEdit->setDate(ui->ReceivedDateEdit->date());
        ui->CompletedDateEdit->setDate(ui->ReceivedDateEdit->date());
    }
    if (Check() == true)
    {
        auto document = bsoncxx::builder::stream::document{}
                        << "state" << ui->StatusComboBox->currentText().toStdString()
                        << "name" << ui->CustomerNameLineEdit->text().toStdString()
                        << "phone" << ui->CustomerPhoneLineEdit->text().toStdString()
                        << "providedDevices" << ui->ProvidedDevicesLineEdit->text().toStdString()
                        << "deviceBrand" << ui->BrandLineEdit->text().toStdString()
                        << "deviceModel" << ui->ModelLineEdit->text().toStdString()
                        << "serialNumber" << ui->SerialNumberLineEdit->text().toStdString()
                        << "imei" << ui->IMEILineEdit->text().toStdString()
                        << "issueDescription" << ui->IssueDescriptionTextEdit->toPlainText().toStdString()
                        << "diagnosticInfo" << ui->DiagnosticInfoTextEdit->toPlainText().toStdString()
                        << "worksDone" << ui->WorksDoneTextEdit->toPlainText().toStdString()
                        << "dateReceived" << ui->ReceivedDateEdit->date().toString("yyyy-MM-dd").toStdString()
                        << "dateStarted" << ui->StartedDateEdit->date().toString("yyyy-MM-dd").toStdString()
                        << "dateCompleted" << ui->CompletedDateEdit->date().toString("yyyy-MM-dd").toStdString()
                        << "priceEstimate" << ui->EstimatePriceSpinBox->value()
                        << "priceFinal" << ui->FinalPriceSpinBox->value()
                        << "prepayment" << ui->PrepaymentSpinBox->value()
                        << "receivedByUser" << ui->ReceivedByLineEdit->text().toStdString()
                        << "repairByUser" << ui->RepairByLineEdit->text().toStdString()
                        << bsoncxx::builder::stream::finalize;
        if(type == "Add")
        {
            db->insertOrder(document);
        }
        if (type == "Edit")
        {
            db->updateOrder(ui->OrderIDLineEdit->text(),document);
        }
        this->close();
    }
}

// Скасування операції
void OrderWindow::on_CancelButton_clicked()
{
    this->close();
}

