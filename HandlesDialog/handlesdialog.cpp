#include "handlesdialog.h"
#include "ui_handlesdialog.h"

HandlesDialog::HandlesDialog(QStringList handles, QWidget * parent) :
    QDialog(parent),
    ui(new Ui::HandlesDialog)
{
    ui->setupUi(this);
    this->ui->listWidget->addItems(handles);
    for(int i = 0; i < this->ui->listWidget->count(); i++) {
        if(Acc->checkHandle(this->ui->listWidget->item(i)->text())){
            this->ui->listWidget->item(i)->setSelected(true);
        }
        this->ui->listWidget->item(i)->setTextAlignment(Qt::AlignCenter);
    }

}

HandlesDialog::~HandlesDialog()
{
    delete ui;
}

void HandlesDialog::on_listWidget_itemSelectionChanged()
{
    this->allowed = this->ui->listWidget->selectedItems();

}

void HandlesDialog::on_cancel_released()
{
    this->reject();
}

void HandlesDialog::on_bind_released()
{
    this->accept();
}
