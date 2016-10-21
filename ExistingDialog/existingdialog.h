#ifndef EXISTINGDIALOG_H
#define EXISTINGDIALOG_H

#include <QDialog>

namespace Ui {
    class ExistingDialog;
}

class ExistingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExistingDialog( QWidget * parent = 0 );
    ~ExistingDialog();

private slots:
    void on_pushButton_2_released();

    void on_pushButton_released();

private:
    Ui::ExistingDialog * ui;
};

#endif // EXISTINGDIALOG_H
