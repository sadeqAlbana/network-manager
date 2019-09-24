#include "networkmanager.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
NetworkManager::NetworkManager(QObject *parent) : QObject (parent)
{
    QObject::connect(&m_manager,&QNetworkAccessManager::finished,this,&NetworkManager::routeReply);

}

NetworkManager* NetworkManager::get(QString url)
{
    QNetworkRequest req;
    req.setAttribute(QNetworkRequest::RedirectionTargetAttribute,true);
    req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::SameOriginRedirectPolicy);
    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);
    setLastOperation(QNetworkAccessManager::GetOperation);

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
    request.setUrl(requestUrl);
    setLastOperation(QNetworkAccessManager::PostOperation);
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
    setLastOperation(QNetworkAccessManager::PutOperation);
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

void NetworkManager::setJwtToken(QByteArray token)
{
    permanentRawHeaders()["authorization"]=token;
}

void NetworkManager::routeReply(QNetworkReply *reply)
{
    NetworkResponse *response=new NetworkResponse(reply);
    router.route(response);
    reply->deleteLater();
    delete response;
}



