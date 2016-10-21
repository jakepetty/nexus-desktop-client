#ifndef SCREENSHOTWORKER
#define SCREENSHOTWORKER

#include <QDebug>
#include <QThread>
#include <QString>
#include <QStringList>
#include <QList>
#include <QDir>
#include <QListWidgetItem>
class ScreenshotWorker : public QThread
{
    Q_OBJECT
public:

    explicit ScreenshotWorker( QString path, QObject * parent = 0 ) : QThread( parent ) {

        qRegisterMetaType <QList<QListWidgetItem *> > ( "QList<QListWidgetItem *>" );
        this->path = path;
    }
    void run() {
        QList<QListWidgetItem *> items;
        QDir dir( this->path );

        // Limit results to JPG files only
        QStringList filters;
        filters << "*.jpg";

        // Find matching files
        QStringList files = dir.entryList( filters, QDir::Files );

        // Process files
        foreach( QString filename, files ) {
            QRegExp rx( "screenshot_([0-9]+)-([0-9]+)-([0-9]+)-([0-9]+)-([0-9]+)-([0-9]+).jpg" );
            if ( rx.indexIn( filename ) != -1 ) {
                // Create a new item
                QListWidgetItem * item = new QListWidgetItem( this->getImage( filename, 96, 98 ), NULL );

                // Add filename data to item
                QVariant data;
                data.setValue( filename );
                item->setData( Qt::UserRole, data );

                // Add item to the list of items
                items.append( item );
            }
        }
        emit onComplete( items, files.count());
    }
signals:
    void onComplete( QList<QListWidgetItem *>, int );
private:
    QString path;

    QPixmap getImage( QString path, int w = 400, int h = 200 ) {
        QString thumb_dir = this->path + "/thumbs/";
        QFile file( thumb_dir + path );
        QPixmap pixmap;
        if ( file.exists()) {
            pixmap.load( thumb_dir + path );
        } else {
            QImage img( this->path + "/" + path );
            QDir d( thumb_dir );
            if ( !d.exists()) {
                d.mkdir( thumb_dir );
            }
            pixmap = pixmap.fromImage( img.scaled( w, h, Qt::KeepAspectRatio, Qt::SmoothTransformation ));
            QFile file( thumb_dir + path );
            file.open( QIODevice::WriteOnly );
            pixmap.save( &file, "jpeg", 100 );
            file.close();
        }
        return pixmap;
    }
};
#endif // SCREENSHOTWORKER
