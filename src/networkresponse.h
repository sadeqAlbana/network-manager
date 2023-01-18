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
public:

    NetworkResponse(QNetworkReply *reply, QObject *parent=nullptr);
    ~NetworkResponse();
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    QJsonValue json(QString key);
    QJsonValue json();
    QByteArray binaryData() const{return m_binaryData;}
    QVariant data(){return m_replyData;}

    bool isJson();
    bool isImage();
    bool isText();
    QByteArray contentTypeHeader() const;
    QUrl url(){return m_reply->url();}
    QNetworkAccessManager::Operation operation() const {return m_reply->operation();}
    QByteArray rawHeader(const QByteArray &headerName) const{return m_reply->rawHeader(headerName);}
    QVariant attribute(QNetworkRequest::Attribute code) const{return m_reply->attribute(code);}
    int status() const {return m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();}
    QNetworkReply *networkReply() const {return m_reply;}
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    const QJsonObject jsonObject() const;
    const QJsonArray jsonArray() const;

    //use union to avoid conversion at request?
#ifdef QT_HAVE_GUI
    QImage image() const;
#endif

    operator bool();

    NetworkResponse * subcribe(std::function<void(NetworkResponse *)> cb);

    template <class T>
    NetworkResponse * subcribe(T *instance,void(T::*cb) (NetworkResponse *))
{
        return this->subcribe(std::bind(cb,instance,std::placeholders::_1));

}


signals:
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
