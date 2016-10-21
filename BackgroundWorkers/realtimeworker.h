#ifndef REALTIMEWORKER_H
#define REALTIMEWORKER_H

#include "structs.h"
#include <QDebug>
#include <QObject>
#include <QThread>
#include <QFile>
#include <QMap>
#include <QApplication>

class RealTimeWorker : public QThread
{
    Q_OBJECT
public:

    explicit RealTimeWorker( QStringList paths, int last_line, QMap<QString, Player> players, QMap<QString, QMap<QString, QList<QString> > > times, QMap<QString, Weapon> weapons, QObject * parent = 0 ) : QThread( parent ) {

        // Needed for threading to work properly
        qRegisterMetaType <QMap<QString, QString> > ( "QMap<QString, QString>" );
        qRegisterMetaType <QMap<QString, Player> > ( "QMap<QString, Player>" );
        qRegisterMetaType <QMap<QString, Weapon> > ( "QMap<QString, Weapon>" );
        qRegisterMetaType <QMap<QString, QMap<QString, QList<QString> > > > ( "QMap<QString, QMap<QString, QList<QString> > >" );

        this->last_line = last_line;
        this->players = players;
        this->timeCache = times;
        this->paths = paths;
        this->weapons = weapons;
    }
protected:
    void run() {
        // allocate resources using new
        QByteArray logData;
        foreach( QString file, this->paths ) {

            QFile combatLog( file );
            if ( combatLog.open( QFile::ReadOnly )) {
                logData.append( combatLog.readAll());
            }
        }

        if ( !logData.isEmpty()) {
            QStringList lines = QString( logData ).trimmed().split( "\n" );
            QMap<QString, QMap<QString, QList<CombatLogItem> > > objects;

            // + 1 so it doesn't reparse the last line

            for ( int i = this->last_line; i < lines.count(); i++ ) {
                // Ignore Fall Damage
                if ( !lines[i].contains( "Autodesc.Combatevent.Falling" )) {
                    CombatLogItem data( lines[i] );

                    if ( !data.attacker_id.isEmpty()) {
                        objects[data.weapon_id][(!data.type.isEmpty() ? data.type : "Miss")].append( data );
                    }
                }
                this->last_line = i + 1;
            }

            QStringList weapons = objects.keys();
            QList<CombatLogItem> rows;
            foreach( QString weapon, weapons ) {
                QStringList categories = objects[weapon].keys();
                foreach( QString category, categories ) {
                    foreach( CombatLogItem item, objects[weapon][category] ) {
                        if ( categories.count() == 1 && categories.contains( "Shield" )) {
                            // Calculate Shield Heals
                            item.setCategory( "Shield Heal" );
                            rows.append( item );
                        } else if ( categories.contains( "Kinetic" ) || !categories.contains( "HitPoints" )) {
                            // Calculate Hull, SHield and Kinetic Damage
                            if ( item.type == "Kinetic" || item.type == "Physical" || item.type == "Shield" ) {
                                item.setCategory( item.type + " Damage" );
                            } else {
                                item.setCategory( "Hull Damage" );
                            }
                            rows.append( item );
                        } else if ( categories.contains( "HitPoints" )) {
                            // Calculate Hull Heals
                            item.setCategory( "Hull Heal" );
                            rows.append( item );
                        } else {
                            // Should be Empty
                        }
                    }
                }
            }

            foreach( CombatLogItem row, rows ) {
                if ( this->__isPlayer( row )) {
                    if ( row.category.contains( "Damage" )) {
                        if ( this->__isSelf( row )) {
                            // Setup Self Damage
                        } else {
                            // Setup Out Damage
                            this->__setupOutAttack( row );
                        }
                    } else if ( row.category.contains( "Heal" )) {
                        // Setup Heals
                        if ( !row.weapon_name.isEmpty()) {
                            if ( this->__isPvP( row )) {
                                // Setup In and Out Healing
                            } else if ( this->__isSelf( row )) {
                                // Setup Self Healing
                            } else {
                                // Healing to NPC's
                            }
                        } else {
                            // Unknown Heal
                        }
                    } else {
                        // Should be empty
                    }
                } else {
                    if ( row.category.contains( "Damage" )) {
                        if ( this->__isNPCVsPlayer( row )) {
                            // Setup In Damage
                            row.swapTargetHandle();
                            this->players[row.target_id].setCharacter( row.character );
                            this->players[row.target_id].setHandle( row.handle );
                            this->players[row.target_id].updateInDamage( row.dmg );

                            if ( row.flags.contains( "Kill" )) {
                                this->players[row.target_id].updateDeaths();
                            }
                            if ( this->players[row.target_id].hardest_hit_in < row.dmg ) {
                                this->players[row.target_id].setHardestHitIn( row.dmg );
                            }
                        } else {
                            // NPC vs NPC damage
                        }
                    } else {
                        // NPC vs NPC Healing
                    }
                }
            }
        }
        emit progressChanged( this->last_line, this->players, this->timeCache, this->weapons );
    }
signals:
    void progressChanged( int, QMap<QString, Player>, QMap<QString, QMap<QString, QList<QString> > >, QMap<QString, Weapon> );
private:
    int last_line;
    QStringList paths;
    QMap<QString, Player> players;
    QMap<QString, QMap<QString, QList<QString> > > timeCache;
    QMap<QString, Weapon> weapons;
    bool __isPlayer( CombatLogItem row ) {
        return row.attacker_id.startsWith( 'P' );
    }
    bool __isSelf( CombatLogItem row ) {
        return row.target_id == row.attacker_id || (row.attacker_id.startsWith( 'P' ) && row.target_id == "*");
    }
    void __setupSelfDamage( CombatLogItem row ) {}

    void __setupOutAttack( CombatLogItem row ) {

        QString player = row.attacker_id;
        this->players[player].setCharacter( row.character );
        this->players[player].setHandle( row.handle );
        this->players[player].updateShots();


        if ( row.category.contains( "Damage" )) {
            // Sets up shield damage
            this->players[player].updateOutDamage( row.dmg );
        }
        if ( row.flags.contains( "Critical" )) {
            this->players[player].updateCrits();
        }
        if ( row.flags.contains( "Flank" )) {
            this->players[player].updateFlanks();
        }
        if ( row.flags.contains( "Kill" )) {
            this->players[player].updateKills();
        }
        if ( row.flags.contains( "Dodge" )) {
            this->players[player].updateDodges();
        }
        if ( row.flags.contains( "Immune" )) {
            this->players[player].updateImmunes();
        }
        if ( row.flags.contains( "Miss" )) {
            this->players[player].updateMisses();
        }
        if ( row.flags.contains( "Miss" )) {
            this->players[player].updateMisses();
        }

        if ( row.category == "Shield Damage" ) {
            // Sets up shield damage
        } else {
            // Sets up hull damage
        }

        if ( this->players[player].hardest_hit_out < row.dmg ) {
            this->players[player].setHardestHitOut( row.dmg );
        }

        if ( !this->timeCache["OutAttack"][player].contains( row.rounded_time )) {
            this->players[player].updateDuration();
            this->timeCache["OutAttack"][player].append( row.rounded_time );
        }

    }

    bool __isPvP( CombatLogItem row ) {
        return row.target_id.startsWith( 'P' ) && row.attacker_id.startsWith( 'P' ) && row.target_id != row.attacker_id;
    }

    QString __getWeapon( QString id ) {
        return (!this->weapons[id].name.isEmpty() ? this->weapons[id].name : "Unknown");
    }
    bool __isNPCVsPlayer( CombatLogItem row ) {
        return row.attacker_id.startsWith( 'C' ) && row.target_id.startsWith( 'P' );
    }
};

#endif // REALTIMEWORKER_H
