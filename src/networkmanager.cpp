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
NetworkManager::NetworkManager(QObject *parent) : QObject (parent),_attempts(1)
{
    QObject::connect(&m_manager,&QNetworkAccessManager::finished,this,&NetworkManager::routeReply);
    QObject::connect(&synchronousManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);

    QObject::connect(&m_manager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);
    QObject::connect(&synchronousManager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);

    QObject::connect(&m_manager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);
    QObject::connect(&synchronousManager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);

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
        reply= synchronousManager.get(createRequest(url));

        if(isIgnoringSslErrors())
            reply->ignoreSslErrors();

        eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

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
        reply= synchronousManager.post(request,rawData(data));
        if(isIgnoringSslErrors())
            reply->ignoreSslErrors();

        eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

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
        reply= synchronousManager.put(request,rawData(data));

        if(isIgnoringSslErrors())
            reply->ignoreSslErrors();

        eventLoop.exec();
        if(reply->error()==QNetworkReply::NoError){
            break;
        }
        else{
            attemps++;
        }
    }
    while(attemps<=attemptsCount());

    return NetworkResponse(reply);
}



void NetworkManager::subcribe(Callback cb)
{
    router.registerRoute(_lastReply,cb);
}

void NetworkManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    permanentRawHeaders()[headerName]=headerValue;
}

void NetworkManager::removeRawHeader(const QByteArray &headerName)
{
    permanentRawHeaders().remove(headerName);
}

bool NetworkManager::isIgnoringSslErrors() const
{
    return _ignoreSslErrors;

}

void NetworkManager::ignoreSslErrors(bool ignore)
{
    _ignoreSslErrors = ignore;
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
    synchronousManager.connectToHostEncrypted(hostName,port,sslConfiguration);
}
#endif

#if QT_VERSION >=QT_VERSION_CHECK(5,15,0)
void NetworkManager::setTransferTimeout(int timeout)
{
    m_manager.setTransferTimeout(timeout);
    synchronousManager.setTransferTimeout(timeout);
}
#endif

QNetworkRequest NetworkManager::createRequest(const QString &url)
{
    QNetworkRequest req;

    if(redirectAllowed()){
        req.setAttribute(QNetworkRequest::RedirectionTargetAttribute,true);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::SameOriginRedirectPolicy);
    }

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);

    for (const QByteArray & headerName : permanentRawHeaders().keys()) {
        req.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }

    emit networkActivity(url);

    return  req;
}

QByteArray NetworkManager::mapContentType(const QVariant::Type type)
{
    if(permanentRawHeaders().contains("content-type")) //if this header already exists then return it to avoid conflicts
        return permanentRawHeaders()["content-type"];

    QByteArray contentType;
    //QMetaType::Type::Type type=static_cast<QMetaType::Type::Type>(data.type());
    switch (type) {
    case QMetaType::Type::QJsonObject  :
    case QMetaType::Type::QJsonValue   :
    case QMetaType::Type::QJsonArray   :
    case QMetaType::Type::QJsonDocument: contentType = "application/json"; break;
    case QMetaType::Type::QImage       : contentType = "image/png";        break;
    case QMetaType::Type::QString      : contentType = "text/plain";       break;

    default                      :                                   break;
    }
    return contentType;
}

QByteArray NetworkManager::rawData(const QVariant &data)
{
    QMetaType::Type type=(QMetaType::Type)data.type();

    /**************************json**************************/
    if(type==QMetaType::Type::QJsonObject)
    {
        QJsonObject object=data.toJsonObject();
        QJsonDocument document;
        document.setObject(object);
        return document.toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::Type::QJsonArray)
    {
        QJsonArray array=data.toJsonArray();
        QJsonDocument document;
        document.setArray(array);
        return document.toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::Type::QJsonDocument)
    {
        return data.toJsonDocument().toJson(QJsonDocument::Compact);
    }

    if(type==QMetaType::Type::QJsonValue)
    {
        QJsonValue jsonValue=data.value<QJsonValue>();

        if(jsonValue.type()==QJsonValue::Type::Array)
        {
            QJsonArray array=jsonValue.toArray();
            QJsonDocument document;
            document.setArray(array);
            return document.toJson(QJsonDocument::Compact);
        }

        if(jsonValue.type()==QJsonValue::Type::Object)
        {
            QJsonObject object=jsonValue.toObject();
            QJsonDocument document;
            document.setObject(object);
            return document.toJson(QJsonDocument::Compact);
        }
        if(jsonValue.type()==QJsonValue::Type::String)
        {
            return jsonValue.toString().toUtf8();
        }
        if(jsonValue.type()==QJsonValue::Type::Double)
        {
            return QString::number(jsonValue.toDouble()).toUtf8();
        }
        if(jsonValue.type()==QJsonValue::Type::Bool)
        {
            return jsonValue.toBool() ? QByteArray("1") : QByteArray("0");
        }
        if(jsonValue.type()==QJsonValue::Type::Null){
            return QByteArray("");
        }
        if(jsonValue.type()==QJsonValue::Type::Undefined){
            return QByteArray();
        }

    }

    /**********************end json**************************/

    if(type==QMetaType::Type::QString)
        return data.toString().toUtf8();

    if(type==QMetaType::Type::Int)
        return QString::number(data.toInt()).toUtf8();

    if(type==QMetaType::Type::Double)
        return QString::number(data.toDouble()).toUtf8();

    if(type==QMetaType::Type::Float)
        return QString::number(data.toFloat()).toUtf8();

    if(type==QMetaType::Type::Long || type==QMetaType::Type::LongLong)
        return QString::number(data.toLongLong()).toUtf8();

    if(type==QMetaType::Type::UInt)
        return QString::number(data.toUInt()).toUtf8();

    if(type==QMetaType::Type::ULongLong)
        return QString::number(data.toULongLong()).toUtf8();

    if(type==QMetaType::Type::QByteArray)
        return data.toByteArray();
#ifdef QT_HAVE_GUI
    if(type==QMetaType::Type::QImage)
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


    if(type==QMetaType::Type::UnknownType)
        return QByteArray();

    qDebug()<<"NetworkManager::rawData : unsupported QVariant type: " << data.type();

    return QByteArray();
}

QNetworkReply *NetworkManager::lastReply() const
{
    return _lastReply;
}

void NetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
{
    Q_UNUSED(reply);
    authenticator->setUser(authenticationCredentials.first);
    authenticator->setPassword(authenticationCredentials.second);
}

void NetworkManager::setLastReply(QNetworkReply *reply)
{
    _lastReply=reply;

    if(isIgnoringSslErrors())
        _lastReply->ignoreSslErrors();
}


int NetworkManager::attemptsCount() const
{
    return _attempts;
}

void NetworkManager::setAttemptsCount(int attempts)
{
    if(attempts<1)
        return;
    _attempts = attempts;
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
    synchronousManager.setConfiguration(config);
}

QNetworkConfiguration NetworkManager::configuration() const
{
    return m_manager.configuration();
}
#endif
void NetworkManager::setProxy(const QNetworkProxy &proxy)
{
    m_manager.setProxy(proxy);
    synchronousManager.setProxy(proxy);
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
    router.route(response);
    reply->deleteLater();
    delete response;
}



