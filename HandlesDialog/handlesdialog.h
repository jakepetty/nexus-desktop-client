#ifndef HANDLESDIALOG_H
#define HANDLESDIALOG_H
#include "common.h"
#include <QDialog>
#include <QDebug>
#include <QListWidgetItem>

namespace Ui {
class HandlesDialog;
}

class HandlesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HandlesDialog(QStringList handles, QWidget * parent = 0);
    ~HandlesDialog();

    QList<QListWidgetItem *> getAllowed() {
        return this->allowed;
    }

private slots:
    void on_listWidget_itemSelectionChanged();

    void on_cancel_released();

    void on_bind_released();

private:
    QList<QListWidgetItem *> allowed;
    Ui::HandlesDialog *ui;
};

#endif // HANDLESDIALOG_H
