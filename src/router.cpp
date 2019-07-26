#include "router.h"
#include <QNetworkAccessManager>

Router::Router()
{

}

void Router::route(NetworkResponse *reply)
{
    MemberCallbacks *callBacks;

    QNetworkAccessManager::Operation op=reply->operation();
    switch(op)
    {
    case QNetworkAccessManager::GetOperation:    callBacks=&getRoutes    ; break;
    case QNetworkAccessManager::PostOperation:   callBacks=&postRoutes   ; break;
    case QNetworkAccessManager::PutOperation:    callBacks=&putRoutes    ; break;
    case QNetworkAccessManager::DeleteOperation: callBacks=&DeleteRoutes ; break;
    case QNetworkAccessManager::HeadOperation:   callBacks=&headerhRoutes; break;
    default: qDebug()<<"invalid operation";                               return;
    }

    QString url=reply->networkReply()->request().url().toString(); //url should be the original request url in case of url redirection !
    if(callBacks->contains(url))
    {
        MemberCallbackInfo cb= callBacks->value(url);
        cb.instance ? (cb.instance->*cb.ptr)(reply) : cb.callback(reply);
    }
}






