#include "existingdialog.h"
#include "ui_existingdialog.h"

ExistingDialog::ExistingDialog( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::ExistingDialog )
{
    ui->setupUi( this );
}

ExistingDialog::~ExistingDialog()
{
    delete ui;
}

void ExistingDialog::on_pushButton_2_released()
{
    accept();
}

void ExistingDialog::on_pushButton_released()
{
    reject();
}
