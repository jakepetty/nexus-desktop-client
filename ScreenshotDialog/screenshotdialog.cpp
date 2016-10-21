#include "screenshotdialog.h"
#include "ui_screenshotdialog.h"

ScreenshotDialog::ScreenshotDialog( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::ScreenshotDialog )
{
    ui->setupUi( this );

}
void ScreenshotDialog::init() {
    if ( !is_initialized ) {
        ui->label_2->hide();
        QMovie * movie = new QMovie( ":/images/spinner.gif" );
        ui->label_3->setMovie( movie );
        ui->preview->setText( "Loading..." );
        movie->start();
        ui->progressBar->hide();
        ui->frame->hide();
        hideCaption();
        is_initialized = true;
        setMaximumSize( minimumSize());
        resize( minimumSize());
    }
}

ScreenshotDialog::~ScreenshotDialog()
{
    delete ui;
}
void ScreenshotDialog::closeEvent( QCloseEvent * e ) {
    for ( int i = 0; i < this->ui->listWidget->count(); i++ ) {
        delete this->ui->listWidget->item( i );
    }
    QWidget::closeEvent( e );
}
void ScreenshotDialog::setupList() {
    this->ui->listWidget->clear();

    ScreenshotWorker * workerThread = new ScreenshotWorker( Acc->User["install_directory"].toString() + SCREENSHOTS );
    connect( workerThread, SIGNAL(onComplete(QList<QListWidgetItem *>,int)), SLOT(onComplete(QList<QListWidgetItem *>,int)));
    connect( workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
    workerThread->start();

}

void ScreenshotDialog::on_select_all_released()
{
    ui->listWidget->selectAll();
}

void ScreenshotDialog::on_delete_selected_released()
{
    QList<QListWidgetItem *> items = ui->listWidget->selectedItems();
    if ( confirm( QString( "Are you sure you want to delete %1 screenshot(s)?" ).arg( items.count()))) {
        foreach( QListWidgetItem * item, items ) {
            QString filename = item->data( Qt::UserRole ).value<QString>();
            QFile file( Acc->User["install_directory"].toString() + SCREENSHOTS + filename );
            if ( file.exists()) {
                if ( !file.remove()) {
                    popup( file.errorString(), QString( "Unabled to delete <b>%1</b>" ).arg( file.fileName()));
                } else {
                    delete item;
                }
            }
        }
        ui->listWidget->clearSelection();
        this->on_listWidget_itemSelectionChanged();
    }
}

void ScreenshotDialog::onComplete( QList<QListWidgetItem *> items, int count )
{
    foreach( QListWidgetItem * item, items ) {
        ui->listWidget->addItem( item );
        ui->listWidget->setIconSize( QSize( 96, 98 ));
    }
    if ( count >= 1 ) {
        ui->select_all->setDisabled( false );
    }
    ui->stats->setText( QString( "Selected Screenshots: <b>0/%2</b>" ).arg( count ));
    ui->frame->show();
    resetPreview();
    ui->label_3->hide();
}

void ScreenshotDialog::on_upload_selected_released()
{
    if ( Web->isAvailable( URL_BASE )) {

        // Check if app is current version
        if ( Web->isLatestVersion( URL_VERSION, version())) {
            this->_request.setUrl( QUrl( URL_UPLOAD_SCREENSHOT ));
            this->_reply = this->_manager.get( this->_request );

            QString bound = "STO";

            QByteArray data( QString( "--" + bound + "\r\n" ).toUtf8());
            data.append( "Content-Disposition: form-data; name=\"_method\"\r\n\r\n" );
            data.append( "POST\r\n" );
            data.append( "--" + bound + "\r\n" );

            int i = 0;
            foreach( QListWidgetItem * item, ui->listWidget->selectedItems()) {
                if ( i <= MAX_FILE_UPLOADS ) {
                    QString filename = item->data( Qt::UserRole ).value<QString>();
                    QFile file( Acc->User["install_directory"].toString() + SCREENSHOTS + filename );
                    if ( file.exists()) {
                        if ( file.open( QFile::ReadOnly )) {
                            QString name = filename.split( '/' ).last();
                            data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][user_id]\"\r\n\r\n" ).arg( i ));
                            data.append( QString::number( Acc->User["id"].toInt()) + "\r\n" );
                            data.append( "--" + bound + "\r\n" );
                            data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][filename]\"\r\n\r\n" ).arg( i ));
                            data.append( name + "\r\n" );
                            data.append( "--" + bound + "\r\n" );
                            data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][taken]\"\r\n\r\n" ).arg( i ));
                            data.append( this->getCreatedTime( name ) + "\r\n" );
                            data.append( "--" + bound + "\r\n" );
                            if ( Acc->DesktopSetting["screenshot_is_private"].toBool()) {
                                data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][is_private]\"\r\n\r\n" ).arg( i ));
                                data.append( "1\r\n" );
                                data.append( "--" + bound + "\r\n" );
                            }
                            if ( Acc->DesktopSetting["screenshot_is_friends_only"].toBool()) {
                                data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][is_friends_only]\"\r\n\r\n" ).arg( i ));
                                data.append( "1\r\n" );
                                data.append( "--" + bound + "\r\n" );
                            }
                            if ( Acc->DesktopSetting["screenshot_is_downloadable"].toBool()) {
                                data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][is_downloadable]\"\r\n\r\n" ).arg( i ));
                                data.append( "1\r\n" );
                                data.append( "--" + bound + "\r\n" );
                            }
                            if ( !this->ui->caption->text().isEmpty()) {
                                data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][caption]\"\r\n\r\n" ).arg( i ));
                                data.append( this->ui->caption->text() + "\r\n" );
                                data.append( "--" + bound + "\r\n" );
                            }
                            if ( Acc->DesktopSetting["screenshot_is_browsable"].toBool()) {
                                data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][is_browsable]\"\r\n\r\n" ).arg( i ));
                                data.append( "1\r\n" );
                                data.append( "--" + bound + "\r\n" );
                            }
                            data.append( QString( "Content-Disposition: form-data; name=\"data[Screenshot][%1][file]\"; filename=\"%2\"\r\n" ).arg( i ).arg( name ));
                            data.append( "Content-Type: image/jpeg\r\n\r\n" );
                            data.append( file.readAll());
                            data.append( "\r\n" );
                            data.append( "--" + bound + "\r\n" );
                            file.close();
                            i++;
                        }
                    }
                }
            }
            resetPreview();
            hideCaption();
            this->_request.setRawHeader( QString( "Content-Type" ).toUtf8(), QString( "multipart/form-data; boundary=" + bound ).toUtf8());
            this->_request.setRawHeader( QString( "Content-Length" ).toUtf8(), QString::number( data.length()).toUtf8());
            this->_reply = this->_manager.post( this->_request, data );
            connect( this->_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
            connect( this->_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgress(qint64,qint64)));
            connect( this->_reply, SIGNAL(finished()), this, SLOT(uploadFinished()));
            ui->delete_selected->setDisabled( true );
            ui->upload_selected->setDisabled( true );
            ui->select_all->setDisabled( true );
            ui->progressBar->show();
            serviceCheck( "Uploading Screenshot", "Started" );
        } else {
            popup( "Update Required", "Please right click our tray icon and click Check for Updates to update then try again." );
        }
    } else {
        popup( "Server Unavailable...", "We're sorry but it appears our server is currently unavailable at this time. Please try again in a few minutes." );
    }
}
QListWidgetItem * ScreenshotDialog::getItemByFilename( QString name ) {
    foreach( QListWidgetItem * item, ui->listWidget->selectedItems()) {
        QString filename = item->data( Qt::UserRole ).value<QString>();
        if ( filename.contains( name )) {
            return item;
        }
    }
    return NULL;
}

void ScreenshotDialog::uploadFinished()
{
    if ( this->_reply->error() == QNetworkReply::NoError ) {
        QByteArray callback = this->_reply->readAll();
        QJsonDocument doc( QJsonDocument::fromJson( callback ));
        QJsonObject json = doc.object();
        if ( json["code"].toInt() == 200 || json["code"].toInt() == 1062 ) {
            backupItems( json );
            if ( Acc->DesktopSetting["screenshot_auto_open"].toBool()) {
                foreach( QJsonValue seo_url, json["urls"].toArray()) {
                    QDesktopServices::openUrl( QUrl( QString( URL_SCREENSHOT ).arg( Acc->User["identifier"].toString()).arg( seo_url.toString())));
                }
            }
        } else {
            QString errors;
            foreach( QJsonValue items, json["errors"].toArray()) {
                foreach( QJsonValue error, items.toArray()) {
                    errors.append( error.toString() + "\n" );
                }
            }
            if ( !errors.isEmpty()) {
                popup( "Errors", errors );
            }
        }
    } else {
        if ( APP_DEBUG ) {
            qDebug() << "Upload Finished: " << this->_reply->errorString();
        }
    }
    ui->listWidget->clearSelection();
    this->on_listWidget_itemSelectionChanged();
    ui->label_2->hide();
    ui->label_3->hide();
    ui->select_all->setDisabled( false );
}

void ScreenshotDialog::removeItems( QJsonObject json ) {
    foreach( QJsonValue name, json["items"].toArray()) {
        QListWidgetItem * item = this->getItemByFilename( name.toString());
        QString filename = item->data( Qt::UserRole ).value<QString>();
        QFile file( Acc->User["install_directory"].toString() + SCREENSHOTS + filename );
        if ( file.exists()) {
            if ( !file.remove()) {
                if ( APP_DEBUG ) {
                    qDebug() << QString( "Unabled to delete <b>%1</b>" ).arg( file.fileName());
                }
            } else {
                QFile::remove( Acc->User["install_directory"].toString() + SCREENSHOTS + "thumbs" + DS + filename );
                delete item;
            }
        }
    }
}

void ScreenshotDialog::backupItems( QJsonObject json ) {
    QString backup = Acc->User["install_directory"].toString() + SCREENSHOTS + "Nexus Backups" + DS;
    QDir dir;
    if ( dir.exists( backup ) || dir.mkpath( backup )) {
        foreach( QJsonValue name, json["items"].toArray()) {
            QListWidgetItem * item = this->getItemByFilename( name.toString());
            QString filename = item->data( Qt::UserRole ).value<QString>();
            QFile file( Acc->User["install_directory"].toString() + SCREENSHOTS + filename );
            if ( file.exists()) {
                file.copy( backup + filename );
                if ( !file.remove()) {
                    if ( APP_DEBUG ) {
                        qDebug() << QString( "Unabled to delete <b>%1</b>" ).arg( file.fileName());
                    }
                } else {
                    QFile::remove( Acc->User["install_directory"].toString() + SCREENSHOTS + "thumbs" + DS + filename );
                    delete item;
                }
            }
        }
    }
}

void ScreenshotDialog::uploadProgress( qint64 bytesSent, qint64 bytesTotal )
{
    qreal speed = ((bytesSent * 1.0) / bytesTotal);
    if ( speed == 1 ) {
        this->setWindowTitle( "Screenshot Manager" );
        serviceCheck( "Uploading Screenshot", "Done" );

        ui->progressBar->hide();
        ui->label_2->show();
        ui->label_3->show();

    } else if ( bytesTotal > 0 ) {
        serviceCheck( "Uploading Screenshot", QString::number( speed * 100, 'f', 2 ) + "%" );
        this->setWindowTitle( QString( "Screenshot Manager - Uploading %1%" ).arg( QString::number( speed * 100, 'f', 2 )));
    }
    this->ui->progressBar->setValue( speed * 100 );
}
void ScreenshotDialog::on_listWidget_itemSelectionChanged()
{
    int selected = this->ui->listWidget->selectedItems().count();
    int total = this->ui->listWidget->count();
    if ( selected > 1 ) {
        setMultiple();
        hideCaption();
    } else if ( selected == 1 ) {
        showCaption();
        resetPreview();
        foreach( QListWidgetItem * item, this->ui->listWidget->selectedItems()) {
            QString filename = item->data( Qt::UserRole ).value<QString>();
            ui->preview->setPixmap( this->getImage( Acc->User["install_directory"].toString() + SCREENSHOTS + filename, ui->preview->width(), ui->preview->height()));
        }
    } else {
        hideCaption();
        resetPreview();
    }
    ui->upload_selected->setDisabled( selected < 1 );
    ui->delete_selected->setDisabled( selected < 1 );
    ui->stats->setText( QString( "Selected Screenshots: <b>%1/%2</b>" ).arg( selected ).arg( total ));
}

void ScreenshotDialog::resetPreview() {
    ui->preview->clear();
    ui->preview->setText( "Preview" );
}

void ScreenshotDialog::setMultiple() {
    ui->preview->clear();
    ui->preview->setPixmap( QPixmap( ":/images/multiple.png" ));
}


void ScreenshotDialog::hideCaption() {
    ui->caption->clear();
    ui->caption->hide();
}
void ScreenshotDialog::showCaption() {
    ui->caption->clear();
    ui->caption->show();
}

void ScreenshotDialog::on_openFolder_released()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( Acc->User["install_directory"].toString() + SCREENSHOTS ));
}

void ScreenshotDialog::on_onlineGallery_released()
{
    QDesktopServices::openUrl( QUrl( QString( URL_SCREENSHOT_GALLERY ).arg( Acc->User["username"].toString())));
}
