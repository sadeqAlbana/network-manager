/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef NETWORKRESPONSE_H
#define NETWORKRESPONSE_H

#include <QNetworkReply>

#ifdef QT_QML_LIB
#include <QQmlEngine>
#include <QJSValue>
#include <QJsonObject>
#include <QVariant>
#include <QJsonArray>
#include <QJsonValue>
#define SNETWORKMANAGER_INVOKABLE Q_INVOKABLE
#else
#define SNETWORKMANAGER_INVOKABLE
#endif


class QDebug;
class QImage;

class NetworkResponse : public QObject
{
    Q_OBJECT
#ifdef QT_QML_LIB
    QML_ANONYMOUS
#endif
    NetworkResponse(QNetworkReply *reply, QObject *parent=nullptr);
public:
    friend class NetworkAccessManager;
    ~NetworkResponse();
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    SNETWORKMANAGER_INVOKABLE QJsonValue json(QString key);
    SNETWORKMANAGER_INVOKABLE QJsonValue json();
    QByteArray binaryData() const;
    SNETWORKMANAGER_INVOKABLE QVariant data() const;

    SNETWORKMANAGER_INVOKABLE bool isJson();
    SNETWORKMANAGER_INVOKABLE bool isImage();
    SNETWORKMANAGER_INVOKABLE bool isText();
    SNETWORKMANAGER_INVOKABLE QString contentTypeHeader() const;
    SNETWORKMANAGER_INVOKABLE QUrl url() const;
    SNETWORKMANAGER_INVOKABLE QNetworkAccessManager::Operation operation() const;
    QByteArray rawHeader(const QByteArray &headerName) const;
    SNETWORKMANAGER_INVOKABLE QVariant attribute(QNetworkRequest::Attribute code) const;
    SNETWORKMANAGER_INVOKABLE int status() const;
    QNetworkReply *networkReply() const;
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    SNETWORKMANAGER_INVOKABLE const QJsonObject jsonObject() const;
    SNETWORKMANAGER_INVOKABLE const QJsonArray jsonArray() const;


    //this method lacks the way to know attempts count !
    SNETWORKMANAGER_INVOKABLE void waitForFinished();
    //use union to avoid conversion at request?
#ifdef QT_HAVE_GUI
    QImage image() const;
#endif

    operator bool() const;

#ifdef QT_QML_LIB
    Q_INVOKABLE void subscribe(QJSValue cb);
#endif

    NetworkResponse * subscribe(std::function<void(NetworkResponse *)> cb);

    /*!
        \fn template <class T> NetworkResponse * subscribe(T *instance,void(T::*cb) (NetworkResponse *))
        this is an overloaded method, it takes an instance of T and a member function pointer to be called later.
    */

    template <class T>
    NetworkResponse * subscribe(T *instance,void(T::*cb) (NetworkResponse *))
{
        return this->subscribe(std::bind(cb,instance,std::placeholders::_1));

}

    void swap(QNetworkReply *newReply);

signals:
    /*!
        \fn void finished()

        this signal is emitted after QNetworkReply::finished is emitted and the reply's data have been read.
    */
    void finished();

private:
    QNetworkReply *m_reply;
    QByteArray m_binaryData;
    QVariant m_replyData;
    void processReply();
    void onReplyFinished();

};

typedef NetworkResponse Response;

QDebug operator <<(QDebug dbg, const NetworkResponse &res);
QDebug operator <<(QDebug dbg, const NetworkResponse *res);

#endif // NETWORKRESPONSE_H
