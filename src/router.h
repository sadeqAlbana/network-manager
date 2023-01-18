/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <functional>
#include <QMap>
#include <QDebug>
class NetworkResponse;


template<class T> using MemberCallback = void (T::*)(NetworkResponse *);

using Callback = std::function<void (NetworkResponse *)>;



typedef QMap<NetworkResponse *,Callback> MemberCallbacks;
namespace SNetworkManager{
class Router
{
public:
    explicit Router(){}

    void route(NetworkResponse *reply)
    {
        if(callbacks.contains(reply))
        {
            Callback cb=callbacks[reply];
            cb(reply);
        }
    }

    template <class T>
    void registerRoute(NetworkResponse *reply, T *instance, MemberCallback<T> ptr)
    {
        Callback cb = std::bind(ptr,instance,std::placeholders::_1);
        callbacks.insert(reply, cb);

    }

    void registerRoute(NetworkResponse *reply, Callback cb)
    {
        callbacks.insert(reply, cb);
    }

private:
    MemberCallbacks callbacks;
};

}


#endif // ROUTER_H
