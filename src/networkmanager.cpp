/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#include "networkmanager.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#ifdef QT_HAVE_GUI
#include <QImage>
#endif
#include <QJsonArray>
#include <QJsonValue>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QNetworkConfiguration>
NetworkManager::NetworkManager(QObject *parent) : QObject (parent),m_attempts(1)
{
    QObject::connect(&m_manager,&QNetworkAccessManager::finished,this,&NetworkManager::routeReply);
    QObject::connect(&m_synchronousManager,&QNetworkAccessManager::finished,&m_eventLoop,&QEventLoop::quit);

    QObject::connect(&m_manager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);
    QObject::connect(&m_synchronousManager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);

    QObject::connect(&m_manager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);
    QObject::connect(&m_synchronousManager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);

}


NetworkManager* NetworkManager::get(QString url)
{
    setLastReply(manager()->get(createRequest(url)));
    return this;
}

NetworkManager* NetworkManager::post(const QString url, const QVariant data, QByteArray contentType)
{
    QNetworkRequest request = createRequest(url);
    if(contentType.isNull())
        contentType=mapContentType(data.type());

    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
    setLastReply(manager()->post(request,rawData(data)));
    return this;
}

NetworkManager *NetworkManager::put(const QString url, const QVariant data, QByteArray contentType)
{
    QNetworkRequest request = createRequest(url);

    if(contentType.isNull())
        contentType=mapContentType(data.type());

    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
    setLastReply(manager()->put(request,rawData(data)));

    return this;
}

NetworkResponse NetworkManager::getSynch(QString url)
{
    QNetworkReply *reply;
    int attemps=1;
    do{
        reply= m_synchronousManager.get(createRequest(url));

        if(isIgnoringSslErrors())
            reply->ignoreSslErrors();

        m_eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError || !isConnectionError(reply->error())){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

    emit finishedNetworkActivity(reply->url().toString());

    return NetworkResponse(reply);
}

NetworkResponse NetworkManager::postSynch(const QString url, const QVariant data, QByteArray contentType)
{
    QNetworkRequest request=createRequest(url);

    if(contentType.isNull())
        contentType=mapContentType(data.type());

    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
    QNetworkReply *reply;
    int attemps=1;
    do{
        reply= m_synchronousManager.post(request,rawData(data));
        if(isIgnoringSslErrors())
            reply->ignoreSslErrors();

        m_eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError || !isConnectionError(reply->error())){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

    emit finishedNetworkActivity(reply->url().toString());

    return NetworkResponse(reply);
}

NetworkResponse NetworkManager::putSynch(const QString url, const QVariant data, QByteArray contentType)
{
    QNetworkRequest request=createRequest(url);

    if(contentType.isNull())
        contentType=mapContentType(data.type());

    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
    QNetworkReply *reply;
    int attemps=1;
    do{
        reply= m_synchronousManager.put(request,rawData(data));
        if(isIgnoringSslErrors() || !isConnectionError(reply->error()))
            reply->ignoreSslErrors();

        m_eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

    emit finishedNetworkActivity(reply->url().toString());
    return NetworkResponse(reply);
}

bool NetworkManager::isConnectionError(QNetworkReply::NetworkError error)
{
    switch (error) {
    case QNetworkReply::ConnectionRefusedError: return true;
    case QNetworkReply::RemoteHostClosedError: return true;
    case QNetworkReply::HostNotFoundError: return true;
    case QNetworkReply::TimeoutError: return true;
    case QNetworkReply::NetworkSessionFailedError: return true;
    default: return false;
    }
}



void NetworkManager::subcribe(Callback cb)
{
    m_router.registerRoute(m_lastReply,cb);
}

void NetworkManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    m_permanentRawHeaders[headerName]=headerValue;
}

void NetworkManager::removeRawHeader(const QByteArray &headerName)
{
    m_permanentRawHeaders.remove(headerName);
}

bool NetworkManager::isIgnoringSslErrors() const
{
    return m_ignoreSslErrors;

}

void NetworkManager::ignoreSslErrors(bool ignore)
{
    m_ignoreSslErrors = ignore;
}

void NetworkManager::onSSLError(QNetworkReply *reply, const QList<QSslError> &errors)
{
    Q_UNUSED(errors)
    reply->ignoreSslErrors();
}
#ifndef QT_NO_SSL
void NetworkManager::connectToHostEncrypted(const QString &hostName, quint16 port, const QSslConfiguration &sslConfiguration)
{
    manager()->connectToHostEncrypted(hostName,port,sslConfiguration);
    m_synchronousManager.connectToHostEncrypted(hostName,port,sslConfiguration);
}
#endif

#if QT_VERSION >=QT_VERSION_CHECK(5,15,0)
void NetworkManager::setTransferTimeout(int timeout)
{
    m_manager.setTransferTimeout(timeout);
    m_synchronousManager.setTransferTimeout(timeout);
}
#endif

QNetworkRequest NetworkManager::createRequest(const QString &url)
{
    QNetworkRequest req;

    if(redirectAllowed()){
        req.setAttribute(QNetworkRequest::RedirectionTargetAttribute,true);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::SameOriginRedirectPolicy);
    }

    QString requestUrl= usingBaseUrl() ? m_baseUrl+url : url;
    req.setUrl(requestUrl);

    for (const QByteArray & headerName : m_permanentRawHeaders.keys()) {
        req.setRawHeader(headerName,m_permanentRawHeaders[headerName]);
    }

    emit networkActivity(url);

    return  req;
}

QByteArray NetworkManager::mapContentType(const QVariant::Type type)
{
    if(m_permanentRawHeaders.contains("content-type")) //if this header already exists then return it to avoid conflicts
        return m_permanentRawHeaders["content-type"];

    QByteArray contentType;
    //QMetaType::Type type=static_cast<QMetaType::Type>(data.type());
    switch (type) {
    case QMetaType::QJsonObject  :
    case QMetaType::QJsonValue   :
    case QMetaType::QJsonArray   :
    case QMetaType::QJsonDocument: contentType = "application/json"; break;
    case QMetaType::QImage       : contentType = "image/png";        break;
    case QMetaType::QString      : contentType = "text/plain";       break;

    default                      :                                   break;
    }
    return contentType;
}

QByteArray NetworkManager::rawData(const QVariant &data)
{
    QMetaType::Type type=static_cast<QMetaType::Type>(data.type());

    /**************************json**************************/
    if(type==QMetaType::QJsonObject)
    {
        QJsonObject object=data.toJsonObject();
        QJsonDocument document;
        document.setObject(object);
        return document.toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::QJsonArray)
    {
        QJsonArray array=data.toJsonArray();
        QJsonDocument document;
        document.setArray(array);
        return document.toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::QJsonDocument)
    {
        return data.toJsonDocument().toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::QJsonValue)
    {
        QJsonValue jsonValue=data.toJsonValue();

        if(jsonValue.type()==QJsonValue::Array)
        {
            QJsonArray array=jsonValue.toArray();
            QJsonDocument document;
            document.setArray(array);
            return document.toJson(QJsonDocument::Compact);
        }

        if(jsonValue.type()==QJsonValue::Object)
        {
            QJsonObject object=jsonValue.toObject();
            QJsonDocument document;
            document.setObject(object);
            return document.toJson(QJsonDocument::Compact);
        }
        if(jsonValue.type()==QJsonValue::String)
        {
            return jsonValue.toString().toUtf8();
        }
        if(jsonValue==QJsonValue::Double)
        {
            return QString::number(jsonValue.toDouble()).toUtf8();
        }
        if(jsonValue.type()==QJsonValue::Bool)
        {
            return jsonValue.toBool() ? QByteArray("1") : QByteArray("0");
        }

    }

    /**********************end json**************************/

    if(type==QMetaType::QString)
        return data.toString().toUtf8();

    if(type==QMetaType::Int)
        return QString::number(data.toInt()).toUtf8();

    if(type==QMetaType::Double)
        return QString::number(data.toDouble()).toUtf8();

    if(type==QMetaType::Float)
        return QString::number(data.toFloat()).toUtf8();

    if(type==QMetaType::Long || type==QMetaType::LongLong)
        return QString::number(data.toLongLong()).toUtf8();

    if(type==QMetaType::UInt)
        return QString::number(data.toUInt()).toUtf8();

    if(type==QMetaType::ULongLong)
        return QString::number(data.toULongLong()).toUtf8();

    if(type==QMetaType::QByteArray)
        return data.toByteArray();
#ifdef QT_HAVE_GUI
    if(type==QMetaType::QImage)
    {
        QImage image=data.value<QImage>();
        QByteArray imageData;
        QBuffer buffer(&imageData);
        buffer.open(QIODevice::WriteOnly);
        image.save(&buffer,"PNG");
        buffer.close();
        return imageData;
    }
#endif


    qDebug()<<"NetworkManager::rawData : unsupported QVariant type";

    return QByteArray();
}

QNetworkReply *NetworkManager::lastReply() const
{
    return m_lastReply;
}

void NetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    authenticator->setUser(authenticationCredentials.first);
    authenticator->setPassword(authenticationCredentials.second);
}

void NetworkManager::setLastReply(QNetworkReply *reply)
{
    m_lastReply=reply;

    if(isIgnoringSslErrors())
        m_lastReply->ignoreSslErrors();
}


int NetworkManager::attemptsCount() const
{
    return m_attempts;
}

void NetworkManager::setAttemptsCount(int attempts)
{
    if(attempts<1)
        return;
    m_attempts = attempts;
}

void NetworkManager::setAuthenticationCredentails(const QString &user, const QString &password)
{
    authenticationCredentials.first=user;
    authenticationCredentials.second=password;
}
#if !defined(QT_NO_BEARERMANAGEMENT)
void NetworkManager::setConfiguration(const QNetworkConfiguration &config)
{
    m_manager.setConfiguration(config);
    m_synchronousManager.setConfiguration(config);
}

QNetworkConfiguration NetworkManager::configuration() const
{
    return m_manager.configuration();
}
#endif
void NetworkManager::setProxy(const QNetworkProxy &proxy)
{
    m_manager.setProxy(proxy);
    m_synchronousManager.setProxy(proxy);
}

QNetworkProxy NetworkManager::proxy() const
{
    return m_manager.proxy();
}

void NetworkManager::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy);
    authenticator->setUser(proxyAuthenticationCredentials.first);
    authenticator->setPassword(proxyAuthenticationCredentials.second);
}

void NetworkManager::routeReply(QNetworkReply *reply)
{   
    emit finishedNetworkActivity(reply->url().toString());
    NetworkResponse *response=new NetworkResponse(reply);
    m_router.route(response);
    reply->deleteLater();
    delete response;
}



