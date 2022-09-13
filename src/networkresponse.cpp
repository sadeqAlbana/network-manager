/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "networkresponse.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#ifdef QT_HAVE_GUI
#include <QImage>
#endif
#include <QDebug>
NetworkResponse::NetworkResponse(QNetworkReply *reply):_reply(reply)
{
    if(reply->error()!=QNetworkReply::OperationCanceledError && reply->error()!=QNetworkReply::TimeoutError)
    {
        _binaryData=reply->readAll();
        processReply();
    }
}

NetworkResponse::~NetworkResponse()
{
    _reply->deleteLater();
}

QNetworkReply::NetworkError NetworkResponse::error() const
{
    return networkReply()->error();
}

QString NetworkResponse::errorString() const
{
    return _reply->errorString();
}

QJsonValue NetworkResponse::json(QString key)
{
    return jsonObject().value(key);
}

QJsonValue NetworkResponse::json()
{

    QMetaType::Type type=static_cast<QMetaType::Type>(_replyData.typeId());
    if(type==QMetaType::QJsonObject)
        return _replyData.toJsonObject();
    else if(type==QMetaType::QJsonArray)
        return _replyData.toJsonArray();
    else
        return QJsonValue();
}

bool NetworkResponse::isJson()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("application/json");
}

bool NetworkResponse::isImage()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("image/");
}

bool NetworkResponse::isText()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("text/");
}

QByteArray NetworkResponse::contentTypeHeader() const
{
    return networkReply()->rawHeader("content-type");
}

const QJsonObject NetworkResponse::jsonObject() const
{
    return _replyData.toJsonObject();
}

const QJsonArray NetworkResponse::jsonArray() const
{
    return _replyData.toJsonArray();
}
#ifdef QT_HAVE_GUI
QImage NetworkResponse::image() const
{
    return _replyData.value<QImage>();
}
#endif

void NetworkResponse::processReply()
{
    QByteArray contentType=contentTypeHeader();
    if(contentType.contains("application/json"))
    {
        QJsonDocument doc=QJsonDocument::fromJson(binaryData());
        if(!doc.isNull())
        {
            if(doc.isArray())
                _replyData=doc.array();
            if(doc.isObject())
                _replyData=doc.object();
        }
        return;
    }
#ifdef QT_HAVE_GUI
    if(contentType.contains("image/"))
    {
        QImage img=QImage::fromData(binaryData());
        if(!img.isNull())
            _replyData=img;
    }
#endif
    if(contentType.contains("text/"))
    {
        _replyData=QString(binaryData());
    }
}


NetworkResponse::operator bool()
{
    return (error()==QNetworkReply::NoError);
}

QDebug operator <<(QDebug dbg, const NetworkResponse &res)
{
        QDebugStateSaver saver(dbg);
        dbg.noquote() <<"Network response: \n";
        dbg.noquote() << "Headers: \n";
        for (const auto &pair : res.networkReply()->rawHeaderPairs())
        {
            dbg.noquote() << pair.first <<" : " << pair.second << "\n";
        }
        dbg.noquote() << "body: \n";
        dbg.noquote() << "status: "      <<      res.status() <<"\n";
        dbg.noquote() << res.binaryData();
        return dbg.noquote();
}

QDebug operator <<(QDebug dbg, const NetworkResponse *res)
{
    return operator <<(dbg,*res);
}
