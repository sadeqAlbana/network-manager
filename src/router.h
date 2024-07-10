/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

#ifndef ROUTER_H
#define ROUTER_H

#include <functional>
#include <QMap>
class NetworkResponse;


template<class T> using MemberCallback = void (T::*)(NetworkResponse *);

using Callback = std::function<void (NetworkResponse *)>;


typedef QMap<NetworkResponse *,Callback> MemberCallbacks;
namespace SNetworkManager{
class Router
{
public:
    Router();
    ~Router();
    void route(NetworkResponse *reply);
    void registerRoute(NetworkResponse *reply, Callback cb);

    template <class T>
    void registerRoute(NetworkResponse *reply, T *instance, MemberCallback<T> ptr)
    {
        Callback cb = std::bind(ptr,instance,std::placeholders::_1);
        callbacks.insert(reply, cb);
    }

    void removeRoute(NetworkResponse *reply);


protected:
    MemberCallbacks callbacks;
};

}


#endif // ROUTER_H
