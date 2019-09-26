#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H


#include "router.h"
#include "QNetworkReply"
#include "networkresponse.h"
#include <QSettings>
#include <QObject>


using HeadersMap= QMap<QByteArray,QByteArray>;

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent=nullptr);
    inline QNetworkAccessManager* manager(){return &m_manager;}
    NetworkManager *get(QString url);
    NetworkManager *post(QString url, QJsonObject object);
    NetworkManager * put (QString url, QJsonObject object);

    void subcribe(Callback cb);
    template <class T>
    void subcribe(T *instance,void (T::*ptr)(NetworkResponse *))
    {
        router.registerRoute(_lastReply,instance,ptr);
    }

    void setBaseUrl(QString url){baseUrl=url; _usingBaseUrl=true;}
    bool usingBaseUrl(){return _usingBaseUrl;}
    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    void removeRawHeader(const QByteArray &headerName);
    void allowRedirect(bool allow){_allowRedirect=allow;}
    inline bool redirectAllowed() const{return _allowRedirect;}

protected:
    void setJwtToken(QByteArray token);
    QString jwtToken(){return permanentRawHeaders()["authorization"];}
    QSettings settings;
    virtual void routeReply(QNetworkReply *reply);

private:
    QNetworkAccessManager m_manager;
    QString baseUrl;
    QNetworkReply* _lastReply;
    HeadersMap _permanentRawHeaders;
    Router router;
    bool _usingBaseUrl=false;
    bool _allowRedirect=false;



    inline void setLastReply(QNetworkReply *reply){_lastReply=reply;}
    HeadersMap & permanentRawHeaders(){return _permanentRawHeaders;}


};

#endif // NETWORKMANAGER_H
