#ifndef STARTREKONLINE_H
#define STARTREKONLINE_H

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStringList>
#include "structs.h"

class StarTrekOnline : public QObject
{
    Q_OBJECT

public:
    explicit StarTrekOnline(QObject * parent = 0) : QObject(parent) {

    }

    ~StarTrekOnline() {

    }

    // Combines all combat logs into one variable
    QByteArray readCombatLog(QString path) {
        QByteArray callback;
        foreach(QString file, this->combatlogs(path)) {
            QFile combatLog(file);
            if(combatLog.open(QFile::ReadOnly)) {
                callback.append(combatLog.readAll());
                combatLog.close();
            }
        }
        return callback;
    }

    // Combines all shutdown logs into one variable
    QByteArray readShutdownLog(QString path) {
        QByteArray callback;
        foreach(QString file, this->shutdownlogs(path)) {
            QFile shutdownLog(file);
            if(shutdownLog.open(QFile::ReadOnly)) {
                callback.append(shutdownLog.readAll());
                shutdownLog.close();
            }
        }
        return callback;
    }

    // Returns a list of combat logs in the logs directory
    QStringList combatlogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Combatlog*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    // Returns a list of shutdown logs in the logs directory
    QStringList shutdownlogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Shutdown*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    QStringList shutdownLogs2(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Shutdown_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    QStringList voiceChatLogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Voicechat_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }
    QStringList pclLogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Pcl_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    QStringList MakeshaderbinsLogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Makeshaderbins_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    QStringList crashLogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Crash_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    QStringList clientservercommLogs(QString path) {
        QStringList callback;
        QDir dir(path);
        QStringList filters;
        filters << "Clientservercomm_*.Log";
        foreach(QString filename, dir.entryList(filters, QDir::Files)) {
            callback.append(dir.absoluteFilePath(filename));
        }
        return callback;
    }

    // Checks if the combat log is formatted properly
    bool validateCombatLog(QByteArray data) {
        bool callback;
        QStringList lines = QString(data).trimmed().split('\n');
        QStringList parts = lines.first().split(',');
        if(parts.count() == 12) {
            callback = parts.first().contains("::");
        }
        return callback;
    }

    // Finds users handle in combat log
    QStringList getLogHandles(QByteArray data) {
        QStringList callback;
        QStringList lines = QString(data).trimmed().split('\n');
        foreach(QString line, lines) {
            CombatLogItem row(line);
            if(!callback.contains(row.handle)) {
                callback.append(row.handle);
            }
        }
        return callback;
    }

    void fileCleanup(QString path) {
        foreach(QString file, this->shutdownLogs2(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Shutdown.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }

        foreach(QString file, this->clientservercommLogs(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Clientservercomm.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }

        foreach(QString file, this->crashLogs(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Crash.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }

        foreach(QString file, this->MakeshaderbinsLogs(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Makeshaderbins.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }

        foreach(QString file, this->pclLogs(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Pcl.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }

        foreach(QString file, this->voiceChatLogs(path)) {
            QFile input(file);
            if(input.open(QFile::ReadOnly)) {
                QFile output("Voicechat.Log");
                if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                    output.write(input.readAll());
                    output.close();
                }
                input.close();
                QFile::remove(file);
            }
        }
    }

    // Deletes combat logs
    void clearCombatLogs(QString path) {

        // Define backup directory
        QString backup = path + "Nexus Backups/";

        // Create the backup directory if it does not exist
        QDir dir;
        if(dir.exists(backup) || dir.mkpath(backup)) {
            // Write log files into a single file
            QFile output(backup + "Combatlog.Log." + QDateTime::currentDateTime().toString("yyyy-MM-dd_hh-mm-ss"));
            foreach(QString file, this->combatlogs(path)) {
                QFile input(file);
                if(input.open(QFile::ReadOnly | QFile::Text)) {
                    if(output.open(QFile::Append | QFile::WriteOnly | QFile::Text)) {
                        output.write(input.readAll());
                    }
                    input.close();
                    QFile::remove(file);
                }
            }
            output.close();
        }

        // Delete Log files
        this->resetCombatLogs(path);
    }

    void resetCombatLogs(QString path) {
        // Delete Log files
        foreach(QString file, this->combatlogs(path)) {
            QFile::remove(file);
        }
    }

    // Processes shutdown log for characters and handles
    QStringList parseShutdownLog(QString path = NULL) {
        QStringList rows;
        if(!path.isEmpty()) {
            QStringList lines = QString(this->readShutdownLog(path)).trimmed().split('\n');
            foreach(QString line, lines) {
                if(line.contains("Client[0 P[")) {
                    rows.append(line.trimmed());
                }
            }
        }
        return rows;
    }

    // Retreives a list of handles found in shutdown logs
    QStringList getHandles(QString path = NULL) {
        QStringList handles;
        foreach(QString line, this->parseShutdownLog(path)) {
            if(line.contains("Client[0 P[")) {
                line = line.trimmed();
                QString first = line.split("Client[0 P[").last();
                QString last = first.split("]]:").first();
                QString ignore = last.split(' ').first();
                QString data = last.replace(ignore, NULL).trimmed();
                QString handle = data.split('@').last();
                if(!handles.contains(handle)) {
                    handles.append(handle);
                }
            }
        }
        return handles;
    }

    // Retreives a list of characters associated with a specific handle found in shutdown logs
    QStringList getCharactersByHandle(QString path = NULL, QString find = NULL) {
        QStringList characters;
        foreach(QString line, this->parseShutdownLog(path)) {
            if(line.contains("Client[0 P[")) {
                line = line.trimmed();
                QString first = line.split("Client[0 P[").last();
                QString last = first.split("]]:").first();
                QString ignore = last.split(' ').first();
                QString data = last.replace(ignore, NULL).trimmed();
                QString character = data.split('@').first();
                QString handle = data.split('@').last();
                if(!characters.contains(character) && handle == find) {
                    characters.append(character);
                }
            }
        }
        return characters;
    }
};

#endif // STARTREKONLINE_H
