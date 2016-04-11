#ifndef APP_H
#define APP_H
#include "common.h"
#include "windows.h"
#include "BackgroundWorkers/timeconversionworker.h"
#include "BackgroundWorkers/hotkeyworker.h"
#include "PreferencesWindow/preferenceswindow.h"
#include "RealTimeWindow/realtimewindow.h"
#include "UpdateDialog/updatedialog.h"
#include "TrayIcon/trayicon.h"
#include <QDir>
#include <QTimer>
#include <QClipboard>
#include <QDesktopServices>

class App : public QObject
{
    Q_OBJECT
public:
    bool is_uploading;
    TrayIcon * Tray;
    QNetworkReply *_reply;
    QNetworkRequest _request;
    QNetworkAccessManager _manager;
    App(QObject * parent = 0) : QObject(parent) {
        this->Tray = new TrayIcon(this);
        is_uploading = false;

    }

    ~App() {

    }

    void init() {
        this->Tray->init(this);
    }

    void Do_Upload() {
        if(!this->is_uploading) {

            // Check if website is available
            if(Web->isAvailable(URL_BASE)) {

                // Check if app is current version
                if(Web->isLatestVersion(URL_VERSION, version())) {
                    this->Tray->startAnimation();

                    // Load the CombatLog file
                    QByteArray fileData = STO->readCombatLog(Acc->User["install_directory"].toString() + LOGS);

                    // Check to see if the log file is empty
                    if(!fileData.isEmpty()) {
                        // Check if the CombatLog is valid
                        if(STO->validateCombatLog(fileData)) {
                            QStringList logHandles = STO->getLogHandles(fileData);
                            QStringList handles = STO->getHandles(Acc->User["install_directory"].toString() + LOGS);
                            bool handle_found = false;
                            foreach(QString handle, logHandles) {
                                if(handles.contains(handle)) {
                                    handle_found = true;
                                    break;
                                }
                            }
                            // Check if the CombatLog contains the uploaders handle
                            if(handle_found) {
                                // Upload CombatLog
                                if(!this->is_uploading) {
                                    this->is_uploading = true;

                                    // Convert log times to UTC
                                    TimeConversionWorker *workerThread = new TimeConversionWorker(fileData);
                                    connect(workerThread, SIGNAL(onComplete(QByteArray, QString)), SLOT(onConversionComplete(QByteArray, QString)));
                                    connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
                                    workerThread->start();
                                }
                            } else {
                                this->Tray->popup("Handle Not Found...", "Unable to detect any of your handles in this combat log", QSystemTrayIcon::Warning);
                                this->Tray->stopAnimation();
                            }
                        } else {
                            this->Tray->popup("Invalid Log File", "Your log file is invalid and can't be uploaded.", QSystemTrayIcon::Warning);
                            this->Tray->stopAnimation();
                        }
                    } else {
                        this->Tray->popup("Empty Log File", "Your log file does not contain any information.", QSystemTrayIcon::Warning);
                        this->Tray->stopAnimation();
                    }
                } else {
                    this->Tray->popup("Update Required", "Your client is out of date! Please restart this application to update.", QSystemTrayIcon::Warning);
                }
            } else {
                this->Tray->popup("Service Unavailable", "We apologize for the inconvenience but our server isn't responding at the moment. Please try again in a few minutes.");
            }
        } else {
            this->Tray->popup("Already Uploading", "There is currently an upload already in progress");
        }
    }
public slots:
    void onConversionComplete(QByteArray fileData, QString players) {
        QString hash = sha1(players);
        qDebug() << hash;
        QJsonObject check = Web->getJSON(QString(URL_REPORT_CHECK).arg(hash));
        if(check["code"].toInt() == 200) {
            QDesktopServices::openUrl(QUrl(QString(URL_REPORT).arg(check["seo_url"].toString())));
            this->Tray->stopAnimation();
            this->is_uploading = false;
            STO->clearCombatLogs(Acc->User["install_directory"].toString() + LOGS);
        } else {

            this->_request.setUrl(QUrl(URL_UPLOAD_REPORT));
            this->_reply = this->_manager.get(this->_request);

            QString bound = "STO";

            QByteArray data(QString("--" + bound + "\r\n").toUtf8());
            data.append("Content-Disposition: form-data; name=\"_method\"\r\n\r\n");
            data.append("POST\r\n");
            data.append("--" + bound + "\r\n");
            data.append("Content-Disposition: form-data; name=\"data[Report][user_id]\"\r\n\r\n");
            data.append(QString::number(Acc->User["id"].toInt()) + "\r\n");
            data.append("--" + bound + "\r\n");
            data.append("Content-Disposition: form-data; name=\"data[Report][hash]\"\r\n\r\n");
            data.append(hash + "\r\n");
            data.append("--" + bound + "\r\n");
            data.append("Content-Disposition: form-data; name=\"data[Report][log]\"; filename=\"Combatlog.Log\"\r\n");
            data.append("Content-Type: application/octet-stream\r\n\r\n");

            // Compress
            data.append(compress(fileData));
            data.append("\r\n");
            data.append("--" + bound + "\r\n");

            this->_request.setRawHeader(QString("Content-Type").toUtf8(),QString("multipart/form-data; boundary=" + bound).toUtf8());
            this->_request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(data.length()).toUtf8());
            this->_reply = this->_manager.post(this->_request, data);
            serviceCheck("Uploading Combatlog", "Started");
            connect(this->_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
            connect(this->_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgress(qint64,qint64)));
            connect(this->_reply, SIGNAL(finished()),this, SLOT(uploadFinished()));
        }
    }

    void uploadProgress(qint64 bytesSent,qint64 bytesTotal) {
        qreal speed = ((bytesSent * 1.0) / bytesTotal);
        if(speed == 1) {
            serviceCheck("Uploading Combatlog", "Done");
        } else if(bytesTotal > 0) {
            serviceCheck("Uploading Combatlog", QString::number(speed * 100, 'f', 2) + "%");
        }
    }

    void uploadError(QNetworkReply::NetworkError error) {
        if(error != QNetworkReply::NoError) {
            QString err = this->_reply->errorString();
            if(!err.isEmpty()) {
                if(err == "Unable to Write") {
                    this->Tray->popup("Connection Error...", "The clients connection to the server was aborted.", QSystemTrayIcon::Critical);
                } else {
                    this->Tray->popup("Server Error...", err, QSystemTrayIcon::Critical);
                }
            } else {
                this->Tray->popup("Server Error...", "There was an issue with our server please try again.", QSystemTrayIcon::Critical);
            }
            serviceCheck("Uploading Combatlog", this->_reply->errorString());
        }

        this->is_uploading = false;
    }

    void uploadFinished() {
        if(this->_reply->error() == QNetworkReply::NoError) {
            QByteArray data = this->_reply->readAll();
            QJsonDocument doc(QJsonDocument::fromJson(data));
            QJsonObject json = doc.object();
            int code = json["code"].toInt();
            if(code == 200) {
                STO->clearCombatLogs(Acc->User["install_directory"].toString() + LOGS);

                QString seo = json["seo"].toString();
                QString url = QString(URL_REPORT_LOGIN).arg(Acc->User["identifier"].toString()).arg(seo);
                QClipboard *clipboard = QApplication::clipboard();
                clipboard->setText(QString(URL_REPORT).arg(seo));
                QDesktopServices::openUrl(QUrl(url));
            } else {
                popup("An error has occured", "Something went wrong while processing your combat log. Please try again.");
            }
            serviceCheck("Processing CombatLog", QString::number(code));
        } else {
            serviceCheck("Processing CombatLog", this->_reply->errorString());
        }

        this->Tray->stopAnimation();
        this->is_uploading = false;
    }

    void onClickUpload() {
        this->Do_Upload();
    }

    void onClickTray(const QSystemTrayIcon::ActivationReason& reason) {
        QStringList logs = STO->combatlogs(Acc->User["install_directory"].toString() + LOGS);
        if(reason == 2) {
            if(!logs.isEmpty()) {
                if(getFileSize(logs) >= MIN_LOG_SIZE) {
                    Do_Upload();
                } else {
                    popup("Insufficient Data", "Not enough combat took place to guarantee an accurate result.");
                }
            } else {
                this->Tray->onClickSettings();
            }
        } else if(reason == 1) {
            this->Tray->setupTrayMenu(getFileSize(logs));
        }
    }

};

#endif // APP_H
