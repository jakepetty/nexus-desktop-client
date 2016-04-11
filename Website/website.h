#ifndef WEBSITE_H
#define WEBSITE_H

#include <QObject>
#include <QFile>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>
#include <QJsonDocument>
#include <QJsonObject>
class Website : public QObject
{
    Q_OBJECT
public:
    explicit Website(QObject * parent = 0) : QObject(parent)
    {

    }

    ~Website() {

    }

    bool isAvailable(QString url) {
        bool callback = true;
        QEventLoop eventLoop;
        QNetworkReply *_reply;
        QObject::connect(&this->_manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        this->_request.setUrl(QUrl(url));
        _reply = this->_manager.get(this->_request);

        eventLoop.exec();
        if (_reply->error() != QNetworkReply::NoError) {
            callback = false;
        }

        _reply->close();
        delete _reply;

        return callback;
    }
    bool isLatestVersion(QString url, QString version) {
        QJsonObject data = this->getJSON(url);
        return version == data["latest"].toString();
    }

    QByteArray get(QString url) {
        QByteArray callback;
        QEventLoop eventLoop;
        QNetworkReply *_reply;

        QObject::connect(&this->_manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));
        this->_request.setUrl(url);
        _reply = this->_manager.get(this->_request);

        eventLoop.exec();

        if (_reply->error() == QNetworkReply::NoError) {
            callback = _reply->readAll();
        } else {
            // Error
        }
        _reply->close();
        delete _reply;
        return callback;
    }

    QJsonObject postJSON(QString url, QUrlQuery params = {}) {
        QJsonObject callback;
        QEventLoop eventLoop;
        QNetworkReply *_reply;

        QObject::connect(&this->_manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        this->_request.setRawHeader(QString("Content-Type").toUtf8(), QString("application/x-www-form-urlencoded").toUtf8());
        this->_request.setUrl(QUrl(url));

        _reply = this->_manager.post(this->_request, params.query(QUrl::FullyEncoded).toUtf8());

        eventLoop.exec();

        if (_reply->error() == QNetworkReply::NoError) {
            QByteArray data = _reply->readAll();
            QJsonDocument doc(QJsonDocument::fromJson(data));

            callback = doc.object();
        } else {
            // Error
        }

        _reply->close();
        delete _reply;

        return callback;
    }
    QJsonObject getJSON(QString url) {
        QJsonObject callback;
        QEventLoop eventLoop;
        QNetworkReply *_reply;
        QObject::connect(&this->_manager, SIGNAL(finished(QNetworkReply*)), &eventLoop, SLOT(quit()));

        this->_request.setRawHeader(QString("Content-Type").toUtf8(), QString("application/json").toUtf8());

        this->_request.setUrl(QUrl(url));

        _reply = this->_manager.get(this->_request);

        eventLoop.exec();

        if (_reply->error() == QNetworkReply::NoError) {
            QByteArray data = _reply->readAll();
            QJsonDocument doc(QJsonDocument::fromJson(data));
            callback = doc.object();
        } else {
            // Error
        }

        _reply->close();
        delete _reply;

        return callback;
    }
private:
    QNetworkRequest _request;
    QNetworkAccessManager _manager;
};

#endif // WEBSITE_H
