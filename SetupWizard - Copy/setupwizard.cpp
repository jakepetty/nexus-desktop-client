#include "setupwizard.h"
#include "HandlesDialog/handlesdialog.h"
#include "FleetDialog/fleetdialog.h"
#include "ui_setupwizard.h"

SetupWizard::SetupWizard(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::SetupWizard)
{
    ui->setupUi(this);
    ui->tabWidget->setTabEnabled(1, false);
    ui->tabWidget->setTabEnabled(2, false);
    this->timer = new QTimer(this);
        connect(this->timer, SIGNAL(timeout()), this, SLOT(checkProcesses()));
        timer->start(1000);
}
void SetupWizard::checkProcesses(){
    QString callback;
    QStringList processList;
    unsigned long aProcesses[2048], cbNeeded, running_processes;
    if(EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)){
        running_processes = cbNeeded / sizeof(unsigned long);
        for(unsigned int i = 0; i < running_processes; i++) {
            HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, aProcesses[i]);
            wchar_t buffer[512];
            GetModuleFileNameEx(hProcess, 0, buffer, 512);
            CloseHandle(hProcess);
            QString path = QString::fromWCharArray(buffer);
            if(!processList.contains(path)){
                if(path.contains("GameClient.exe") && path.contains("Star Trek Online")){
                    callback = path.replace(DS "GameClient.exe", NULL);
                }
                processList.append(path);
            }
        }
    }
    if(!callback.isEmpty()){
        this->timer->stop();
        saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "install_directory", callback);
        ui->tabWidget->setTabEnabled(0, false);
        ui->tabWidget->setTabEnabled(1, true);
        ui->tabWidget->setCurrentIndex(1);
        ui->bindHandles->setEnabled(true);
    }
}

SetupWizard::~SetupWizard()
{
    delete ui;
}

void SetupWizard::on_bindHandles_released()
{

    STO->parseShutdownLog(Acc->User["install_directory"].toString() + LOGS + "Shutdown.Log");
    QStringList handles = STO->getHandles();
    if(handles.isEmpty()) {
        popup("Problem detecting handles", "Please verify your install directory is accurate if it is then please contact us with a bug report.");
    }else{
        HandlesDialog *handleDlg = new HandlesDialog(handles);
        if(handleDlg->exec()){

            QUrlQuery post;
            post.addQueryItem("data[Handle][user_id]", QString::number(Acc->User["id"].toInt()));
            Web->postJSON(URL_UNBIND_HANDLES, post);
            post.clear();
            foreach(QListWidgetItem *handle, handleDlg->getAllowed()){
                post.addQueryItem("data[Handle][user_id]", QString::number(Acc->User["id"].toInt()));
                post.addQueryItem("data[Handle][name]", handle->text());
                int i = 0;
                foreach(QString character, STO->getCharactersByHandle(handle->text())){
                    post.addQueryItem(QString("data[Character][%0][name]").arg(i), character);
                    i++;
                }
                QJsonObject callback = Web->postJSON(URL_BIND_HANDLE, post);
                post.clear();
            }

            ui->tabWidget->setTabEnabled(1, false);
            ui->bindHandles->setEnabled(false);
            ui->tabWidget->setTabEnabled(2, true);
            ui->selectFleets->setEnabled(true);
            ui->tabWidget->setCurrentIndex(2);
        }
    }
}

void SetupWizard::on_selectFleets_released()
{
    FleetDialog * fleetManager = new FleetDialog();
    fleetManager->setupList();
    fleetManager->exec();
    if(fleetManager->uploadCount >= 1){
        popup("Setup Complete", "You have successfully set up your account!");
        this->close();
    }
}
