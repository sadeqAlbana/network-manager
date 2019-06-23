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

typedef QMap<QString,MemberCallbackInfo> MemberCallbacks;

class Router
{
public:
    explicit Router();

    void route(NetworkResponse *reply);

    template <class T>
    void registerRoute(QNetworkAccessManager::Operation op,const QString& action, T *instance, MemberCallback<T> ptr)
    {
        MemberCallbacks *callBacks;
        switch(op)
        {
        case QNetworkAccessManager::GetOperation:    callBacks=&getRoutes    ; break;
        case QNetworkAccessManager::PostOperation:   callBacks=&postRoutes   ; break;
        case QNetworkAccessManager::PutOperation:    callBacks=&putRoutes    ; break;
        case QNetworkAccessManager::DeleteOperation: callBacks=&DeleteRoutes ; break;
        case QNetworkAccessManager::HeadOperation:   callBacks=&headerhRoutes; break;
        default: qDebug()<<"invalid operation";                               return;
        }
        MemberCallbackInfo obj={reinterpret_cast<CNTRLR *>(instance),reinterpret_cast<MemberCallback<CNTRLR>>(ptr),0};
        callBacks->insert(action, obj);
    }


    template <class T>
    void get   (const QString& action, T *instance, MemberCallback<T> ptr)
    {
        registerRoute(QNetworkAccessManager::GetOperation,action,instance,ptr);
    }

    template <class T>
    void post  (const QString& action, T *instance, MemberCallback<T> ptr)
    {
        registerRoute(QNetworkAccessManager::PostOperation,action,instance,ptr);
    }

    template <class T>
    void head (const QString& action, T *instance, MemberCallback<T> ptr)
    {
        registerRoute(QNetworkAccessManager::HeadOperation,action,instance,ptr);
    }

    template <class T>
    void put   (const QString& action, T *instance, MemberCallback<T> ptr)
    {
        registerRoute(QNetworkAccessManager::PutOperation,action,instance,ptr);
    }

    template <class T>
    void Delete(const QString& action, T *instance, MemberCallback<T> ptr)
    {
        registerRoute(QNetworkAccessManager::DeleteOperation,action,instance,ptr);
    }



    void registerRoute(QNetworkAccessManager::Operation op, const QString& action, Callback cb)
    {
        MemberCallbacks *callBacks;
        switch(op)
        {
        case QNetworkAccessManager::GetOperation:    callBacks=&getRoutes    ; break;
        case QNetworkAccessManager::PostOperation:   callBacks=&postRoutes   ; break;
        case QNetworkAccessManager::PutOperation:    callBacks=&putRoutes    ; break;
        case QNetworkAccessManager::DeleteOperation: callBacks=&DeleteRoutes ; break;
        case QNetworkAccessManager::HeadOperation:   callBacks=&headerhRoutes; break;
        default: qDebug()<<"invalid operation";                               return;
        }



        MemberCallbackInfo obj={0,0,cb};
        callBacks->insert(action, obj);
    }


    void post(const QString& action, Callback cb)
    {
        registerRoute(QNetworkAccessManager::PostOperation,action,cb);
    }

    void get(const QString& action, Callback cb)
    {
        registerRoute(QNetworkAccessManager::GetOperation,action,cb);
    }


private:
    MemberCallbacks getRoutes;
    MemberCallbacks postRoutes;
    MemberCallbacks headerhRoutes;
    MemberCallbacks putRoutes;
    MemberCallbacks DeleteRoutes;
};

typedef Router Route;

#endif // ROUTER_H
