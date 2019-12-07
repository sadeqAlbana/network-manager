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

    setLastReply(manager()->get(req));

    return this;
}


NetworkManager* NetworkManager::post(QString url, QJsonObject object)
{
    QNetworkRequest request;

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    QJsonDocument doc;
    doc.setObject(object);  
    request.setUrl(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const QByteArray & headerName : permanentRawHeaders()) {
        request.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }
    setLastReply(manager()->post(request,doc.toJson()));
    return this;
}

NetworkManager *NetworkManager::put(QString url, QJsonObject object)
{
    QNetworkRequest req;
    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);
    QJsonDocument doc;
    doc.setObject(object);
    req.setUrl(requestUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const QByteArray & headerName : permanentRawHeaders()) {
        req.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }
    setLastReply(manager()->put(req,doc.toJson()));
    return this;
}

NetworkResponse *NetworkManager::getSynch(QString url)
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

    QNetworkReply *reply= synchronousManager.get(req);
    eventLoop.exec();
    return new NetworkResponse(reply);
}

NetworkResponse *NetworkManager::postSynch(QString url, QJsonObject object)
{
    QNetworkRequest request;

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    QJsonDocument doc;
    doc.setObject(object);
    request.setUrl(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const QByteArray & headerName : permanentRawHeaders()) {
        request.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }
    QNetworkReply *reply= synchronousManager.post(request,doc.toJson());
    return new NetworkResponse(reply);
}

NetworkResponse *NetworkManager::putSynch(QString url, QJsonObject object)
{
    QNetworkRequest request;

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    QJsonDocument doc;
    doc.setObject(object);
    request.setUrl(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const QByteArray & headerName : permanentRawHeaders()) {
        request.setRawHeader(headerName,permanentRawHeaders()[headerName]);
    }
    QNetworkReply *reply= synchronousManager.put(request,doc.toJson());
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

void NetworkManager::routeReply(QNetworkReply *reply)
{
    NetworkResponse *response=new NetworkResponse(reply);
    router.route(response);
    reply->deleteLater();
    delete response;
}



