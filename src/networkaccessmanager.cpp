#include "networkaccessmanager.h"
#include <QNetworkReply>
NetworkAccessManager::NetworkAccessManager(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

void NetworkAccessManager::abortAllRequets()
{
    for(QNetworkReply *reply : m_replies){
        reply->abort();
    }
    m_replies.clear();
}


QNetworkReply *NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=QNetworkAccessManager::createRequest(op,originalReq,outgoingData);
    m_replies << reply;
    emit networkActivity(originalReq.url());

    return reply;
}
