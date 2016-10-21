#include "notificationdialog.h"
#include "ui_notificationdialog.h"
#include <QSystemTrayIcon>

NotificationDialog::NotificationDialog( QWidget * parent ) :
    QDialog( parent ),

    ui( new Ui::NotificationDialog )
{
    setWindowFlags(
        QWidget::windowFlags() |
        Qt::SplashScreen |
        Qt::WindowStaysOnTopHint );
    ui->setupUi( this );
}

NotificationDialog::~NotificationDialog()
{
    delete ui;
}

void NotificationDialog::fadeTo( double _stop, int _speed )
{
    fadeAnimation = new QPropertyAnimation( this, "windowOpacity" );
    fadeAnimation->setDuration( _speed );
    fadeAnimation->setStartValue( this->windowOpacity());
    fadeAnimation->setEndValue( _stop );
    fadeAnimation->start();
}

void NotificationDialog::mousePressEvent( QMouseEvent * e )
{
    if ( e->button() == 2 )
    {
        disappear();
    }
    return QWidget::mousePressEvent( e );
}
bool NotificationDialog::event( QEvent * e )
{
    if ( e->type() == QEvent::Enter )
    {
        dimTimer->stop();
        hideTimer->stop();
        appear( 100 );
        dimTimer->stop();
        hideTimer->stop();
    }
    if ( e->type() == QEvent::Leave )
    {
        startHider();
        startDimmer();
    }

    return QWidget::event( e );
}
void NotificationDialog::startDimmer()
{
    dimTimer = new QTimer();
    dimTimer->setInterval( totalDuration / 2 );
    connect( dimTimer, SIGNAL(timeout()), this, SLOT(onDimTimeout()));
    dimTimer->start();
}
void NotificationDialog::startHider()
{
    hideTimer = new QTimer();
    hideTimer->setInterval( totalDuration );
    connect( hideTimer, SIGNAL(timeout()), this, SLOT(onHideTimeout()));
    hideTimer->start();
}

void NotificationDialog::appear( int _speed )
{
    fadeTo( 1, _speed );
    QRect screen = QApplication::desktop()->availableGeometry();
    movementAnimation = new QPropertyAnimation( this, "pos" );
    movementAnimation->setDuration( 100 );
    movementAnimation->setStartValue( this->pos());
    movementAnimation->setEndValue( QPoint( this->pos().x(), screen.height() - this->height() - 20 ));
    movementAnimation->start();

    startHider();
    startDimmer();
}
void NotificationDialog::disappear()
{
    QRect screen = QApplication::desktop()->geometry();

    movementAnimation = new QPropertyAnimation( this, "pos" );
    movementAnimation->setDuration( 500 );
    movementAnimation->setStartValue( this->pos());
    movementAnimation->setEndValue( QPoint( this->pos().x(), screen.height()));

    connect( movementAnimation, SIGNAL(finished()), this, SLOT(onFinish()));
    movementAnimation->start();
    fadeTo( 0, 250 );
}
void NotificationDialog::onHideTimeout()
{
    hideTimer->stop();
    disappear();
}
void NotificationDialog::onFinish()
{
    reject();
}

void NotificationDialog::onDimTimeout()
{
    dimTimer->stop();
    fadeTo( 0.75 );
}

void NotificationDialog::setTitle( QString _t )
{
    int len = _t.length();
    _t = _t.mid( 0, 33 );
    if ( len > 33 ) {
        _t += "...";
    }
    this->ui->title->setText( _t );
}

void NotificationDialog::setBody( QString _t )
{
    this->ui->body->setText( "<div style=\"line-height:17px\">" + _t + "</div>" );
    this->ui->body->setWordWrap( true );
    QRect screen = QApplication::desktop()->availableGeometry();

    this->setGeometry( screen.width() - this->width() - 20, screen.height(), this->width(), this->height());

    this->setWindowOpacity( 0 );
    this->adjustSize();
    appear();
}

void NotificationDialog::setAuthor( QString _t )
{
    this->ui->author->setText( "By: " + _t );
}
void NotificationDialog::setURL( QString _t, QString _btn )
{
    this->_url = _t;
    this->ui->link->setText( _btn );
}

void NotificationDialog::on_link_released()
{
    QDesktopServices::openUrl( QUrl( this->_url ));
    accept();
}
