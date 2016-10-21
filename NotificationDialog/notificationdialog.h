#ifndef NOTIFICATIONDIALOG_H
#define NOTIFICATIONDIALOG_H

#include <QDialog>
#include <QTimer>
#include <QDesktopWidget>
#include <QPropertyAnimation>
#include <QDebug>
#include <QDesktopServices>
#include <QUrl>
#include <QMouseEvent>

namespace Ui {
    class NotificationDialog;
}

class NotificationDialog : public QDialog
{
    Q_OBJECT

protected:
    bool event( QEvent * e ) override;
    void mousePressEvent( QMouseEvent * e ) override;
public:
    void setTitle( QString );
    void setBody( QString );
    void setAuthor( QString );
    void setURL( QString _t, QString _btn = "Read More" );
    explicit NotificationDialog( QWidget * parent = 0 );
    ~NotificationDialog();

private slots:
    void onHideTimeout();
    void onDimTimeout();
    void onFinish();
    void on_link_released();

private:
    void fadeTo( double _stop, int _speed = 1000 );
    void appear( int _speed = 1000 );
    void disappear();
    void startHider();
    void startDimmer();
    int totalDuration = 15000;
    QString _url;
    QTimer * hideTimer;
    QTimer * dimTimer;
    QPropertyAnimation * fadeAnimation;
    QPropertyAnimation * movementAnimation;
    Ui::NotificationDialog * ui;
};

#endif // NOTIFICATIONDIALOG_H
