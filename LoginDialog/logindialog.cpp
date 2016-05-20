#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(QWidget * parent) :
    QDialog(parent, Qt::WindowSystemMenuHint | Qt::WindowTitleHint | Qt::WindowCloseButtonHint),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
}
void LoginDialog::init() {
    ui->error->hide(); // Hides visiblity of the error notification
    ui->frame->setFocus(); // Needed otherwise email gets focused removing the watermark text
    this->resize(this->width(), this->minimumHeight());
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::on_login_released()
{
    ui->error->hide();
    this->resize(this->width(), this->minimumHeight());
    QUrlQuery post;
    post.addQueryItem("data[User][username]", ui->username->text());
    post.addQueryItem("data[User][timezone]", getTimeZoneOffset());
    post.addQueryItem("data[User][password]", sha1(ui->password->text()));

    QJsonObject user = Web->postJSON(URL_LOGIN, post);
    if(APP_DEBUG) {
        qDebug() << user;
    }
    // Check if user exists with login credentials
    if(user["code"].toInt() == 200) {

        // Populate User Structure
        Acc->setup(user);

        // Save login info to file
        if(ui->remember_me->isChecked()) {
            settings->setValue("email", ui->username->text());
            settings->setValue("password", sha1(ui->password->text()));
            settings->sync();
        }

        accept();
    } else {
        ui->error->show();
    }
}

void LoginDialog::on_signup_released()
{
    SignupDialog * sWin = new SignupDialog();
    if(sWin->exec()) {
        this->ui->username->setText(sWin->getEmail());
        this->ui->password->setText(sWin->getPassword());
        this->ui->remember_me->setChecked(true);
        this->on_login_released();
    }
}
