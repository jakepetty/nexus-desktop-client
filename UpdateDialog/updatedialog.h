#ifndef UPDATEDIALOG_H
#define UPDATEDIALOG_H

#include "common.h"
#include <QWidget>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QSystemTrayIcon>
#include <QMessageBox>

namespace Ui {
    class UpdateDialog;
}

class UpdateDialog : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateDialog( QWidget * parent = 0 );
    void setTrayIcon( QSystemTrayIcon * );
    void start( QUrl );
    ~UpdateDialog();
private slots:

    void downloadProgress( qint64, qint64 );

    void downloadError( QNetworkReply::NetworkError );

    void downloadFinished();

private:
    QNetworkRequest _request;
    QNetworkAccessManager * _manager;
    QNetworkReply * _reply;
    QSystemTrayIcon * _trayIcon;
    QUrl _downloadUrl;
    Ui::UpdateDialog * ui;
};

#endif // UPDATEDIALOG_H
