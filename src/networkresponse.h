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

class NetworkResponse
{
public:
    NetworkResponse(QNetworkReply *reply);
    ~NetworkResponse();
    QNetworkReply::NetworkError error() const;
    QString errorString() const;
    QJsonValue json(QString key);
    QJsonValue json();
    QByteArray binaryData() const{return _binaryData;}
    QVariant data(){return _replyData;}

    bool isJson();
    bool isImage();
    bool isText();
    QByteArray contentTypeHeader() const;
    QUrl url(){return _reply->url();}
    QNetworkAccessManager::Operation operation() const {return _reply->operation();}
    QByteArray rawHeader(const QByteArray &headerName) const{return _reply->rawHeader(headerName);}
    QVariant attribute(QNetworkRequest::Attribute code) const{return _reply->attribute(code);}
    int status() const {return _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();}
    QNetworkReply *networkReply() const {return _reply;}
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    const QJsonObject jsonObject() const;
    const QJsonArray jsonArray() const;

    //use union to avoid conversion at request?
#ifdef QT_HAVE_GUI
    QImage image() const;
#endif

    operator bool();

private:
    QNetworkReply *_reply;
    QByteArray _binaryData;
    QVariant _replyData;
    void processReply();


};

typedef NetworkResponse Response;

QDebug operator <<(QDebug dbg, const NetworkResponse &res);
QDebug operator <<(QDebug dbg, const NetworkResponse *res);

#endif // NETWORKRESPONSE_H
