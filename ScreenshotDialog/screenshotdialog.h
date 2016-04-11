#ifndef SCREENSHOTDIALOG_H
#define SCREENSHOTDIALOG_H

#include "common.h"
#include "BackgroundWorkers/screenshotworker.h"
#include <QMovie>
#include <QDialog>
#include <QCloseEvent>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QDesktopServices>

namespace Ui {
class ScreenshotDialog;
}

class ScreenshotDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ScreenshotDialog(QWidget *parent = 0);
    void init();
    void setupList();
    QPixmap getImage(QString path, int w = 400, int h = 200) {
        QPixmap pix(path);
        return pix.scaled(w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    QString getCreatedTime(QString filename) {
        QString date = filename.split('_').last().remove(".jpg");
        date.replace(10, 1, ' ');
        date.replace(13, 1, ':');
        date.replace(16, 1, ':');
        return QDateTime::fromString(date, "yyyy-MM-dd HH:mm:ss").toUTC().toString("yyyy-MM-dd HH:mm:ss");
    }

    ~ScreenshotDialog();
    void closeEvent (QCloseEvent *e);
    void removeItems(QJsonObject);
    void backupItems(QJsonObject);
    void resetPreview();
    void hideCaption();
    void showCaption();
    void setMultiple();

private slots:

    void onComplete(QList<QListWidgetItem *>, int);

    void on_select_all_released();

    void on_delete_selected_released();

    void on_upload_selected_released();

    void uploadProgress(qint64 bytesSent,qint64 bytesTotal);

    void uploadError(QNetworkReply::NetworkError error) {
        if(error != QNetworkReply::NoError) {
            if(APP_DEBUG) {
                qDebug() << "Upload Error: " << this->_reply->errorString();
            }
        }
    }

    void uploadFinished();

    void on_listWidget_itemSelectionChanged();
    void on_openFolder_released();

    void on_onlineGallery_released();

private:
    bool is_initialized = false;
    Ui::ScreenshotDialog * ui;
    QNetworkReply *_reply;
    QNetworkRequest _request;
    QNetworkAccessManager _manager;
    int count;
    QListWidgetItem* getItemByFilename(QString);
};

#endif // SCREENSHOTDIALOG_H
