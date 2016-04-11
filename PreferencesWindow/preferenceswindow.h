#ifndef PREFERENCESWINDOW_H
#define PREFERENCESWINDOW_H

#define PROCESS_CHECK_SPEED 100
#include "common.h"
#include "HandlesDialog/handlesdialog.h"
#include "RealTimeWindow/realtimewindow.h"
#include "StarTrekOnline/startrekonline.h"
#include "Website/website.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include <QFile>
#include "Windows.h"
#include <psapi.h>
#include <QSystemTrayIcon>
#include <QCloseEvent>
#include <QMenu>
#include <QComboBox>
#include <QFileDialog>
#include <QMovie>

namespace Ui {
class PreferencesWindow;
}

class PreferencesWindow : public QMainWindow
{
    Q_OBJECT

public:
    void setupRTW(RealTimeWindow * rtw) {
        this->rtw = rtw;
    }

    void init();
    void loadData();

    explicit PreferencesWindow(QWidget *parent = 0);
    ~PreferencesWindow();
    QString monitor() {
        QString callback;
        QStringList processList;
        unsigned long aProcesses[2048], cbNeeded, running_processes;
        if(EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded)) {
            running_processes = cbNeeded / sizeof(unsigned long);
            for(unsigned int i = 0; i < running_processes; i++) {
                HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, 0, aProcesses[i]);
                wchar_t buffer[512];
                GetModuleFileNameEx(hProcess, 0, buffer, 512);
                CloseHandle(hProcess);
                QString path = QString::fromWCharArray(buffer);
                if(!processList.contains(path)) {
                    if(path.contains("GameClient.exe") && path.contains("Star Trek Online")) {
                        callback = path.replace(DS "GameClient.exe", NULL);
                    }
                    processList.append(path);
                }
            }
        }
        return callback;
    }

    void avatarUpload(QByteArray fileData) {

        this->_request.setUrl(QUrl(QString(URL_AVATAR).arg(Acc->User["id"].toInt()).arg("o")));
        this->_reply = this->_manager.get(this->_request);

        QString bound = "STO";

        QByteArray data(QString("--" + bound + "\r\n").toUtf8());
        data.append("Content-Disposition: form-data; name=\"_method\"\r\n\r\n");
        data.append("POST\r\n");
        data.append("--" + bound + "\r\n");
        data.append("Content-Disposition: form-data; name=\"data[User][id]\"\r\n\r\n");
        data.append(QString::number(Acc->User["id"].toInt()) + "\r\n");
        data.append("--" + bound + "\r\n");
        data.append("Content-Disposition: form-data; name=\"data[User][file]\"; filename=\"image.jpg\"\r\n");
        data.append("Content-Type: image/jpeg\r\n\r\n");

        data.append(fileData);
        data.append("\r\n");
        data.append("--" + bound + "\r\n");

        this->_request.setRawHeader(QString("Content-Type").toUtf8(),QString("multipart/form-data; boundary=" + bound).toUtf8());
        this->_request.setRawHeader(QString("Content-Length").toUtf8(), QString::number(data.length()).toUtf8());
        this->_reply = this->_manager.post(this->_request, data);
        serviceCheck("Uploading Avatar", "Started");
        connect(this->_reply, SIGNAL(error(QNetworkReply::NetworkError)), this, SLOT(avatarUploadError(QNetworkReply::NetworkError)));
        connect(this->_reply, SIGNAL(uploadProgress(qint64,qint64)), this, SLOT(avatarUploadProgress(qint64,qint64)));
        connect(this->_reply, SIGNAL(finished()),this, SLOT(avatarUploadFinished()));
    }


private slots:
    void on_install_directory_released()
    {
        QDesktopServices::openUrl(QUrl::fromLocalFile(Acc->User["install_directory"].toString()));
    }

    void on_detect_sto_released() {

        QString path = monitor();
        if(path.isEmpty()) {
            popup("STO-Nexus", "Star Trek Online isn't running. Please start Star Trek Online and try again when you reach the character select screen.");
        } else {
            if(Acc->User["install_directory"].toString() != path) {
                QUrlQuery post;
                post.addQueryItem("id", QString::number(Acc->User["id"].toInt()));
                post.addQueryItem("install_directory", path);
                Web->postJSON(URL_EDIT_USER, post);

                popup("STO-Nexus", "Your install directory has been updated successfully!");
            } else {
                popup("STO-Nexus", "Your path is already accurate");
            }
        }
        loadData();
    }

    void on_is_api_enabled_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_api_enabled", checked);
        }
    }

    void on_is_monitor_enabled_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_monitor_enabled", checked);
        }
    }

    void on_is_subscribed_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_subscribed", checked);
        }
    }

    void on_is_charts_inverted_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_charts_inverted", checked);
        }
    }

    void on_show_handles_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "show_handles", checked);
        }
    }
    void on_show_dps_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_dps", checked);
            rtw->resetColumns();
        }
    }

    void on_show_kills_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_kills", checked);
            rtw->resetColumns();
        }
    }

    void on_show_duration_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_duration", checked);
            rtw->resetColumns();
        }
    }

    void on_show_missed_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_misses", checked);
            rtw->resetColumns();
        }
    }

    void on_show_dmg_out_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_dmg_out", checked);
            rtw->resetColumns();
        }
    }

    void on_show_crits_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_crits", checked);
            rtw->resetColumns();
        }
    }

    void on_show_names_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_name", checked);
            rtw->resetColumns();
        }
    }

    void on_show_handles_2_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_handle", checked);
            rtw->resetColumns();
        }
    }

    void on_show_accuracy_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_accuracy", checked);
            rtw->resetColumns();
        }
    }

    void on_show_dmg_in_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_dmg_in", checked);
            rtw->resetColumns();
        }
    }

    void on_show_shots_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_shots", checked);
            rtw->resetColumns();
        }
    }

    void on_show_deaths_toggled(bool checked)
    {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_deaths", checked);
            rtw->resetColumns();
        }
    }

    void on_bind_handles_released();

    void on_opacity_sliderReleased();

    void on_opacity_valueChanged(int value)
    {
        if(is_initialized) {
            qreal percentage = ((1.0/100.0) * value);
            if(this->rtw != 0) {
                this->rtw->setWindowOpacity(percentage);
            }
        }
    }

    void on_show_hardest_incoming_hit_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_hardest_incoming_hit", checked);
            rtw->resetColumns();
        }
    }

    void on_show_hardest_outgoing_hit_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "show_hardest_outgoing_hit", checked);
            rtw->resetColumns();
        }
    }

    void on_hotkey_currentIndexChanged(const QString &arg1) {

        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "hotkey", arg1);
            UnregisterHotKey(NULL, 1);
            UINT modifier = 0x0;
            UINT vkey = 0x0;
            QString hotkey = Acc->DesktopSetting["hotkey"].toString();
            if(hotkey.contains("CTRL") && hotkey.contains("ALT")) {
                modifier = MOD_CONTROL | MOD_ALT;
            } else if(hotkey.contains("CTRL")) {
                modifier = MOD_CONTROL;
            } else if(hotkey.contains("ALT")) {
                modifier = MOD_ALT;
            }
            hotkey.replace("ALT", NULL).replace("CTRL", NULL).replace("+", NULL);
            if(hotkey == "F1") {
                vkey = VK_F1;
            } else if(hotkey == "F2") {
                vkey = VK_F2;
            } else if(hotkey == "F3") {
                vkey = VK_F3;
            } else if(hotkey == "F4") {
                vkey = VK_F4;
            } else if(hotkey == "F5") {
                vkey = VK_F5;
            } else if(hotkey == "F6") {
                vkey = VK_F6;
            } else if(hotkey == "F7") {
                vkey = VK_F7;
            } else if(hotkey == "F8") {
                vkey = VK_F8;
            } else if(hotkey == "F9") {
                vkey = VK_F9;
            } else if(hotkey == "F10") {
                vkey = VK_F10;
            } else if(hotkey == "F11") {
                vkey = VK_F11;
            } else if(hotkey == "F12") {
                vkey = VK_F12;
            } else {
                vkey = VK_F11;
            }
            RegisterHotKey(NULL, 1, modifier | 0x4000, vkey);
        }
    }

    void on_auto_open_fleet_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "auto_open_fleet", checked);
        }
    }

    void on_user_is_private_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_private", checked);
        }
    }

    void on_hide_sidebar_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "hide_sidebar", checked);
        }
    }

    void on_screenshot_is_private_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "screenshot_is_private", checked);
        }
    }

    void on_screenshot_is_browsable_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "screenshot_is_browsable", checked);
        }
    }

    void on_screenshot_is_friends_only_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "screenshot_is_friends_only", checked);
        }
    }

    void on_screenshot_is_downloadable_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "screenshot_is_downloadable", checked);
        }
    }

    void on_is_bar_chart_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_bar_chart", checked);
        }
    }

    void avatarUploadProgress(qint64 bytesSent,qint64 bytesTotal) {
        qreal speed = ((bytesSent * 1.0) / bytesTotal);
        if(speed == 1) {
            serviceCheck("Uploading Avatar", "Done");
        } else if(bytesTotal > 0) {
            serviceCheck("Uploading Avatar", QString::number(speed * 100, 'f', 2) + "%");
        }
    }

    void avatarUploadError(QNetworkReply::NetworkError error) {
        if(error != QNetworkReply::NoError) {
            QString err = this->_reply->errorString();
            if(!err.isEmpty()) {
                if(err == "Unable to Write") {
                    popup("Connection Error...", "The clients connection to the server was aborted.");
                } else {
                    popup("Server Error...", err);
                }
            } else {
                popup("Server Error...", "There was an issue with our server please try again.");
            }
            serviceCheck("Uploading Avatar", this->_reply->errorString());
        }

        this->is_uploading = false;
    }

    void avatarUploadFinished() {
        if(this->_reply->error() == QNetworkReply::NoError) {
            QByteArray data = this->_reply->readAll();
            debug_log("PreferencesWindow::avatarUploadFinished", data);
            QJsonDocument doc(QJsonDocument::fromJson(data));
            QJsonObject json = doc.object();
            int code = json["code"].toInt();
            if(code != 200) {
                QString errors;
                foreach(QJsonValue items, json["errors"].toArray()) {
                    foreach(QJsonValue error, items.toArray()) {
                        errors.append(error.toString() + "\n");
                    }
                }
                if(!errors.isEmpty()) {
                    popup("Errors", errors);
                }
            }
            this->loadData();
            serviceCheck("Processing Avatar", QString::number(code));
        } else {
            serviceCheck("Processing Avatar", this->_reply->errorString());
        }

        this->is_uploading = false;
    }

    void on_avatar_upload_btn_released();

    void on_screenshot_auto_open_toggled(bool checked) {
        if(is_initialized) {
            saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(),  "screenshot_auto_open", checked);
        }
    }

    void on_hide_friends_toggled(bool checked){
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "hide_friends", checked);
        }
    }
    void on_hide_characters_toggled(bool checked){
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "hide_characters", checked);
        }
    }

    void on_cleanUp_released();

    void on_refreshBtn_released();

    void on_is_xmpp_news_toggled(bool checked){
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_xmpp_news", checked);
        }
    }

    void on_is_xmpp_tribble_news_toggled(bool checked){
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_xmpp_tribble_news", checked);
        }
    }

    void on_is_xmpp_service_monitor_toggled(bool checked){
        if(is_initialized) {
            saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "is_xmpp_service_monitor", checked);
        }
    }

private:
    bool is_initialized = false;
    bool is_uploading = false;
    Ui::PreferencesWindow *ui;
    RealTimeWindow * rtw;
    QNetworkReply *_reply;
    QNetworkRequest _request;
    QNetworkAccessManager _manager;
};

#endif // PREFERENCESWINDOW_H
