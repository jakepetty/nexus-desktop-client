#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <QDialog>
#include <QTimer>
#include "Windows.h"
#include "Psapi.h"
#include "common.h"
#include "HandlesDialog/handlesdialog.h"
#include "FleetDialog/fleetdialog.h"

namespace Ui {
class SetupWizard;
}

class SetupWizard : public QDialog
{
    Q_OBJECT

public:
    explicit SetupWizard(QWidget *parent = 0);
    void init();
    ~SetupWizard();
private slots:
    void checkProcesses();

    void on_bindHandles_released();

    void on_selectFleets_released();

    void on_skipFleets_released();

    void on_continueBtn_released();

private:
    QTimer * timer;
    Ui::SetupWizard *ui;
};

#endif // SETUPWIZARD_H
