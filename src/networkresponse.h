/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef NETWORKRESPONSE_H
#define NETWORKRESPONSE_H

#include <QNetworkReply>
class QDebug;
class QImage;

class NetworkResponse : public QObject
{
    Q_OBJECT

    NetworkResponse(QNetworkReply *reply, QObject *parent=nullptr);
public:
    friend class NetworkAccessManager;
    ~NetworkResponse();
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    QJsonValue json(QString key);
    QJsonValue json();
    QByteArray binaryData() const;
    QVariant data() const;

    bool isJson();
    bool isImage();
    bool isText();
    QString contentTypeHeader() const;
    QUrl url() const;
    QNetworkAccessManager::Operation operation() const;
    QByteArray rawHeader(const QByteArray &headerName) const;
    QVariant attribute(QNetworkRequest::Attribute code) const;
    int status() const;
    QNetworkReply *networkReply() const;
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    const QJsonObject jsonObject() const;
    const QJsonArray jsonArray() const;

    //this method lacks the way to know attempts count !
    void waitForFinished();
    //use union to avoid conversion at request?
#ifdef QT_HAVE_GUI
    QImage image() const;
#endif

    operator bool() const;

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
