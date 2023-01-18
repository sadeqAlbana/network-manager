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
#include "networkaccessmanager.h"
#include <QEventLoop>
NetworkResponse::NetworkResponse(QNetworkReply *reply, QObject *parent): QObject(parent),
    m_reply(reply)
{

    if(!reply)
        return;

    QObject::connect(reply,&QNetworkReply::finished,this,&NetworkResponse::onReplyFinished);
}

NetworkResponse::~NetworkResponse()
{
    m_reply->deleteLater();
}

QNetworkReply::NetworkError NetworkResponse::error() const
{
    return networkReply()->error();
}

QString NetworkResponse::errorString() const
{
    return m_reply->errorString();
}

QJsonValue NetworkResponse::json(QString key)
{
    return jsonObject().value(key);
}

QJsonValue NetworkResponse::json()
{

    QMetaType::Type type=static_cast<QMetaType::Type>(m_replyData.typeId());
    if(type==QMetaType::QJsonObject)
        return m_replyData.toJsonObject();
    else if(type==QMetaType::QJsonArray)
        return m_replyData.toJsonArray();
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
    return m_replyData.toJsonObject();
}

const QJsonArray NetworkResponse::jsonArray() const
{
    return m_replyData.toJsonArray();
}

void NetworkResponse::waitForFinished()
{
    QEventLoop eventLoop;

    connect(this,&NetworkResponse::finished,&eventLoop,&QEventLoop::quit);

    eventLoop.exec();
}
#ifdef QT_HAVE_GUI
QImage NetworkResponse::image() const
{
    return m_replyData.value<QImage>();
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
                m_replyData=doc.array();
            if(doc.isObject())
                m_replyData=doc.object();
        }
        return;
    }
#ifdef QT_HAVE_GUI
    if(contentType.contains("image/"))
    {
        QImage img=QImage::fromData(binaryData());
        if(!img.isNull())
            m_replyData=img;
    }
#endif
    if(contentType.contains("text/"))
    {
        m_replyData=QString(binaryData());
    }
}

void NetworkResponse::onReplyFinished()
{
    if(this->m_reply->error()!=QNetworkReply::OperationCanceledError && this->m_reply->error()!=QNetworkReply::TimeoutError)
    {
        m_binaryData=this->m_reply->readAll();
        processReply();
    }

    emit finished();
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


NetworkResponse * NetworkResponse::subcribe(std::function<void (NetworkResponse *)> cb)
{

    NetworkAccessManager *manager = qobject_cast<NetworkAccessManager *>(parent());
    if(manager){
        manager->registerRoute(this,cb);
        connect(this,&NetworkResponse::finished,manager,[this,manager](){manager->route(this);});
    }
    return this;
}


