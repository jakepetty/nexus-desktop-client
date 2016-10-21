#include "fleetdialog.h"
#include "ui_fleetdialog.h"

FleetDialog::FleetDialog( QWidget * parent ) :
    QDialog( parent ),
    ui( new Ui::FleetDialog )
{
    ui->setupUi( this );
}
void FleetDialog::init() {
    if ( !is_initialized ) {
        ui->label->setText( QString( ui->label->text()).arg( MAX_FILE_AGE / (60 * 60 * 24)));
        ui->progressBar->hide();
        ui->label_2->hide();
        ui->label_3->hide();
        is_initialized = true;
    }
}

void FleetDialog::setupList() {
    ui->listWidget->clear();
    QDir dir( Acc->User["install_directory"].toString());
    QRegExp rx( "(.*)_([0-9]+)-([0-9]+).Csv" );
    QStringList filters;
    filters << "*.Csv";
    foreach( QString filename, dir.entryList( filters, QDir::Files )) {
        if ( rx.indexIn( filename ) != -1 ) {
            QString oFileName = filename;
            QString date = filename.split( '_' ).last().remove( ".Csv" );
            QString name = filename.remove( "_" + date + ".Csv" ).replace( '_', ' ' );
            date.replace( '-', ' ' );
            date.insert( 13, ':' );
            date.insert( 11, ':' );
            date.insert( 6, '-' );
            date.insert( 4, '-' );
            int timestamp = QDateTime::fromString( date, "yyyy-MM-dd HH:mm:ss" ).toUTC().toTime_t();
            bool outdated = (NOW( true ).toInt() - timestamp) > MAX_FILE_AGE;
            if ( outdated ) {
                QFile::remove( dir.absoluteFilePath( oFileName ));
            } else {
                QListWidgetItem * item = new QListWidgetItem( name + " - " + timeAgoInWords((NOW( true ).toInt() - timestamp)) + " ago" );
                QVariant data;
                data.setValue( oFileName );
                item->setData( Qt::UserRole, data );
                ui->listWidget->addItem( item );
            }
        }
    }
}

FleetDialog::~FleetDialog()
{
    delete ui;
}

void FleetDialog::on_upload_btn_released()
{
    if ( Web->isAvailable( URL_BASE )) {

        // Check if app is current version
        if ( Web->isLatestVersion( URL_VERSION, version())) {
            ui->upload_btn->setDisabled( true );
            ui->delete_btn->setDisabled( true );
            this->_request.setUrl( QUrl( URL_UPLOAD_FLEET ));
            this->_reply = this->_manager.get( this->_request );

            QString bound = "STO";

            QByteArray data( QString( "--" + bound + "\r\n" ).toUtf8());
            data.append( "Content-Disposition: form-data; name=\"_method\"\r\n\r\n" );
            data.append( "POST\r\n" );
            data.append( "--" + bound + "\r\n" );

            int i = 0;
            foreach( QListWidgetItem * item, ui->listWidget->selectedItems()) {
                QString filename = item->data( Qt::UserRole ).value<QString>();
                QFile file( Acc->User["install_directory"].toString() + DS + filename );
                if ( file.exists()) {
                    if ( file.open( QFile::ReadOnly )) {

                        //QString date = filename.split('_').last().remove(".Csv");
                        //QString name = filename.remove("_" + date + ".Csv").replace('_', ' ');

                        QString oDate = filename.split( '_' ).last().remove( ".Csv" );

                        filename = filename.replace( oDate, QDateTime::fromString( oDate, "yyyyMMdd-hhmmss" ).toUTC().toString( "yyyyMMdd-hhmmss" ));

                        data.append( QString( "Content-Disposition: form-data; name=\"data[Fleet][%1][file]\"; filename=\"%2\"\r\n" ).arg( i ).arg( filename ));
                        data.append( "Content-Type: application/octet-stream\r\n\r\n" );
                        data.append( compress( file.readAll()));
                        data.append( "\r\n" );
                        data.append( "--" + bound + "\r\n" );
                        file.close();
                        i++;
                    }
                }
            }

            this->_request.setRawHeader( QString( "Content-Type" ).toUtf8(), QString( "multipart/form-data; boundary=" + bound ).toUtf8());
            this->_request.setRawHeader( QString( "Content-Length" ).toUtf8(), QString::number( data.length()).toUtf8());
            this->_reply = this->_manager.post( this->_request, data );
            ui->progressBar->show();
            serviceCheck( "Uploading Roster", "Started" );
            connect( this->_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(uploadError(QNetworkReply::NetworkError)));
            connect( this->_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(uploadProgress(qint64,qint64)));
            connect( this->_reply, SIGNAL(finished()), this, SLOT(uploadFinished()));
        } else {
            popup( "Update Required", "Please right click our tray icon and click Check for Updates to update then try again." );
        }
    } else {
        popup( "Server Unavailable...", "We're sorry but it appears our server is currently unavailable at this time. Please try again in a few minutes." );
    }
}

void FleetDialog::on_delete_btn_released()
{
    if ( confirm( "Are you sure you want to delete the selected fleet rosters?" )) {
        foreach( QListWidgetItem * item, ui->listWidget->selectedItems()) {
            QString filename = item->data( Qt::UserRole ).value<QString>();
            item->setHidden( true );
            if ( !QFile::remove( Acc->User["install_directory"].toString() + DS + filename )) {
                popup( "Error...", QString( "Unable to delete %0" ).arg( filename ));
            }
        }
        ui->listWidget->clearSelection();
    }

}

void FleetDialog::on_listWidget_itemSelectionChanged()
{
    bool enabled = ui->listWidget->selectedItems().count() > 0;
    ui->delete_btn->setDisabled( !enabled );
    ui->upload_btn->setDisabled( !enabled );
}

void FleetDialog::uploadProgress( qint64 bytesSent, qint64 bytesTotal ) {

    qreal speed = ((bytesSent * 1.0) / bytesTotal);
    if ( speed == 1 ) {
        this->setWindowTitle( "Roster Manager" );
        serviceCheck( "Uploading Roster", "Done" );
        ui->progressBar->hide();
        QMovie * movie = new QMovie( ":images/spinner.gif" );
        ui->label_2->setMovie( movie );
        ui->label_2->show();
        ui->label_3->show();
        movie->start();
    } else if ( bytesTotal > 0 ) {
        serviceCheck( "Uploading Roster", QString::number( speed * 100, 'f', 2 ) + "%" );
        this->setWindowTitle( QString( "Roster Manager - Uploading %1%" ).arg( QString::number( speed * 100, 'f', 2 )));
    }
    this->ui->progressBar->setValue( speed * 100 );
}

void FleetDialog::uploadFinished()
{
    if ( this->_reply->error() == QNetworkReply::NoError ) {
        QByteArray callback = this->_reply->readAll();
        QJsonDocument doc( QJsonDocument::fromJson( callback ));
        QJsonObject json = doc.object();
        if ( json["code"].toInt() == 200 ) {
            foreach( QJsonValue name, json["items"].toArray()) {
                QListWidgetItem * item = this->getItemByFilename( name.toString());
                QString filename = item->data( Qt::UserRole ).value<QString>();
                QFile file( Acc->User["install_directory"].toString() + DS + filename );
                if ( !file.remove()) {
                    popup( file.errorString(), QString( "Unabled to delete <b>%1</b>" ).arg( filename ));
                } else {
                    delete item;
                }
                this->uploadCount++;
            }
            if ( this->isWizard ) {
                this->accept();
            } else if ( Acc->DesktopSetting["auto_open_fleet"].toBool()) {
                foreach( QJsonValue name, json["urls"].toArray()) {
                    QDesktopServices::openUrl( QUrl( QString( URL_FLEET ).arg( Acc->User["identifier"].toString()).arg( name.toString())));
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
        if ( APP_DEBUG ) {
            qDebug() << callback;
        }
    } else {
        if ( APP_DEBUG ) {
            qDebug() << "Upload Finished: " << this->_reply->errorString();
        }
    }
    ui->listWidget->clearSelection();
    this->on_listWidget_itemSelectionChanged();
    ui->progressBar->hide();
    ui->label_3->hide();
    ui->label_2->hide();
}
QListWidgetItem * FleetDialog::getItemByFilename( QString name ) {
    name = QDateTime::fromString( name, "yyyyMMdd-hhmmss" ).toLocalTime().toString( "yyyyMMdd-hhmmss" );
    foreach( QListWidgetItem * item, ui->listWidget->selectedItems()) {
        QString filename = item->data( Qt::UserRole ).value<QString>();
        if ( filename.contains( name )) {
            return item;
        }
    }
    return NULL;
}

void FleetDialog::on_pushButton_released()
{
    this->setupList();
}

void FleetDialog::on_openBtn_released()
{
    QDesktopServices::openUrl( QUrl::fromLocalFile( Acc->User["install_directory"].toString()));
}
