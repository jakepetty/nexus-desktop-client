#ifndef HOTKEYWORKER
#define HOTKEYWORKER

#include <QDebug>
#include <QApplication>
#include <QThread>
#include <QString>
#include "windows.h"
class HotKeyWorker : public QThread
{
    Q_OBJECT
public:

    explicit HotKeyWorker( QString hotkey, QObject * parent = 0 ) : QThread( parent ) {
        this->hotkey = hotkey;
        QApplication::processEvents();
    }
    void run() {
        // Do Stuff
        UINT modifier = 0x0;
        UINT vkey = 0x0;

        if ( this->hotkey.contains( "CTRL" ) && this->hotkey.contains( "ALT" )) {
            modifier = MOD_CONTROL | MOD_ALT;
        } else if ( this->hotkey.contains( "CTRL" )) {
            modifier = MOD_CONTROL;
        } else if ( this->hotkey.contains( "ALT" )) {
            modifier = MOD_ALT;
        }
        QString key = this->hotkey.replace( "ALT", NULL ).replace( "CTRL", NULL ).replace( "+", NULL );
        if ( key == "F1" ) {
            vkey = VK_F1;
        } else if ( key == "F2" ) {
            vkey = VK_F2;
        } else if ( key == "F3" ) {
            vkey = VK_F3;
        } else if ( key == "F4" ) {
            vkey = VK_F4;
        } else if ( key == "F5" ) {
            vkey = VK_F5;
        } else if ( key == "F6" ) {
            vkey = VK_F6;
        } else if ( key == "F7" ) {
            vkey = VK_F7;
        } else if ( key == "F8" ) {
            vkey = VK_F8;
        } else if ( key == "F9" ) {
            vkey = VK_F9;
        } else if ( key == "F10" ) {
            vkey = VK_F10;
        } else if ( key == "F11" ) {
            vkey = VK_F11;
        } else if ( key == "F12" ) {
            vkey = VK_F12;
        } else {
            vkey = VK_F11;
        }
        RegisterHotKey( NULL, 1, modifier | 0x4000, vkey );
        MSG msg;
        while ( GetMessage( &msg, NULL, 0, 0 )) {
            TranslateMessage( &msg );
            DispatchMessage( &msg );
            if ( msg.message == WM_HOTKEY ) {
                if ( msg.wParam == 1 ) {
                    emit onKeyPressed();
                }
            }
        }
    }
signals:
    void onKeyPressed();
private:
    QString hotkey;
};
#endif // HOTKEYWORKER
