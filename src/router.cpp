#include "router.h"
#include <QNetworkAccessManager>

Router::Router()
{

}

void Router::route(NetworkResponse *reply)
{
    if(callbacks.contains(reply->networkReply()))
    {
        MemberCallbackInfo cb= callbacks.value(reply->networkReply());
        cb.instance ? (cb.instance->*cb.ptr)(reply) : cb.callback(reply);
    }
}






