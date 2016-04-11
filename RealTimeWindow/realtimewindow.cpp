#include "realtimewindow.h"
#include "ui_realtimewindow.h"
#include <QItemDelegate>
class MyItemDelegate: public QItemDelegate
{
public:
    MyItemDelegate(QObject* pParent = 0) : QItemDelegate(pParent)
    {
    }

    void paint(QPainter* pPainter, const QStyleOptionViewItem& rOption, const QModelIndex& rIndex) const
    {
        QStyleOptionViewItem ViewOption(rOption);

        QColor ItemForegroundColor = rIndex.data(Qt::ForegroundRole).value<QColor>();
        if (ItemForegroundColor.isValid())
        {
            if (ItemForegroundColor != rOption.palette.color(QPalette::WindowText))
            {
                ViewOption.palette.setColor(QPalette::HighlightedText, ItemForegroundColor);
            }
        }
        QItemDelegate::paint(pPainter, ViewOption, rIndex);
    }
};
void RealTimeWindow::createTable() {
    this->ui->playerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->ui->playerTable->verticalHeader()->setSectionResizeMode(QHeaderView::Fixed);
    this->ui->playerTable->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    this->buildColumns();
    this->buildTable();
}
void RealTimeWindow::buildColumns() {
    int i = 0;
    foreach(QString field, column_order) {
        if(Acc->DesktopSetting["show_" + field].toBool()) {
            column_ids[field] = i;
            this->ui->playerTable->setColumnCount(i + 1);
            if(field.contains("dps")) {
                field = "DPS";
            }
            QTableWidgetItem *column = new QTableWidgetItem(ucfirst(field.replace("_", " ")));
            column->setTextAlignment(Qt::AlignLeft);
            this->ui->playerTable->setHorizontalHeaderItem(i, column);
            i++;
        }
    }
}
RealTimeWindow::RealTimeWindow(QWidget * parent) :
    QMainWindow(parent, Qt::Dialog | Qt::WindowStaysOnTopHint),
    ui(new Ui::RealTimeWindow)
{
    ui->setupUi(this);
    this->fileMonitor = new QFileSystemWatcher(this);
    this->_logCheck = new QTimer(this);
    connect(this->fileMonitor, SIGNAL(fileChanged(QString)), this, SLOT(onFileChanged(QString)));
    connect(this->fileMonitor, SIGNAL(directoryChanged(QString)), this, SLOT(onDirectoryChanged(QString)));
    connect(this->_logCheck, SIGNAL(timeout()), this, SLOT(checkLog()));
    connect(this->ui->playerTable, SIGNAL(cellClicked(int, int)), this, SLOT(onCellClick(int, int)));
}
// Functions
void RealTimeWindow::init() {
    if(!is_initialized) {
        qApp->installEventFilter(this);
        qreal percentage = ((1.0/100.0) * Acc->DesktopSetting["opacity"].toInt());
        setWindowOpacity(percentage);
        this->last_line = 0;
        column_order << "name"
                     << "handle"
                     << "dps"
                     << "dmg_out"
                     << "dmg_in"
                     << "hardest_outgoing_hit"
                     << "hardest_incoming_hit"
                     << "shots"
                     << "misses"
                     << "crits"
                     << "kills"
                     << "deaths"
                     << "accuracy"
                     << "duration";
        this->createTable();
        is_processing = false;
        is_initialized = true;
    }
    fileMonitor->addPath(Acc->User["install_directory"].toString() + LOGS);
    QStringList logs = STO->combatlogs(Acc->User["install_directory"].toString() + LOGS);
    if(!logs.isEmpty()) {
        fileMonitor->addPaths(logs);
    }
    this->_logCheck->start(1000);
    this->monitor();
}
void RealTimeWindow::buildTable() {

    int x = 0;
    this->ui->playerTable->setRowCount(this->players.count());
    foreach(Player player, this->players) {
        if(!members.keys().contains(player.handle)) {
            QUrlQuery post;
            post.addQueryItem("data[Handle][name]", player.handle);
            QJsonObject data = Web->postJSON(URL_FIND_HANDLE, post);
            members.insert(player.handle, data["code"].toInt() == 200);
        }
        if(!player.character.isEmpty()) {
            foreach(int column, column_ids) {
                QString value = player.getValue(column_ids.key(column));
                MyCustomTableWidgetItem *item = new MyCustomTableWidgetItem;
                if(column_ids.key(column) == "duration") {
                    int time = value.toInt();
                    int hours = floor(time / 3600);
                    int minutes = floor(time / 60);
                    int seconds = time - minutes * 60;
                    QString data;
                    if(hours > 0) {
                        data = QString("%1h %2m %3s").arg(hours).arg(minutes).arg(seconds);
                    } else if(minutes > 0) {
                        data = QString("%1m %2s").arg(minutes).arg(seconds);
                    } else {
                        data = QString("%1s").arg(seconds);
                    }
                    item->setData(Qt::DisplayRole, data);
                } else if(value.contains(".")) {
                    bool ok;
                    double val = value.toDouble(&ok);
                    if(ok) {
                        item->setData(Qt::DisplayRole, QLocale::system().toString(val, ',', 2));
                    } else {
                        item->setData(Qt::DisplayRole, value);
                    }
                } else {
                    bool ok;
                    double val = value.toDouble(&ok);
                    if(ok) {
                        item->setData(Qt::DisplayRole, QLocale::system().toString(val, ',', 0));
                    } else {
                        item->setData(Qt::DisplayRole, value);
                    }
                }
                item->setForeground(QColor(204,211,229));
                this->ui->playerTable->setItem(x, column, item);
                if(members.value(player.handle)) {
                    if(Acc->exists(player.handle)) {
                        this->ui->playerTable->item(x, column)->setForeground(QBrush(QColor(231,245,0)));
                        this->ui->playerTable->item(x, column)->setFont(QFont("Arial", 11, 600));
                    } else {
                        this->ui->playerTable->item(x, column)->setForeground(QBrush(QColor(255,255,255)));
                        this->ui->playerTable->item(x, column)->setFont(QFont("Arial", 9, 600));
                    }
                }
            }
            x++;
        }
    }
    this->ui->playerTable->setItemDelegate(new MyItemDelegate(this));
    resizeTable();

    // TODO: Figure out how to detect which sortItem id was clicked and keep it
    this->ui->playerTable->sortItems(column_ids["dps"], Qt::DescendingOrder);
    is_processing = false;
    is_waiting = false;
}
void RealTimeWindow::resetColumns() {
    if(is_initialized) {
        this->buildColumns();
        this->buildTable();
        this->resizeTable();
    }
}
void RealTimeWindow::resizeTable() {

    int width = 17;
    if(this->ui->playerTable->rowCount() > 10) {
        width = 28;
    }
    this->ui->playerTable->resizeRowsToContents();
    this->ui->playerTable->resizeColumnsToContents();

    // Calculates width of the table
    for(int x = 0; x < this->ui->playerTable->columnCount(); x++) {
        width += this->ui->playerTable->columnWidth(x);
    }

    // Sets max row display to 10 and min to 1
    int height = (this->ui->playerTable->rowHeight(0) * ((this->ui->playerTable->rowCount() <= 10 ? this->ui->playerTable->rowCount() : 10)+1));

    this->ui->playerTable->resize(width, height);
    resize(width, height+25);
    raise();
}
void RealTimeWindow::monitor() {
    if(!is_processing) {
        is_processing = true;
        workerThread = new RealTimeWorker(STO->combatlogs(Acc->User["install_directory"].toString() + LOGS), this->last_line, this->players, this->timeCache, this->weapons);
        // Connect our signal and slot
        connect(workerThread, SIGNAL(progressChanged(int, QMap<QString, Player>, QMap<QString, QMap<QString, QList<QString> > >, QMap<QString, Weapon>)), SLOT(onProgressChanged(int, QMap<QString, Player>, QMap<QString, QMap<QString, QList<QString> > >, QMap<QString, Weapon>)));
        // Setup callback for cleanup when it finishes
        connect(workerThread, SIGNAL(finished()), workerThread, SLOT(deleteLater()));
        workerThread->start(); // This invokes WorkerThread::run in a new thread
    }
}
void RealTimeWindow::resetTable() {
    while (this->ui->playerTable->rowCount() > 0)
    {
        this->ui->playerTable->removeRow(0);
    }

    this->last_line = 0;
    this->players.clear();
    this->timeCache.clear();
    this->weapons.clear();
    this->is_processing = false;
    this->is_waiting = false;
    this->resizeTable();
}
void RealTimeWindow::setupConfigButton(QObject * parent) {
    connect(this->ui->configBtn, SIGNAL(released()), parent, SLOT(onClickSettings()));
}

// Slots
void RealTimeWindow::checkLog() {
    if(is_waiting && is_processing) {
        buildTable();
    }
    QStringList logs = STO->combatlogs(Acc->User["install_directory"].toString() + LOGS);
    this->ui->resetBtn->setDisabled(logs.isEmpty());
    this->ui->uploadBtn->setDisabled(logs.isEmpty() || getFileSize(logs) <= MIN_LOG_SIZE || getFileSize(logs) >= MAX_LOG_SIZE);
}
void RealTimeWindow::onCellClick(int row, int col) {
    QString txt = this->ui->playerTable->item(row, col)->text();
    foreach(Player player, this->players) {
        if(player.handle == txt) {
            if(members.value(player.handle)) {
                QUrlQuery post;
                post.addQueryItem("data[Handle][name]", player.handle);
                QJsonObject data = Web->postJSON(URL_FIND_HANDLE, post);
                if(data["code"].toInt() == 200) {
                    QDesktopServices::openUrl(QString(URL_CHARACTER).arg(player.handle).arg(strtolower(slug(player.character, "-"))));
                }
            }
            break;
        }
    }
}
void RealTimeWindow::onFileChanged(QString path)
{
    if(QFile::exists(path)) {
        monitor();
    } else {
        this->resetTable();
    }
}
void RealTimeWindow::onDirectoryChanged(QString path) // Detects file deletion/creation
{
    if(APP_DEBUG) {
        qDebug() << path;
    }
    QStringList files = STO->combatlogs(path);
    if(!files.isEmpty()) {
        foreach(QString file, files) {
            if(!fileMonitor->files().contains(file)){
                fileMonitor->addPath(file);
            }
        }
        this->buildColumns();
        monitor();
    }
}
void RealTimeWindow::onProgressChanged(int last_line, QMap<QString, Player> players, QMap<QString, QMap<QString, QList<QString> > > times, QMap<QString, Weapon> weapons) {
    this->last_line = last_line;
    this->players = players;
    this->timeCache = times;
    this->weapons = weapons;
    is_waiting = true;
}
void RealTimeWindow::enterEvent(QEvent *event) {
    event;
    qreal percentage = ((1.0/100.0)*Acc->DesktopSetting["opacity"].toInt());
    QPropertyAnimation *animate = new QPropertyAnimation(this, "windowOpacity", this);
    animate->setDuration(100);
    animate->setStartValue(percentage);
    animate->setEndValue(1);
    animate->start();

}
void RealTimeWindow::leaveEvent(QEvent *event) {
    event;
    qreal percentage = ((1.0/100.0)*Acc->DesktopSetting["opacity"].toInt());
    QPropertyAnimation *animate = new QPropertyAnimation(this, "windowOpacity", this);
    animate->setDuration(100);
    animate->setStartValue(1);
    animate->setEndValue(percentage);
    animate->start();
}
void RealTimeWindow::closeEvent(QCloseEvent *e) {
    e;
    this->_logCheck->stop();
    fileMonitor->removePath(Acc->User["install_directory"].toString() + LOGS);
    fileMonitor->removePaths(STO->combatlogs(Acc->User["install_directory"].toString() + LOGS));
    this->resetTable();
}

RealTimeWindow::~RealTimeWindow()
{
    delete ui;
}

void RealTimeWindow::setupUploadBtn(QObject * parent) {
    connect(this->ui->uploadBtn, SIGNAL(released()), parent, SLOT(onClickUpload()));
    this->resetTable();
}

void RealTimeWindow::on_resetBtn_released()
{
    STO->resetCombatLogs(Acc->User["install_directory"].toString() + LOGS);
    this->resetTable();
}
