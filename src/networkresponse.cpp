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

/*!
    \fn NetworkResponse::NetworkResponse(QNetworkReply *reply, QObject *parent)
    constructs a new NetworkResponse wrapper, if reply is not null, the wrapper will read all the reply's data through \a QNetworkReply::readAll(), parse it and then emits the \a finished() signal.
*/

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

/*!
    \fn QNetworkReply::NetworkError NetworkResponse::error() const
    a wrapper method for \a QNetworkReply::errorOccured()
*/

QNetworkReply::NetworkError NetworkResponse::error() const
{
    return networkReply()->error();
}


/*!
    \fn QString NetworkResponse::errorString() const
    a wrapper method for \a QNetworkReply::errorString()
*/

QString NetworkResponse::errorString() const
{
    return m_reply->errorString();
}

/*!
    \fn QJsonValue NetworkResponse::json(QString key)
    assumes that the response body is a json object and gets the \a QJsonValue associated with key.
*/

QJsonValue NetworkResponse::json(QString key)
{
    return jsonObject().value(key);
}

/*!
    \fn QJsonValue NetworkResponse::json()
    this method will either return a json object or a json array wrapped inside  a \a QJsonValue,
    or a default constructed \a QJsonValue if it were none of the above.
*/


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

/*!
    \fn QByteArray NetworkResponse::binaryData() const

    returns the response body as obtained from \a QNetworkReply::readAll().
*/


QByteArray NetworkResponse::binaryData() const
{
    return m_binaryData;
}

/*!
    \fn QVariant NetworkResponse::data() const

    returns the response body as Qvariant object if the reply's content-type is supported and valid, otherwise returns a default constructed \a QVariant.
*/

QVariant NetworkResponse::data() const
{
    return m_replyData;
}

/*!
    \fn bool NetworkResponse::isJson()

    this method determines wether the content-type header contains the string literal "application/json", in a case insensitive manner.
*/


bool NetworkResponse::isJson()
{
    return contentTypeHeader().contains(QStringLiteral("application/json"),Qt::CaseInsensitive);
}

/*!
    \fn bool NetworkResponse::isImage()

    this method determines wether the content-type header contains the string literal "image/", in a case insensitive manner.
*/

bool NetworkResponse::isImage()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("image/",Qt::CaseInsensitive);
}

/*!
    \fn bool NetworkResponse::isText()

    this method determines wether the content-type header contains the string literal "text/", in a case insensitive manner.
*/


bool NetworkResponse::isText()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("text/",Qt::CaseInsensitive);
}

/*!
    \fn QString NetworkResponse::contentTypeHeader() const

    returns the content-type header as a QString
*/

QString NetworkResponse::contentTypeHeader() const
{
    return QString(networkReply()->rawHeader("content-type"));
}

/*!
    \fn QUrl NetworkResponse::url() const

    this is a wrapper method for \a QNetworkReply::url().
*/


QUrl NetworkResponse::url() const
{
    return m_reply->url();
}

/*!
    \fn QNetworkAccessManager::Operation NetworkResponse::operation() const

    this is a wrapper method for \a QNetworkReply::operation().
*/

QNetworkAccessManager::Operation NetworkResponse::operation() const
{
    return m_reply->operation();
}

/*!
    \fn QByteArray NetworkResponse::rawHeader(const QByteArray &headerName) const

    this is a wrapper method for \a QNetworkReply::rawHeader().
*/

QByteArray NetworkResponse::rawHeader(const QByteArray &headerName) const
{
    return m_reply->rawHeader(headerName);
}

/*!
    \fn QVariant NetworkResponse::attribute(QNetworkRequest::Attribute code) const

    this is a wrapper method for \a QNetworkReply::attribute().
*/

QVariant NetworkResponse::attribute(QNetworkRequest::Attribute code) const
{
    return m_reply->attribute(code);
}

/*!
    \fn int NetworkResponse::status() const

    returns the http response status code, it is the equivelant of calling QNetworkReply::attribute(QNetworkRequest::HttpStatusCodeAttribute).
*/

int NetworkResponse::status() const
{
    return m_reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

/*!
    \fn QNetworkReply *NetworkResponse::networkReply() const

    returns a pointer to the wrapped QNetworkReply instance.
*/


QNetworkReply *NetworkResponse::networkReply() const
{
    return m_reply;
}

/*!
    \fn QJsonObject NetworkResponse::jsonObject() const

    returns the response body as a \a QJsonObject, a default constructed json object will be returned in case it is invalid.
*/

const QJsonObject NetworkResponse::jsonObject() const
{
    return m_replyData.toJsonObject();
}

/*!
    \fn QJsonArray NetworkResponse::jsonArray() const

    returns the response body as a \a QJsonArray, a default constructed json array will be returned in case it is invalid.
*/

const QJsonArray NetworkResponse::jsonArray() const
{
    return m_replyData.toJsonArray();
}

/*!
    \fn void NetworkResponse::waitForFinished()
    this method creates a \a QEventLoop instance that quits when the response emits the \a finished() signal
*/

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
    QString contentType=contentTypeHeader();
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

/*!
    \fn NetworkResponse::operator bool() const
    this is a convenience method, it is the equivelant of "error()==QNetworkReply::NoError".
*/

NetworkResponse::operator bool() const
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

/*!
    \fn NetworkResponse * NetworkResponse::subcribe(std::function<void (NetworkResponse *)> cb)
    routes the \a NetworkResponse object to  parameter cb when finished.
*/

NetworkResponse * NetworkResponse::subcribe(std::function<void (NetworkResponse *)> cb)
{

    NetworkAccessManager *manager = qobject_cast<NetworkAccessManager *>(parent());
    if(manager){
        manager->registerRoute(this,cb);
        connect(this,&NetworkResponse::finished,manager,[this,manager](){manager->route(this);});
    }
    return this;
}


