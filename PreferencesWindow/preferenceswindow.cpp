#include "preferenceswindow.h"
#include "ui_preferenceswindow.h"

PreferencesWindow::PreferencesWindow(QWidget *parent):
    QMainWindow(parent, Qt::Dialog),
    ui(new Ui::PreferencesWindow)
{
    ui->setupUi(this);
}
void PreferencesWindow::init() {
    if(!is_initialized) {
        QMovie* movie = new QMovie(":/images/spinner.gif");
        this->ui->spinner->setMovie(movie);
        movie->start();
        this->ui->version->setText("Version " + version());
        is_initialized = true;
        this->loadData();
    }
    int h = QTime::currentTime().hour();
    if(h >= 5 && h < 12) {
        this->ui->greeting->setText("Good Morning");
    } else if(h >= 12 && h < 17) {
        this->ui->greeting->setText("Good Afternoon");
    } else {
        this->ui->greeting->setText("Good Evening");
    }
}

PreferencesWindow::~PreferencesWindow()
{
    delete ui;
}

void PreferencesWindow::loadData() {
    ui->spinner->hide();
    this->ui->avatar_upload_btn->show();
    QJsonObject callback = Web->getJSON(QString(URL_AVATAR).arg(Acc->User["id"].toInt()).arg("a"));
    if(callback["code"].toInt() == 200) {
        QByteArray imgData = Web->get(callback["url"].toString() + "?"+QDateTime::currentDateTime().toTime_t());
        this->ui->avatar->setPixmap(QPixmap::fromImage(QImage::fromData(imgData, "JPEG")));
    }

    QFile file(Acc->User["install_directory"].toString() + DS + ".patch" + DS + "patch_trivia.txt");
    QMap<QString, QString> fileinfo;
    if(file.open(QFile::ReadOnly)) {
        QStringList lines = QString(file.readAll()).trimmed().split("\n");
        int i = 0;
        foreach(QString line, lines) {
            line = line.trimmed().replace("Trivia ", "");
            fileinfo[line.mid(0, line.indexOf(' '))] = line.mid(line.indexOf(' ')+1).replace("\"", "");
            i++;
        }
        file.close();
    }
    this->ui->name->setText(Acc->User["name"].toString());
    this->ui->group->setText(Acc->Group["name"].toString());
    this->ui->report_count->setText(QString::number(Acc->User["report_count"].toInt()));
    this->ui->character_count->setText(QString::number(Acc->getCharacterCount()));
    this->ui->handle_count->setText(QString::number(Acc->User["handle_count"].toInt()));
    this->ui->screenshot_count->setText(QString::number(Acc->User["screenshot_count"].toInt()));
    this->ui->install_directory->setText(Acc->User["install_directory"].toString());
    this->ui->patch_name->setText(fileinfo["PatchName"]);
    this->ui->patch_branch->setText(fileinfo["PatchBranchName"]);
    this->ui->patch_time->setText(fileinfo["PatchTimeReadable"]);
    this->ui->is_api_enabled->setChecked(Acc->User["is_api_enabled"].toBool());
    this->ui->is_charts_inverted->setChecked(Acc->User["is_charts_inverted"].toBool());
    this->ui->is_monitor_enabled->setChecked(Acc->User["is_monitor_enabled"].toBool());
    this->ui->is_subscribed->setChecked(Acc->User["is_subscribed"].toBool());
    this->ui->show_handles->setChecked(Acc->User["show_handles"].toBool());
    this->ui->auto_open_fleet->setChecked(Acc->DesktopSetting["auto_open_fleet"].toBool());

    this->ui->show_accuracy->setChecked(Acc->DesktopSetting["show_accuracy"].toBool());
    this->ui->show_crits->setChecked(Acc->DesktopSetting["show_crits"].toBool());
    this->ui->show_deaths->setChecked(Acc->DesktopSetting["show_deaths"].toBool());
    this->ui->show_dmg_in->setChecked(Acc->DesktopSetting["show_dmg_in"].toBool());
    this->ui->show_dmg_out->setChecked(Acc->DesktopSetting["show_dmg_out"].toBool());
    this->ui->show_dps->setChecked(Acc->DesktopSetting["show_dps"].toBool());
    this->ui->show_duration->setChecked(Acc->DesktopSetting["show_duration"].toBool());
    this->ui->show_handles_2->setChecked(Acc->DesktopSetting["show_handle"].toBool());
    this->ui->show_kills->setChecked(Acc->DesktopSetting["show_kills"].toBool());
    this->ui->show_missed->setChecked(Acc->DesktopSetting["show_misses"].toBool());
    this->ui->show_shots->setChecked(Acc->DesktopSetting["show_shots"].toBool());
    this->ui->show_names->setChecked(Acc->DesktopSetting["show_name"].toBool());
    this->ui->show_hardest_incoming_hit->setChecked(Acc->DesktopSetting["show_hardest_incoming_hit"].toBool());
    this->ui->show_hardest_outgoing_hit->setChecked(Acc->DesktopSetting["show_hardest_outgoing_hit"].toBool());
    this->ui->opacity->setValue(Acc->DesktopSetting["opacity"].toInt());
    this->ui->hotkey->setCurrentText(Acc->DesktopSetting["hotkey"].toString());

    this->ui->screenshot_is_browsable->setChecked(Acc->DesktopSetting["screenshot_is_browsable"].toBool());
    this->ui->screenshot_is_downloadable->setChecked(Acc->DesktopSetting["screenshot_is_downloadable"].toBool());
    this->ui->screenshot_is_friends_only->setChecked(Acc->DesktopSetting["screenshot_is_friends_only"].toBool());
    this->ui->screenshot_is_private->setChecked(Acc->DesktopSetting["screenshot_is_private"].toBool());
    this->ui->screenshot_auto_open->setChecked(Acc->DesktopSetting["screenshot_auto_open"].toBool());

    this->ui->hide_characters->setChecked(Acc->User["hide_characters"].toBool());
    this->ui->hide_friends->setChecked(Acc->User["hide_friends"].toBool());
    this->ui->user_is_private->setChecked(Acc->User["is_private"].toBool());
    this->ui->hide_sidebar->setChecked(Acc->User["hide_sidebar"].toBool());
    this->ui->is_bar_chart->setChecked(Acc->User["is_bar_chart"].toBool());
    this->ui->is_xmpp_news->setChecked(Acc->User["is_xmpp_news"].toBool());
    this->ui->is_xmpp_tribble_news->setChecked(Acc->User["is_xmpp_tribble_news"].toBool());
    this->ui->is_xmpp_service_monitor->setChecked(Acc->User["is_xmpp_service_monitor"].toBool());
}

void PreferencesWindow::on_bind_handles_released()
{
    QStringList handles = STO->getHandles(Acc->User["install_directory"].toString() + LOGS);
    if(Acc->User["install_directory"].toString().isEmpty()) {
        if(confirm("You haven't configured your Star Trek Online install directory yet. Would you like to do so now?")) {
            // do something
            popup("Step 1","Please start Star Trek Online if you haven't done so already.");
            this->on_detect_sto_released();
        } else {
            // do something else
        }
    } else if(handles.isEmpty()) {
        popup("Problem detecting handles", "Please verify your install directory is accurate if it is then please contact us with a bug report.");
    } else {
        HandlesDialog *handleDlg = new HandlesDialog(handles);
        if(handleDlg->exec()) {
            QUrlQuery post;
            post.addQueryItem("data[Handle][user_id]", QString::number(Acc->User["id"].toInt()));
            Web->postJSON(URL_UNBIND_HANDLES, post);
            post.clear();
            foreach(QListWidgetItem *handle, handleDlg->getAllowed()) {
                post.addQueryItem("data[Handle][user_id]", QString::number(Acc->User["id"].toInt()));
                post.addQueryItem("data[Handle][name]", handle->text());
                int i = 0;
                foreach(QString character, STO->getCharactersByHandle(Acc->User["install_directory"].toString() + LOGS, handle->text())) {
                    post.addQueryItem(QString("data[Character][%0][name]").arg(i), character);
                    i++;
                }
                Web->postJSON(URL_BIND_HANDLE, post);
                post.clear();
            }
            relog();
            loadData();
        }
    }
}

void PreferencesWindow::on_opacity_sliderReleased()
{
    saveField(URL_EDIT_DESKTOPSETTING, "DesktopSetting", Acc->DesktopSetting["id"].toInt(), "opacity", this->ui->opacity->value());
}

void PreferencesWindow::on_avatar_upload_btn_released()
{

    QFileDialog * dlg = new QFileDialog();
    dlg->setNameFilter(tr("JPEG (*.jpg *.jpeg)" ));
    dlg->exec();
    QStringList files = dlg->selectedFiles();
    if(!files.isEmpty()) {
        this->ui->avatar_upload_btn->hide();
        this->ui->spinner->show();
        foreach(QString file, files) {
            QFile img(file);
            if(img.open(QFile::ReadOnly)) {
                this->avatarUpload(img.readAll());
            }
        }
    }
}

void PreferencesWindow::on_cleanUp_released()
{
    if(confirm("This process merges your rotated log files together based on their file name. Since this process alters files that Star Trek Online uses it's probably best you close your game before using this feature. Are you sure you want to continue?")){
        STO->fileCleanup(Acc->User["install_directory"].toString() + LOGS);
        if(confirm("Cleanup Complete! Would you like to view your log folder now?")){
            QDesktopServices::openUrl(QUrl::fromLocalFile(Acc->User["install_directory"].toString() + LOGS));
        }
    }
}

void PreferencesWindow::on_refreshBtn_released()
{
    this->loadData();
}
