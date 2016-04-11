#ifndef SIGNUPDIALOG_H
#define SIGNUPDIALOG_H

#include <QDialog>
#include <QMessageBox>
#include <QDate>
#include <QLocale>
#include <QStyledItemDelegate>
#include "common.h"

namespace Ui {
class SignupDialog;
}

class SignupDialog : public QDialog
{
    Q_OBJECT

public:

    explicit SignupDialog(QWidget *parent = 0);
    ~SignupDialog();

    QString getPassword() {
        return this->password;
    }

    QString getEmail() {
        return this->email;
    }

private slots:

    void on_signup_btn_released();

    void on_login_btn_released();

private:
    QString email;
    QString password;
    Ui::SignupDialog *ui;
};

#endif // SIGNUPDIALOG_H
