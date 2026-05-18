#ifndef WARNINGWINDOW_H
#define WARNINGWINDOW_H

#include <QDialog>

namespace Ui {
class WarningWindow;
}

class WarningWindow : public QDialog
{
    Q_OBJECT

public:
    explicit WarningWindow(QWidget *parent = nullptr);
    ~WarningWindow();

private:
    Ui::WarningWindow *ui;
};

#endif // WARNINGWINDOW_H
