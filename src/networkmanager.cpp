#include "networkmanager.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
NetworkManager::NetworkManager()
{

}

NetworkManager* NetworkManager::get(QString url)
{
    QNetworkRequest req;
    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    req.setUrl(requestUrl);
    _lastUrl=requestUrl;
    _lastRequest=req;
    _lastOperation=QNetworkAccessManager::GetOperation;

    if(!_jwtToken.isEmpty())
        req.setRawHeader(QByteArray("Authorization"),_rawToken);

    manager()->get(req);
    return this;
}


NetworkManager* NetworkManager::post(QString url, QJsonObject object)
{
    QNetworkRequest req;

    QString requestUrl= usingBaseUrl() ? baseUrl+url : url;
    _lastUrl=requestUrl;
    _lastRequest=req;
    _lastOperation=QNetworkAccessManager::PostOperation;
    QJsonDocument doc;
    doc.setObject(object);  
    req.setUrl(requestUrl);
    req.setHeader(QNetworkRequest::ContentTypeHeader,"application/json");

    if(!_jwtToken.isEmpty())
        req.setRawHeader(QByteArray("Authorization"),_rawToken);

    manager()->post(req,doc.toJson());
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

    if(!_jwtToken.isEmpty())
        req.setRawHeader(QByteArray("Authorization"),_rawToken);

    manager()->put(req,doc.toJson());
    return this;
}



void NetworkManager::subcribe(Callback cb)
{
    router.registerRoute(_lastOperation,_lastUrl,cb);
}

void NetworkManager::setJwtToken(QString token)
{
    _jwtToken=token;
    _rawToken.append(_jwtToken);
}

