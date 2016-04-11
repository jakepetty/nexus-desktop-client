#ifndef STRUCTS_H
#define STRUCTS_H

#include <QHash>
#include <QDateTime>
#include <QMap>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>
struct Report {
    int id;
    int user_id;
    int difficulty_id;
    int map_id;
    double top_dps;
    double top_base_dps;
    int kills;
    double dmg;
    double base_dmg;
    QString identifier;
    QString comment;
    bool is_ground;
    bool is_pvp;
    bool is_downloadable;
    bool is_browsable;
    bool is_pug;
    bool is_success;
    QString seo_url;
    int player_count;
    int view_count;
    int duration;
    QString created;
    Report(QMap<QString, QString> data) {
        id = data["id"].toInt();
        user_id = data["user_id"].toInt();
        difficulty_id = data["difficulty_id"].toInt();
        map_id = data["map_id"].toInt();
        top_dps = data["top_dps"].toDouble();
        top_base_dps = data["top_base_dps"].toDouble();
        kills = data["kills"].toInt();
        dmg = data["dmg"].toDouble();
        base_dmg = data["base_dmg"].toDouble();
        identifier = data["identifier"];
        comment = data["comment"];
        is_ground = data["is_ground"].toInt() == 1;
        is_pvp = data["is_pvp"].toInt() == 1;
        is_downloadable = data["is_downloadable"].toInt() == 1;
        is_browsable = data["is_browsable"].toInt() == 1;
        is_pug = data["is_pug"].toInt() == 1;
        is_success = data["is_success"].toInt() == 1;
        seo_url = data["seo_url"];
        player_count = data["player_count"].toInt();
        view_count = data["view_count"].toInt();
        duration = data["duration"].toInt();
        created = data["created"];
    }
};

struct Weapon {
    QString name;
    QString owner;
    Weapon() {

    }
    void setName(QString name) {
        this->name = name;
    }

    void setOwner(QString owner) {
        this->owner = name;
    }

    bool isEmpty() {
        return this->name.isEmpty() && this->owner.isEmpty();
    }
};

struct CombatLogItem {
    QString time;
    QString attacker_id;
    QString pet_name;
    QString pet_id;
    QString target_name;
    QString target_id;
    QString weapon_name;
    QString weapon_id;
    QString type;
    QString flags;
    double  dmg;
    double  base_dmg;
    QString handle;
    QString character;
    QString rounded_time;
    QString category;

    CombatLogItem(QString line = NULL) {
        if(!line.isEmpty()) {
            QStringList parts = line.split(",");
            QDateTime date = QDateTime::fromString("20" + parts.first().split("::").first(), "yyyy:MM:dd:HH:mm:ss.z").toUTC();
            if(!date.isValid()) {
                date = QDateTime::fromString(parts.first().split("::").first(), "yyyy:MM:dd:HH:mm:ss.z");
            }
            this->time = date.toString("yyyy:MM:dd HH:mm:ss.z");
            this->rounded_time = date.toString("yyyy:MM:dd HH:mm:ss");
            this->attacker_id = parts[1];
            this->pet_name = parts[2];
            this->pet_id = parts[3];
            this->target_name = parts[4];
            this->target_id = parts[5];
            this->weapon_name = parts[6];
            this->weapon_id = parts[7];
            this->type = (parts[8].isEmpty() ? "Miss" : parts[8]);
            this->flags = parts[9];
            this->dmg = qAbs(parts[10].toDouble());
            this->base_dmg = qAbs(parts[11].toDouble());

            if(this->attacker_id.startsWith('P')) {
                QStringList parts = this->decodePlayer(this->attacker_id);
                if(!parts.isEmpty()) {
                    this->character = parts.first();
                    this->handle = parts.last();
                }
            }
        }
    }
    void reset() {
        this->time = "";
        this->rounded_time = "";
        this->attacker_id = "";
        this->pet_name = "";
        this->pet_id = "";
        this->target_name = "";
        this->target_id = "";
        this->weapon_name = "";
        this->weapon_id = "";
        this->type = "";
        this->flags = "";
        this->dmg = 0.00;
        this->base_dmg = 0.00;
        this->handle = "";
        this->character = "";
    }
    QString swapTargetHandle() {
        if(this->target_id.contains("P[")) {
            QStringList parts = this->decodePlayer(this->target_id);
            this->handle = parts.last();
            this->character = parts.first();
        }
        return this->handle;
    }
    void setTargetName(QString name) {
        this->target_name = name;
    }

    void setCategory(QString name) {
        this->category = name;
    }
    void setAttackerId(QString id) {
        this->attacker_id = id;
        if(this->attacker_id.contains("P[")) {
            QStringList parts = this->decodePlayer(this->attacker_id);
            this->handle = parts.last();
            this->character = parts.first();
        }
    }

    void setTargetId(QString id) {
        this->target_id = id;
    }
    void setWeaponName(QString name) {
        this->weapon_name = name;
    }
    QStringList decodePlayer(QString handle) {
        QString ignore = handle.split(' ').first();
        handle.remove(ignore + " ").remove(']');
        return handle.split('@');
    }
};

struct Player {
    QString character;
    QString handle;
    double  dmg_out;
    double  dmg_in;
    int duration;
    int kills;
    int deaths;
    int crits;
    int misses;
    int immunes;
    int flanks;
    int shots;
    int dodges;
    double  hardest_hit_out;
    double  hardest_hit_in;

    Player() {
        this->handle = "";
        this->character = "";
        this->dmg_in = 0.00;
        this->dmg_out = 0.00;
        this->duration = 0;
        this->kills = 0;
        this->flanks = 0;
        this->immunes = 0;
        this->deaths = 0;
        this->crits = 0;
        this->misses = 0;
        this->shots = 0;
        this->dodges = 0;
        this->hardest_hit_out = 0.00;
        this->hardest_hit_in = 0.00;
    }
    QString getValue(QString variable) {
        if(variable == "handle") {
            return this->handle;
        } else if(variable == "name") {
            return this->character;
        } else if(variable == "hardest_outgoing_hit") {
            return QString::number(this->hardest_hit_out, 'f', 2);
        } else if(variable == "hardest_incoming_hit") {
            return QString::number(this->hardest_hit_in, 'f', 2);
        } else if(variable == "dmg_in") {
            return QString::number(this->dmg_in, 'f', 2);
        } else if(variable == "dmg_out") {
            return QString::number(this->dmg_out, 'f', 2);
        } else if(variable == "duration") {
            return QString::number(this->duration);
        } else if(variable == "kills") {
            return QString::number(this->kills);
        } else if(variable == "deaths") {
            return QString::number(this->deaths);
        } else if(variable == "crits") {
            return QString::number(this->crits);
        } else if(variable == "misses") {
            return QString::number(this->misses);
        } else if(variable == "shots") {
            return QString::number(this->shots);
        } else if(variable == "dps") {
            return QString::number(this->getDPS(), 'f', 2);
        } else if(variable == "accuracy") {
            double hits = (this->shots - this->misses);
            double accuracy = (hits / this->shots) * 100;
            return QString::number(accuracy, 'f', 2);
        } else {
            return NULL;
        }
    }

    void setHandle(QString handle) {
        this->handle = handle;
    }
    void setCharacter(QString character) {
        this->character = character;
    }
    QString getName() {
        if(!this->character.isEmpty() && !this->handle.isEmpty())
            return "";
        return this->character + "@" + this->handle;
    }

    void updateInDamage(double  dmg) {
        this->dmg_in += dmg;
    }
    void updateOutDamage(double  dmg) {
        this->dmg_out += dmg;
    }
    void updateShots() {
        this->shots++;
    }

    void updateKills() {
        this->kills++;
    }
    void updateDodges() {
        this->dodges++;
    }
    void updateDeaths() {
        this->deaths++;
    }
    void updateFlanks() {
        this->flanks++;
    }
    void updateImmunes() {
        this->immunes++;
    }
    void updateCrits() {
        this->crits++;
    }
    void updateMisses() {
        this->misses++;
    }
    void setHardestHitOut(double dmg) {
        this->hardest_hit_out = dmg;
    }
    void setHardestHitIn(double dmg) {
        this->hardest_hit_in = dmg;
    }
    void updateDuration() {
        // If numbers go weird change this to the log way
        this->duration++;
    }
    int getIntDPS() {
        return this->getDPS();
    }
    double getDPS() {
        return (this->dmg_out == 0 ? 0 : (this->dmg_out / this->duration));
    }

};

struct Account {
    QJsonObject User;
    QJsonObject Group;
    QJsonObject Gender;
    QJsonArray  Handle;
    QJsonObject Country;
    QJsonObject DesktopSetting;
    Account() {
    }
    void set(QString model, QString field, QJsonValue value) {
        if(model == "User") {
            this->User.insert(field, value);
        }
        if(model == "DesktopSetting") {
            this->DesktopSetting.insert(field, value);
        }
    }

    void setup(QJsonObject data) {
        this->User    = data["User"].toObject();
        this->Group   = data["Group"].toObject();
        this->Gender  = data["Gender"].toObject();
        this->Handle  = data["Handle"].toArray();
        this->Country = data["Country"].toObject();
        this->DesktopSetting = data["DesktopSetting"].toObject();
    }

    bool exists(QString handle_name, QString character_name = 0) {
        bool callback = false;
        foreach(QJsonValue handle, this->Handle) {
            QJsonObject Handle = handle.toObject();
            if(Handle["name"].toString() == handle_name) {
                if(character_name.isNull()) {
                    callback = true;
                    break;
                } else {
                    foreach(QJsonValue character, Handle["Character"].toArray()) {
                        QJsonObject Character = character.toObject();
                        if(Character["name"].toString() == character_name) {
                            callback = true;
                            break;
                        }
                    }
                    break;
                }
            }
        }
        return callback;
    }

    int getHandleId(QString handle_name) {
        int callback;
        foreach(QJsonValue handle, this->Handle) {
            QJsonObject Handle = handle.toObject();
            if(Handle["name"].toString() == handle_name) {
                callback = Handle["id"].toInt();
                break;
            }
        }
        return callback;
    }
    bool checkHandle(QString handle_name) {
        foreach(QJsonValue handle, this->Handle) {
            QJsonObject Handle = handle.toObject();
            if(Handle["name"].toString() == handle_name) {
                return true;
            }
        }
        return false;
    }

    int getCharacterCount() {
        int callback = 0;
        foreach(QJsonValue handle, this->Handle) {
            QJsonObject Handle = handle.toObject();
            callback += Handle["character_count"].toInt();
        }
        return callback;
    }

};
#endif /* STRUCTS_H */
