#include <QSysInfo>
#include <QtGlobal>
#include "SingleApplication/singleapplication.h"
#include "LoginDialog/logindialog.h"
#include "ExistingDialog/existingdialog.h"
#include "app.h"
#include "SetupWizard/setupwizard.h"
#include "common.h"
Account * Acc;
QSettings * settings;
Website * Web;
StarTrekOnline * STO;
bool show_login() {
    LoginDialog login;
    login.init();
    return login.exec();
}
int main(int argc, char *argv[])
{
    qputenv("QT_BEARER_POLL_TIMEOUT", QByteArray::number(-1));
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling); // Required for 4k scaling compatibility PLACEMENT MATTERS
    QtSingleApplication a(argc, argv);

    Web = new Website();
    if(!Web->isAvailable(URL_BASE)) {
        // Displays error dialog if SQL server isn't reachable
        popup("Server Unavailable...", "We're sorry but it appears our server is currently unavailable at this time. Please try again in a few minutes.");
        return -500;
    } else if(!Web->isLatestVersion(URL_VERSION, version())) {
        QJsonObject data = Web->getJSON(QSysInfo::buildAbi().contains("llp64") ? URL_UPDATE_64 : URL_UPDATE_32);
        if(data["url"].isString()) {
            UpdateDialog * update = new UpdateDialog();
            update->start(QUrl(data["url"].toString()));
            update->show();
        } else {
            popup("MediaFire Error", data["error"].toString());
            return -404;
        }
    } else {
        STO = new StarTrekOnline();
        // Check if update in progress
        QString working_file_name = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
        if(working_file_name == APP_TEMP_FILENAME) {
            killTask(APP_FILENAME);
            QFile::remove(APP_FILENAME);
            QFile::copy(APP_TEMP_FILENAME, APP_FILENAME);
            QDesktopServices::openUrl(QUrl::fromLocalFile(APP_FILENAME));
            return -200;
        } else if(QFile::exists(APP_TEMP_FILENAME)) {
            killTask(APP_TEMP_FILENAME);
            QFile::remove(APP_TEMP_FILENAME);
        }

        // Checks if app is already loading
        if(!a.lock())
            return -42;

        // Makes sure the app says alive when all windows are closed
        qApp->setQuitOnLastWindowClosed(false);

        // Setup Globals
        settings = new QSettings(SETTING_FILE, QSettings::IniFormat);
        Acc = new Account();

        // Setup email and password variables for login if they exist
        QString email = settings->value("email").toString();
        QString password = settings->value("password").toString();

        // Show Login Dialog if Email and Password are empty or missing
        if(email.isEmpty() || password.isEmpty()) {
            // Check if login is successful
            if(!show_login()) {
                // Closes app if login window is closed
                return -620;
            }
        } else {
            QUrlQuery post;
            post.addQueryItem("data[User][username]", email);
            post.addQueryItem("data[User][password]", password);
            post.addQueryItem("data[User][timezone]", getTimeZoneOffset());
            QJsonObject data = Web->postJSON(URL_LOGIN, post);

            // Check if login credentials exist
            if(data["code"].toInt() != 200) {

                // Show Login Dialog
                if(!show_login()) {

                    // Close app if Login Dialog is closed
                    return -620;
                }
            } else {
                // Populates user structure
                Acc->setup(data);
            }
        }

        if(!Acc->User["is_setup"].toBool()) {
            SetupWizard * wiz = new SetupWizard();
            wiz->init();
            if(!wiz->exec()) {
                return -620;
            }
        }
        // Starts application
        App * app = new App();
        app->init();

        saveField(URL_EDIT_USER, "User", Acc->User["id"].toInt(),  "app_version", version());
        app->Tray->popup("Nexus Desktop Client Reminder", "Our app sits in your system tray. Do not combine multiple queues/missions into one combat file. Upload your logs at the end of every queue/mission.");
        // Check if combat log exists before starting and prompt user what to do
        QStringList combatlogs = STO->combatlogs(Acc->User["install_directory"].toString() + LOGS);
        if(!combatlogs.isEmpty()) {
            int size = getFileSize(combatlogs);
            if(size >= MIN_LOG_SIZE && size <= MAX_LOG_SIZE) {
                ExistingDialog *dlg = new ExistingDialog();
                if(dlg->exec()) {
                    app->Do_Upload();
                } else {
                    STO->clearCombatLogs(Acc->User["install_directory"].toString() + LOGS);
                }
            } else if(size > 0) {
                STO->clearCombatLogs(Acc->User["install_directory"].toString() + LOGS);
            }
        }
    }
    return a.exec();
}
