#include "networkaccessmanager.h"
#include <QNetworkReply>
NetworkAccessManager::NetworkAccessManager(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

NetworkAccessManager *NetworkAccessManager::get(const QNetworkRequest &request)
{
    get(request);
    return this;
}

void NetworkAccessManager::abortAllRequests()
{
    for(QNetworkReply *reply : m_replies){
        reply->abort();
    }
    m_replies.clear();
}

QNetworkRequest NetworkAccessManager::createNetworkRequest(const QUrl &url)
{

}


QNetworkReply *NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=QNetworkAccessManager::createRequest(op,originalReq,outgoingData);

    m_replies << reply;
    if(originalReq.attribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::MonitorProgressAttribute)).toBool()){
        connect(reply,&QNetworkReply::downloadProgress,this,[this,originalReq](qint64 bytesReceived, qint64 bytesTotal){
            int requestId=originalReq.attribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::MonitorProgressAttribute)).toInt();
            emit this->downloadProgress(bytesReceived,bytesTotal,requestId);
        });
    }
    emit networkActivity(originalReq.url());

    return reply;
}
