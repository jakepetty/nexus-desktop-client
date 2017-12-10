#include "setupwizard.h"
#include "ui_setupwizard.h"

SetupWizard::SetupWizard( QWidget * parent ) :
    QDialog( parent, Qt::Dialog ),
    ui( new Ui::SetupWizard )
{
    ui->setupUi( this );
}

SetupWizard::~SetupWizard()
{
    delete ui;
}
void SetupWizard::init() {
    ui->tabWidget->setTabEnabled( 1, false );
    ui->tabWidget->setTabEnabled( 2, false );
    ui->tabWidget->setTabEnabled( 3, false );
    this->timer = new QTimer( this );
    connect( this->timer, SIGNAL(timeout()), this, SLOT(checkProcesses()));
    timer->start( 1000 );
}

void SetupWizard::checkProcesses() {
    QString callback;
    QStringList processList;
    unsigned long aProcesses[2048], cbNeeded, running_processes;
    if ( EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded )) {
        running_processes = cbNeeded / sizeof(unsigned long);
        for ( unsigned int i = 0; i < running_processes; i++ ) {
            HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, aProcesses[i] );
            wchar_t buffer[512];
            GetModuleFileNameEx( hProcess, 0, buffer, 512 );
            CloseHandle( hProcess );
            QString path = QString::fromWCharArray( buffer );
            if ( !processList.contains( path )) {
                if ( path.contains( "GameClient.exe" ) && path.contains( "Star Trek Online" )) {
                    callback = STO->ramdiskToPhysical( path.replace( DS "GameClient.exe", NULL ).replace( DS "x86", NULL ).replace( DS "x64", NULL ));
                }
                processList.append( path );
            }
        }
    }
    if ( !callback.isEmpty()) {
        callback = STO->ramdiskToPhysical( callback );
        this->timer->stop();
        saveField( URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "install_directory", callback );
        STO->clearCombatLogs( callback );
        ui->tabWidget->setTabEnabled( 0, false );
        ui->tabWidget->setTabEnabled( 1, true );
        ui->tabWidget->setCurrentIndex( 1 );
        ui->bindHandles->setEnabled( true );
        activateWindow();
        raise();
    }
}

void SetupWizard::on_bindHandles_released()
{

    QStringList handles = STO->getHandles( Acc->User["install_directory"].toString() + LOGS );
    if ( handles.isEmpty()) {
        popup( "Problem detecting handles", "Please verify your install directory is accurate if it is then please contact us with a bug report." );
    } else {
        HandlesDialog * handleDlg = new HandlesDialog( handles );
        int success = 0;
        if ( handleDlg->exec()) {

            QUrlQuery post;
            post.addQueryItem( "data[Handle][user_id]", QString::number( Acc->User["id"].toInt()));
            Web->postJSON( URL_UNBIND_HANDLES, post );
            post.clear();
            foreach( QListWidgetItem * handle, handleDlg->getAllowed()) {
                post.addQueryItem( "data[Handle][user_id]", QString::number( Acc->User["id"].toInt()));
                post.addQueryItem( "data[Handle][name]", handle->text());
                int i = 0;
                foreach( QString character, STO->getCharactersByHandle( Acc->User["install_directory"].toString() + LOGS, handle->text())) {
                    post.addQueryItem( QString( "data[Character][%0][name]" ).arg( i ), character );
                    i++;
                }
                QJsonObject callback = Web->postJSON( URL_BIND_HANDLE, post );
                if ( callback["code"] == 200 ) {
                    success++;
                }
                post.clear();
            }
            if ( success > 0 ) {
                ui->tabWidget->setTabEnabled( 1, false );
                ui->bindHandles->setEnabled( false );

                ui->tabWidget->setTabEnabled( 2, true );
                ui->selectFleets->setEnabled( true );
                ui->skipFleets->setEnabled( true );

                ui->tabWidget->setCurrentIndex( 2 );
            } else {
                popup( "Handle Binding Error", "Handling binding is required in order to proceed. If none of your handles appeared in the list, please choose a character from your character select screen in-game and after the loading screen type /quit and click Bind Handles again." );
            }
        }
    }
}

void SetupWizard::on_selectFleets_released()
{
    FleetDialog * fleetManager = new FleetDialog();
    fleetManager->setupWizard();
    fleetManager->setupList();
    fleetManager->init();
    fleetManager->exec();
    if ( fleetManager->uploadCount >= 1 ) {
        ui->tabWidget->setTabEnabled( 2, false );
        ui->selectFleets->setEnabled( false );
        ui->skipFleets->setEnabled( false );

        ui->tabWidget->setTabEnabled( 3, true );
        ui->continueBtn->setEnabled( true );

        ui->tabWidget->setCurrentIndex( 3 );
    }
}


void SetupWizard::on_skipFleets_released()
{
    if ( confirm( "If you don't submit a fleet roster your characters won't have career, faction or level information this can be setup later it's just a reminder. Are you sure you want to skip this step?" )) {

        ui->tabWidget->setTabEnabled( 2, false );
        ui->selectFleets->setEnabled( false );
        ui->skipFleets->setEnabled( false );

        ui->tabWidget->setTabEnabled( 3, true );
        ui->continueBtn->setEnabled( true );

        ui->tabWidget->setCurrentIndex( 3 );
    }
}

void SetupWizard::on_continueBtn_released()
{
    saveField( URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_setup", "1" );
    relog();
    this->accept();
}
