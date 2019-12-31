#ifndef NETWORKMANAGER_H
#define NETWORKMANAGER_H


#include <QObject>
#include "networkresponse.h"
#include "router.h"
#include <QNetworkReply>
#include <QEventLoop>

using HeadersMap= QMap<QByteArray,QByteArray>;

class NetworkManager : public QObject
{
    Q_OBJECT
public:
    NetworkManager(QObject *parent=nullptr);
    inline QNetworkAccessManager* manager(){return &m_manager;}
    NetworkManager *get(QString url);
    NetworkManager *post(const QString url, const QVariant data, QByteArray contentType=QByteArray());
    NetworkManager * put (const QString url, const QVariant data, QByteArray contentType=QByteArray());

    NetworkResponse *getSynch(QString url);
    NetworkResponse *postSynch(const QString url, const QVariant data, QByteArray contentType);
    NetworkResponse *putSynch(const QString url, const QVariant data, QByteArray contentType);

    void subcribe(Callback cb);
    template <class T>
    void subcribe(T *instance,void (T::*ptr)(NetworkResponse *))
    {
        router.registerRoute(_lastReply,instance,ptr);
    }

    void setBaseUrl(QString url){baseUrl=url; _usingBaseUrl=true;}
    bool usingBaseUrl(){return _usingBaseUrl;}
    void allowRedirect(bool allow){_allowRedirect=allow;}
    inline bool redirectAllowed() const{return _allowRedirect;}

protected:
    virtual void routeReply(QNetworkReply *reply);
    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    void removeRawHeader(const QByteArray &headerName);
    QNetworkRequest createRequest(const QString &url);
    QByteArray mapContentType(const QVariant &data);
    QByteArray rawData(const QVariant &data);

private:
    QNetworkAccessManager m_manager;
    QNetworkAccessManager synchronousManager;
    QEventLoop eventLoop;
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
