#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include "networkresponse.h"
#include <functional>


#ifdef Q_CC_MSVC
#pragma pointers_to_members(full_generality,virtual_inheritance)
#endif

class CNTRLR{}; //MSVC will throw C2440 when using reinterpret_cast with undefined classes

template<class T> using MemberCallback = void (T::*)(NetworkResponse *);

using Callback = std::function<void (NetworkResponse *)>;

struct MemberCallbackInfo
{
    CNTRLR *instance;
    MemberCallback<CNTRLR> ptr;
    Callback callback;
};

typedef QMap<QNetworkReply *,MemberCallbackInfo> MemberCallbacks;

class Router
{
public:
    explicit Router();

    void route(NetworkResponse *reply);

    template <class T>
    void registerRoute(QNetworkReply *reply, T *instance, MemberCallback<T> ptr)
    {
        MemberCallbackInfo obj={reinterpret_cast<CNTRLR *>(instance),reinterpret_cast<MemberCallback<CNTRLR>>(ptr),0};
        callbacks.insert(reply, obj);
    }

    void registerRoute(QNetworkReply *reply, Callback cb)
    {
        MemberCallbackInfo obj={0,0,cb};
        callbacks.insert(reply, obj);
    }

private:
    MemberCallbacks callbacks;
};

typedef Router Route;

#endif // ROUTER_H
