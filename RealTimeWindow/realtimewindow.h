#ifndef REALTIMEWINDOW_H
#define REALTIMEWINDOW_H
#include "common.h"
#include "BackgroundWorkers/realtimeworker.h"
#include <QMainWindow>
#include <QThread>
#include <QMoveEvent>
#include <QDesktopWidget>
#include <QTimer>
#include <QLocale>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QFileSystemWatcher>
#include <QPropertyAnimation>
#include <QDesktopServices>

class MyCustomTableWidgetItem : public QTableWidgetItem {
public:
    bool operator<( const QTableWidgetItem & other ) const
    {
        return text().replace( ",", NULL ).toDouble() < other.text().replace( ",", NULL ).toDouble();
    }
};

namespace Ui {
    class RealTimeWindow;
}

class RealTimeWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit RealTimeWindow( QWidget * parent = 0 );
    ~RealTimeWindow();
    void setupUploadBtn( QObject * );
    void init();
    void createTable();
    void buildColumns();
    void buildTable();
    void resetColumns();
    void resizeTable();
    void resetTable();
    void monitor();
    void setupConfigButton( QObject * );
public slots:
    void checkLog();
    void onCellClick( int, int );
    void onFileChanged( QString );
    void onDirectoryChanged( QString );
    void onProgressChanged( int, QMap<QString, Player>, QMap<QString, QMap<QString, QList<QString> > >, QMap<QString, Weapon> );
    void enterEvent( QEvent * );
    void leaveEvent( QEvent * );
    void closeEvent( QCloseEvent * );
private slots:
    void on_resetBtn_released();

    void on_closeBtn_released();

private:
    bool eventFilter( QObject *, QEvent * );
    QPoint dragPos;
    int last_line;
    bool is_processing;
    bool is_initialized = false;
    bool is_waiting = false;
    QTimer * _logCheck;
    QStringList handles;
    QMap<QString, bool> members;
    QStringList column_order;
    QMap<QString, int> column_ids;
    QMap<QString, Player> players;
    QMap<QString, QMap<QString, QList<QString> > > timeCache;
    QMap<QString, Weapon> weapons;
    QFileSystemWatcher * fileMonitor;
    RealTimeWorker * workerThread;
    QThread * thread;
    Ui::RealTimeWindow * ui;
};

#endif // REALTIMEWINDOW_H
