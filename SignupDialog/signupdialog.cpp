#include "signupdialog.h"
#include "ui_signupdialog.h"

SignupDialog::SignupDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SignupDialog)
{
    ui->setupUi(this);
    QStyledItemDelegate* itemDelegate = new QStyledItemDelegate();
    this->ui->month->setItemDelegate(itemDelegate);
    this->ui->day->setItemDelegate(itemDelegate);
    this->ui->year->setItemDelegate(itemDelegate);
    this->ui->gender->setItemDelegate(itemDelegate);
    QDate date;

    for(int i = date.currentDate().year()-13; i > 1940; i--) {
        this->ui->year->addItem(QString::number(i));
    }
}

SignupDialog::~SignupDialog()
{
    delete ui;
}

void SignupDialog::on_signup_btn_released()
{
    QRegularExpression regex("^[0-9a-zA-Z]+([0-9a-zA-Z]*[-._+])*[0-9a-zA-Z]+@[0-9a-zA-Z]+([-.][0-9a-zA-Z]+)*([0-9a-zA-Z]*[.])[a-zA-Z]{2,6}$");
    if(ui->name->text().isEmpty()) {
        popup("Validation Error...","Display Name is a required field");
    } else if(ui->name->text().length() < 3) {
        popup("Validation Error...","Display Name must be at least 3 characters");
        ui->name->clear();
    } else if(ui->email->text().isEmpty()) {
        popup("Validation Error...","Email is a required field");
    } else if(!regex.match(ui->email->text()).hasMatch()) {
        popup("Validation Error...","The email you entered is invalid");
        ui->email->clear();
    } else if(ui->password->text().isEmpty()) {
        popup("Validation Error...","Password is a required field");
        ui->confirm_password->clear();
    } else if(ui->password->text().length() < 6) {
        popup("Validation Error...","Password must be atleast 6 characters long");
        ui->password->clear();
        ui->confirm_password->clear();
    } else if(ui->confirm_password->text().isEmpty()) {
        popup("Validation Error...","Confirm Password is a required field");
    } else if(ui->password->text() != ui->confirm_password->text()) {
        popup("Validation Error...","Passwords did not match");
        ui->password->clear();
        ui->confirm_password->clear();
    } else if(ui->gender->currentText() == "Gender") {
        popup("Validation Error...","Gender is required");
    } else if(ui->month->currentText() == "Month") {
        popup("Validation Error...","Month is required");
    } else if(ui->day->currentText() == "Day") {
        popup("Validation Error...","Day is required");
    } else if(ui->year->currentText() == "Year") {
        popup("Validation Error...","Year is required");
    } else {
        QLocale loc;
        QString i = QString::number(this->ui->month->currentIndex());
        QString month = this->ui->month->currentIndex() < 10 ? "0" + i : i;
        QUrlQuery post;
        post.addQueryItem("data[User][name]", ui->name->text());
        post.addQueryItem("data[User][timezone]", getTimeZoneOffset());
        post.addQueryItem("data[User][username]", ui->username->text());
        post.addQueryItem("data[User][app_version]", version());
        post.addQueryItem("data[User][email]", ui->email->text());
        post.addQueryItem("data[User][password]", ui->password->text());
        post.addQueryItem("data[User][confirm_password]", ui->confirm_password->text());
        post.addQueryItem("data[User][dob]", QString("%1-%2-%3").arg(this->ui->year->currentText(), month, this->ui->day->currentText()));
        post.addQueryItem("data[Gender][name]", this->ui->gender->currentText());
        QJsonObject json = Web->postJSON(URL_ADD_USER, post);
        if(json["code"].toInt() == 200) {
            this->email = this->ui->email->text();
            this->password = this->ui->password->text();

            this->accept();
        } else if(json["code"].toInt() == 620) {
            QString errors;
            int i = 1;

            foreach(QJsonValue items, json["errors"].toArray()) {
                foreach(QJsonValue error, items.toArray()) {
                    errors += "<p><strong>Error "+QString::number(i)+":</strong> " + error.toString() + "</p>";
                    i++;
                }
            }
            if(!errors.isEmpty()) {
                popup("Error(s)", errors);
            }
        } else {
            popup("Exception Error...", "Something unexpected happened while processing your request. Please wait a few minutes and try again.");
        }
    }
}

void SignupDialog::on_login_btn_released()
{
    this->close();
}
