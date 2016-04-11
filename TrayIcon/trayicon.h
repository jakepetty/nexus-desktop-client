#ifndef TRAYICON_H
#define TRAYICON_H

#include "common.h"
#include "BackgroundWorkers/hotkeyworker.h"
#include "ScreenshotDialog/screenshotdialog.h"
#include "FleetDialog/fleetdialog.h"
#include "RealTimeWindow/realtimewindow.h"
#include "PreferencesWindow/preferenceswindow.h"
#include "UpdateDialog/updatedialog.h"
#include <QDesktopServices>
#include <QUrl>
#include <QObject>
#include <QTimer>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QMovie>

class TrayIcon : public QObject
{
    Q_OBJECT
public:
    QSystemTrayIcon *_icon;
    QMenu *_menu;
    int current_image = 0;
    QTimer *_animationTimer;
    FleetDialog * fleetManager;
    ScreenshotDialog * screeshotManager;
    RealTimeWindow * realTimeWindow;
    PreferencesWindow * preferencesWindow;
    UpdateDialog * update;
    QAction *_actionOpenCombatLogDirectory;
    QAction *_actionDeleteLogs;
    QAction *_actionMyAccount;
    QAction *_actionUpload;
    QAction *_actionFleetManager;
    QAction *_actionUpdate;
    QAction *_actionReset;
    QAction *_actionLogout;
    QAction *_actionRealTimeMonitor;
    QAction *_actionSettings;
    QAction *_actionScreenshots;
    QAction *_actionExit;
    explicit TrayIcon(QObject *parent = 0) : QObject(parent) {
        HotKeyWorker *workerThread = new HotKeyWorker(Acc->DesktopSetting["hotkey"].toString(), this);
        connect(workerThread, SIGNAL(onKeyPressed()), this, SLOT(onHotkeyPress()));
        workerThread->start();
    }

    ~TrayIcon() {

    }
    void startAnimation() {
        this->_animationTimer->start(1000 / 30);
    }

    void stopAnimation() {
        this->_animationTimer->stop();
        this->_icon->setIcon(QIcon(":/images/favicon.ico"));
    }

    void init(QObject *parent) {
        this->_animationTimer = new QTimer();
        connect(this->_animationTimer, SIGNAL(timeout()), this, SLOT(changeImage()));
        fleetManager = new FleetDialog();
        screeshotManager = new ScreenshotDialog();
        realTimeWindow = new RealTimeWindow();
        realTimeWindow->setupUploadBtn(parent);
        //realTimeWindow->init();
        preferencesWindow = new PreferencesWindow();
        preferencesWindow->setupRTW(realTimeWindow);
        update = new UpdateDialog();

        this->setupTrayIcon(parent);
        update->setTrayIcon(this->_icon);
        QString style = "QMenu { background-color: #303542;color:#b5bcc4;border:none;font-size:12px arial}"
                        "QMenu::item:selected {background-color:#24dba1;color:#fff}"
                        "QMenu::separator {margin:0;padding:0}"
                        "QMenu::item::disabled {color:#22252e}"
                        "QMenu::item::disabled:selected {color:#199970}"
                        "QMenu::item {padding:7px 30px}"
                        "QMenu::icon {padding:7px}";

        this->_menu = new QMenu();
        this->_menu->setStyleSheet(style);


        // END - Open Combat Log Directory Action

        // BEGIN - My Account Action
        this->_actionMyAccount = new QAction("{email}", this);
        this->_actionMyAccount->setIcon(QIcon(":/images/user.png"));
        connect(this->_actionMyAccount, SIGNAL(triggered()), this, SLOT(onClickMyAccount()));
        this->_menu->addAction(this->_actionMyAccount);
        // END - My Account Action

        this->_menu->addSeparator();

        // BEGIN - Screenshots Action
        this->_actionScreenshots = new QAction("Screenshot Manager", this);
        this->_actionScreenshots->setIcon(QIcon(":/images/screenshots.png"));
        connect(this->_actionScreenshots, SIGNAL(triggered()), this, SLOT(onClickScreenshots()));
        this->_menu->addAction(this->_actionScreenshots);
        // END - My Screenshots Action

        // BEGIN - Fleet Manager Action
        this->_actionFleetManager = new QAction("Fleet Roster Manager", this);
        this->_actionFleetManager->setIcon(QIcon(":/images/fleet.png"));
        connect(this->_actionFleetManager, SIGNAL(triggered()), this, SLOT(onClickFleets()));
        this->_menu->addAction(this->_actionFleetManager);
        // END - Fleet Manager Action

        this->_menu->addSeparator();

        // BEGIN - Open Combat Log Directory Action
        this->_actionOpenCombatLogDirectory = new QAction("Open Log Directory", this );
        this->_actionOpenCombatLogDirectory->setIcon(QIcon(":/images/folder.png"));
        connect(this->_actionOpenCombatLogDirectory, SIGNAL(triggered()), this, SLOT(onClickOpenCombatLogDirectory() ));
        this->_menu->addAction(this->_actionOpenCombatLogDirectory);
        // END - Open Combat Log Directory Action

        this->_menu->addSeparator();

        // BEGIN - Real-Time Monitor Action
        this->_actionRealTimeMonitor = new QAction("Real-Time Monitor", this);
        this->_actionRealTimeMonitor->setIcon(QIcon(":/images/monitor.png"));
        connect(this->_actionRealTimeMonitor, SIGNAL(triggered()), this, SLOT(onClickRealTimeMonitor()));
        this->_menu->addAction(this->_actionRealTimeMonitor);
        // END - Real-Time Monitor Action

        // BEGIN - Settings Action
        this->_actionSettings = new QAction("Preferences", this);
        this->_actionSettings->setIcon(QIcon(":/images/settings.png"));
        connect(this->_actionSettings, SIGNAL(triggered()), this, SLOT(onClickSettings()));
        this->_menu->addAction(this->_actionSettings);
        // END - Settings Action

        this->_menu->addSeparator();

        // BEGIN - Upload Action
        this->_actionUpload = new QAction("Upload", parent);
        this->_actionUpload->setDisabled(true);
        this->_actionUpload->setIcon(QIcon(":/images/upload_disabled.png"));
        connect(this->_actionUpload, SIGNAL(triggered()), parent, SLOT(onClickUpload()));
        this->_menu->addAction(this->_actionUpload);
        // END - Upload Action

        // BEGIN - Reset Log Action
        this->_actionReset = new QAction("Reset Log", this);
        this->_actionReset->setIcon(QIcon(":/images/reset.png"));
        connect(this->_actionReset, SIGNAL(triggered()), this, SLOT(onClickReset()));
        this->_menu->addAction(this->_actionReset);
        // END - Reset Log Action

        this->_menu->addSeparator();

        // BEGIN - Check For Updates Action
        this->_actionUpdate = new QAction("Check For Updates", this);
        this->_actionUpdate->setIcon(QIcon(":/images/update.png"));
        connect(this->_actionUpdate, SIGNAL(triggered()), this, SLOT(onClickUpdate()));
        this->_menu->addAction(this->_actionUpdate);
        // END - Check For Updates Action

        this->_menu->addSeparator();

        // BEGIN - Logout Action
        this->_actionLogout = new QAction("Logout", this);
        this->_actionLogout->setIcon(QIcon(":/images/logout.png"));
        connect(this->_actionLogout, SIGNAL(triggered()), this, SLOT(onClickLogout()));
        this->_menu->addAction(this->_actionLogout);
        // END - Logout Action

        // BEGIN - Exit Action
        this->_actionExit = new QAction( "Exit", this );
        this->_actionExit->setIcon(QIcon(":/images/exit.png"));
        connect(this->_actionExit, SIGNAL(triggered()), this, SLOT(onClickExit()));
        this->_menu->addAction(this->_actionExit);
        // END - Exit Action

        this->_icon->setContextMenu(this->_menu);

    }

    void setupTrayIcon(QObject *parent) {
        this->_icon = new QSystemTrayIcon(parent);
        this->_icon->setIcon(QIcon(":/images/favicon.ico"));
        this->_icon->setToolTip("Combat Log Uploader");
        this->_icon->setVisible(true);
        this->_icon->show();
        connect(this->_icon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), parent, SLOT(onClickTray(QSystemTrayIcon::ActivationReason)));
    }

    void setupTrayMenu(qint64 log_size = 0) {
        bool hasInstallDirectory = !Acc->User["install_directory"].isNull();
        this->_actionFleetManager->setEnabled(hasInstallDirectory);
        this->_actionScreenshots->setEnabled(hasInstallDirectory);
        this->_actionRealTimeMonitor->setEnabled(hasInstallDirectory);
        this->_actionOpenCombatLogDirectory->setEnabled(hasInstallDirectory);

        this->_actionMyAccount->setText(Acc->User["name"].toString());
        this->_actionUpload->setText(QString("Upload (%1)").arg(humanFileSize(log_size)));
        this->_actionReset->setDisabled(log_size == 0);
        this->_actionRealTimeMonitor->setShortcut(Acc->DesktopSetting["hotkey"].toString());
        this->_actionUpload->setDisabled(log_size < MIN_LOG_SIZE);
        if(this->_actionUpload->isEnabled()) {
            this->_actionUpload->setIcon(QIcon(":/images/upload.png"));
        } else {
            QIcon ico = QIcon(":/images/upload_disabled.png");
            ico.addPixmap(QPixmap(":/images/upload_disabled.png"), QIcon::Disabled);
            this->_actionUpload->setIcon(ico);
        }
        if(this->_actionReset->isEnabled()) {
            this->_actionReset->setIcon(QIcon(":/images/reset.png"));
        } else {
            QIcon ico = QIcon(":/images/reset_disabled.png");
            ico.addPixmap(QPixmap(":/images/reset_disabled.png"), QIcon::Disabled);
            this->_actionReset->setIcon(ico);
        }
    }

    void popup(QString title, QString msg, QSystemTrayIcon::MessageIcon ico = QSystemTrayIcon::Information) {
        this->_icon->showMessage(title, msg, ico);
    }
public slots:
    void onClickSettings() {
        preferencesWindow->init();
        preferencesWindow->show();
        preferencesWindow->raise();
    }

private slots:
    void changeImage() {
        this->_icon->setIcon(QIcon(QString(":/images/video/icon%0.png").arg(current_image)));
        if(current_image >= 19) {
            current_image = 0;
        } else {
            current_image++;
        }
    }

    void onClickMyAccount() {
        QDesktopServices::openUrl(QUrl(QString(URL_ACCOUNT).arg(Acc->User["username"].toString())));
    }

    void onClickLogout() {
        settings->clear();
        settings->sync();
        delete this->_icon;
        exit(0);
    }
    void onClickScreenshots() {
        screeshotManager->init();
        screeshotManager->setupList();
        screeshotManager->setAttribute(Qt::WA_DeleteOnClose);
        if(!screeshotManager->exec()) {
            screeshotManager = new ScreenshotDialog();
        }
    }
    void onClickFleets() {
        fleetManager->init();
        fleetManager->setupList();
        fleetManager->setAttribute(Qt::WA_DeleteOnClose);
        if(!fleetManager->exec()) {
            fleetManager = new FleetDialog();
        }
    }

    void onClickRealTimeMonitor() {
        this->realTimeWindow->init();
        this->realTimeWindow->setupConfigButton(this);
        this->realTimeWindow->show();
        this->realTimeWindow->raise();
    }

    void onClickReset() {
        STO->resetCombatLogs(Acc->User["install_directory"].toString() + LOGS);
    }

    void onClickOpenCombatLogDirectory() {
        QDesktopServices::openUrl(QUrl::fromLocalFile(Acc->User["install_directory"].toString() + LOGS));
    }

    void onClickExit() {
        delete this->_icon;
        exit(0);
    }

    void onClickUpdate()
    {
        this->startAnimation();
        if(Web->isLatestVersion(URL_VERSION, version())) {
            this->popup("Latest Version", "You are currently running the latest version.");
        } else {
            QJsonObject data = Web->getJSON(QSysInfo::buildAbi().contains("llp64") ? URL_UPDATE_64 : URL_UPDATE_32);
            if(data["url"].isString()) {
                update->start(QUrl(data["url"].toString()));
                update->show();
            } else {
                popup("MediaFire Error", data["error"].toString());
            }
        }
        this->stopAnimation();
    }

    void onHotkeyPress() {
        this->onClickRealTimeMonitor();
    }
};
#endif // TRAYICON_H
