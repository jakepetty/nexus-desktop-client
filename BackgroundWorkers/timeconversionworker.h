#ifndef TIMECONVERSIONWORKER
#define TIMECONVERSIONWORKER
#include <QDebug>
#include <QThread>
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QDateTime>
class TimeConversionWorker : public QThread
{
    Q_OBJECT
public:

    explicit TimeConversionWorker(QByteArray data, QObject *parent = 0) : QThread(parent) {
        this->data = data;
    }

    void run() {
        QStringList lines = QString(data).trimmed().split("\n");
        QString new_lines;
        QString players;
        QStringList players_cache;
        foreach(QString line, lines) {

            // Split line by separator
            QStringList parts = line.split(",");

            // Pull datetime from line
            QStringList date_parts = parts.first().split("::");

            // Convert datetime to UTC
            QString time = QDateTime::fromString(date_parts.first(), "yy:MM:dd:HH:mm:ss.z").toUTC().toString("yy:MM:dd:HH:mm:ss.z");

            // Reformat the line
            QString entry = QString("%0::%1,%2,%3,%4,%5,%6,%7,%8,%9,%10,%11,%12\n").arg(time).arg(date_parts.last()).arg(parts[1]).arg(parts[2]).arg(parts[3]).arg(parts[4]).arg(parts[5]).arg(parts[6]).arg(parts[7]).arg(parts[8]).arg(parts[9]).arg(parts[10]).arg(parts[11]);
            if(!players_cache.contains(parts[1])){
                if(QString(parts[1]).contains("P[")){
                    players_cache.append(parts[1]);
                }
            }
            // Append line to string
            new_lines.append(entry);
        }
        players_cache.sort();
        foreach(QString player, players_cache){
            players.append(player);
        }

        players_cache.clear();
        // Return processed results
        emit onComplete(new_lines.toUtf8(), players);
    }
signals:
    void onComplete(QByteArray,QString);
private:
    QByteArray data;
};
#endif // TIMECONVERSIONWORKER
