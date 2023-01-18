#include "router.h"
#include "networkresponse.h"
SNetworkManager::Router::Router()
{

}

SNetworkManager::Router::~Router()
{

}

void SNetworkManager::Router::route(NetworkResponse *reply)
{
    if(callbacks.contains(reply))
    {
        Callback cb=callbacks[reply];
        cb(reply);
    }
    reply->deleteLater();
}

void SNetworkManager::Router::registerRoute(NetworkResponse *reply, Callback cb)
{
    callbacks.insert(reply, cb);
}

