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
    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);
    _lastUrl=requestUrl;
    _lastRequest=req;
    _lastOperation=QNetworkAccessManager::GetOperation;

    for (const RawHeaderPair & pair : _permanentRawHeaders) {
        req.setRawHeader(pair.first,pair.second);
    }

    manager()->get(req);
    return this;
}


NetworkManager* NetworkManager::post(QString url, QJsonObject object)
{
    QNetworkRequest request;

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    _lastUrl=requestUrl;
    _lastRequest=request;
    _lastOperation=QNetworkAccessManager::PostOperation;
    QJsonDocument doc;
    doc.setObject(object);  
    request.setUrl(requestUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const RawHeaderPair & pair : _permanentRawHeaders) {
        request.setRawHeader(pair.first,pair.second);
    }

    manager()->post(request,doc.toJson());
    return this;
}

NetworkManager *NetworkManager::put(QString url, QJsonObject object)
{
    QNetworkRequest req;
    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    _lastUrl=requestUrl;
    _lastRequest=req;
    _lastOperation=QNetworkAccessManager::PutOperation;
    QJsonDocument doc;
    doc.setObject(object);
    req.setUrl(requestUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    for (const RawHeaderPair & pair : _permanentRawHeaders) {
        req.setRawHeader(pair.first,pair.second);
    }
    manager()->put(req,doc.toJson());
    return this;
}



void NetworkManager::subcribe(Callback cb)
{
    router.registerRoute(_lastOperation,_lastUrl,cb);
}

void NetworkManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    _permanentRawHeaders << RawHeaderPair(headerName,headerValue);
}

void NetworkManager::removeRawHeader(const QByteArray &headerName)
{
    for (const RawHeaderPair &pair : _permanentRawHeaders ) {
        if(pair.first==headerName)
        {
            _permanentRawHeaders.removeOne(pair);
            return;
        }
    }
}

void NetworkManager::setJwtToken(QString token)
{
    _jwtToken=token;
    _rawToken.append(_jwtToken);
}

void NetworkManager::routeReply(QNetworkReply *reply)
{
    NetworkResponse *res=new NetworkResponse(reply);
    router.route(res);
    reply->deleteLater();
    delete res;
}

