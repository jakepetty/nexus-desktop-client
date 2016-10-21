#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include <QAbstractButton>
#include <QUrlQuery>
#include <QJsonObject>
#include "SignupDialog/signupdialog.h"
#include "common.h"

namespace Ui {
    class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog( QWidget * parent = 0 );
    void init();
    ~LoginDialog();

private slots:

    void on_login_released();

    void on_signup_released();

private:
    Ui::LoginDialog * ui;
};

#endif // LOGINDIALOG_H
