#ifndef FLEETDIALOG_H
#define FLEETDIALOG_H

#include "common.h"
#include <QDialog>
#include <QMovie>
#include <QDesktopServices>
#include <QMessageBox>
#include <QListWidgetItem>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

namespace Ui {
class FleetDialog;
}

class FleetDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FleetDialog(QWidget *parent = 0);
    void setupList();
    void init();
    void setupWizard() {
        this->isWizard = true;
    }

    int uploadCount = 0;
    ~FleetDialog();

private slots:
    void on_upload_btn_released();

    void on_delete_btn_released();

    void on_listWidget_itemSelectionChanged();
    void uploadProgress(qint64 bytesSent,qint64 bytesTotal);
    void uploadError(QNetworkReply::NetworkError error) {
        if(error != QNetworkReply::NoError) {
            popup("Upload Error", "Please try again. If you're uploading multiple fleet rosters at once, try uploading them one at a time.");
            if(APP_DEBUG) {
                qDebug() << "Upload Error: " << this->_reply->errorString();
            }
        }
    }

    void uploadFinished();
    void on_pushButton_released();

    void on_openBtn_released();

private:
    bool isWizard = false;
    bool is_initialized = false;
    Ui::FleetDialog *ui;
    QNetworkReply *_reply;
    QNetworkRequest _request;
    QNetworkAccessManager _manager;
    QListWidgetItem* getItemByFilename(QString);
};

#endif // FLEETDIALOG_H
