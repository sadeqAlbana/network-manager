#include "networkmanager.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
NetworkManager::NetworkManager(QObject *parent) : QObject (parent)
{
    QObject::connect(&m_manager,&QNetworkAccessManager::finished,this,&NetworkManager::routeReply);
    QObject::connect(&synchronousManager,&QNetworkAccessManager::finished,&eventLoop,&QEventLoop::quit);
}

NetworkManager* NetworkManager::get(QString url)
{
    setLastReply(manager()->get(createRequest(url)));
    return this;
}

NetworkManager* NetworkManager::post(QString url, QJsonObject object)
{
    QJsonDocument doc;
    doc.setObject(object);  
    QNetworkRequest request = createRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    setLastReply(manager()->post(request,doc.toJson()));
    return this;
}

NetworkManager *NetworkManager::put(QString url, QJsonObject object)
{
    QJsonDocument doc;
    doc.setObject(object);
    QNetworkRequest request = createRequest(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");
    setLastReply(manager()->put(request,doc.toJson()));
    return this;
}

NetworkResponse *NetworkManager::getSynch(QString url)
{
    QNetworkReply *reply= synchronousManager.get(createRequest(url));
    eventLoop.exec();
    return new NetworkResponse(reply);
}

NetworkResponse *NetworkManager::postSynch(QString url, QVariant data)
{
    QNetworkRequest request=createRequest(url);

    QJsonDocument doc;
    doc.setObject(data.toJsonObject());
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    QNetworkReply *reply= synchronousManager.post(request,doc.toJson());
    eventLoop.exec();
    return new NetworkResponse(reply);
}

NetworkResponse *NetworkManager::putSynch(QString url, QVariant data)
{
    QNetworkRequest request=createRequest(url);

    QJsonDocument doc;
    QByteArray payload=rawData(data);
    request.setHeader(QNetworkRequest::ContentTypeHeader,mapContentType(data));

    QNetworkReply *reply= synchronousManager.put(request,payload);
    eventLoop.exec();
    return new NetworkResponse(reply);
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

QNetworkRequest NetworkManager::createRequest(const QString &url)
{
    QNetworkRequest req;

    if(redirectAllowed()){
        req.setAttribute(QNetworkRequest::RedirectionTargetAttribute,true);
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::SameOriginRedirectPolicy);
    }

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);

    for (const QByteArray & headerName : permanentRawHeaders()) {
        req.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }

    return  req;
}

QByteArray NetworkManager::mapContentType(const QVariant &data)
{
    QByteArray contentType;
    QMetaType::Type type=static_cast<QMetaType::Type>(data.type());
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

    return QByteArray();
}

void NetworkManager::routeReply(QNetworkReply *reply)
{
    NetworkResponse *response=new NetworkResponse(reply);
    router.route(response);
    reply->deleteLater();
    delete response;
}



