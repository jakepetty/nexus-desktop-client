#ifndef SETUPWIZARD_H
#define SETUPWIZARD_H

#include <QMainWindow>
#include <QTimer>
#include "Windows.h"
#include "Psapi.h"
#include "common.h"

namespace Ui {
class SetupWizard;
}

class SetupWizard : public QMainWindow
{
    Q_OBJECT

public:
    explicit SetupWizard(QWidget *parent = 0);
    ~SetupWizard();
private slots:
    void checkProcesses();

    void on_bindHandles_released();

    void on_selectFleets_released();

private:
    QTimer * timer;
    Ui::SetupWizard *ui;
};

#endif // SETUPWIZARD_H
