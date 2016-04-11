#include "updatedialog.h"
#include "ui_updatedialog.h"

UpdateDialog::UpdateDialog(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateDialog)
{
    ui->setupUi(this);
}
void UpdateDialog::setTrayIcon(QSystemTrayIcon *_trayIcon) {
    this->_trayIcon = _trayIcon;
}
void UpdateDialog::start(QUrl url) {
    this->_manager = new QNetworkAccessManager(this);
    this->_request.setUrl(url);
    this->_reply = this->_manager->get(this->_request);

    connect(this->_reply, SIGNAL(downloadProgress(qint64,qint64)), this, SLOT(downloadProgress(qint64,qint64)));
    connect(this->_reply, SIGNAL(finished()), this, SLOT(downloadFinished()));
    connect(this->_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(downloadError(QNetworkReply::NetworkError)));
}

void UpdateDialog::downloadProgress(qint64 bytesReceived, qint64 bytesTotal) {
    qreal speed = ((bytesReceived * 1.0) / bytesTotal);
    this->ui->progressBar->setValue(speed*100);
}

void UpdateDialog::downloadError(QNetworkReply::NetworkError err) {
    if(APP_DEBUG) {
        qDebug() << "Error(" +QString(err)+ "): " << QString(_reply->errorString());
    }
    popup("Unable to connect...", QString(_reply->errorString()));
    this->_manager->deleteLater();
    this->_reply->deleteLater();
}

void UpdateDialog::downloadFinished() {
    if(this->_reply->error() == QNetworkReply::NoError) {
        if(APP_DEBUG) {
            qDebug() << "Download Finished!";
        }
        QFile *file = new QFile(APP_TEMP_FILENAME);
        if(file->open(QFile::WriteOnly)) {
            file->write(_reply->readAll());
            file->flush();
            file->close();
            delete file;
            this->_manager->deleteLater();
            this->_reply->deleteLater();
            QDesktopServices::openUrl(QUrl::fromLocalFile(APP_TEMP_FILENAME));
            if(this->_trayIcon) {
                delete this->_trayIcon;
            }
            exit(0);
        }
    } else {
        if(APP_DEBUG) {
            qDebug() << "Download Error!";
        }
    }
}
UpdateDialog::~UpdateDialog()
{
    delete ui;
}
