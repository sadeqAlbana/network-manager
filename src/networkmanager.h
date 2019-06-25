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
        router.registerRoute(_lastOperation,_lastUrl,instance,ptr);
    }

    void setBaseUrl(QString url){baseUrl=url; _usingBaseUrl=true;}
    bool usingBaseUrl(){return _usingBaseUrl;}
    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    void removeRawHeader(const QByteArray &headerName);

protected:
    void setJwtToken(QByteArray token);
    QString jwtToken(){return _permanentRawHeaders["authorization"];}
    QSettings settings;

private:
    QNetworkAccessManager m_manager;
    QString baseUrl;
    QString _lastUrl;
    QNetworkAccessManager::Operation _lastOperation;
    QNetworkRequest _lastRequest;
    HeadersMap _permanentRawHeaders;
    Router router;
    bool _usingBaseUrl=false;
    //QUrl base;

     void routeReply(QNetworkReply *reply);
     inline void setLastRequest(const QNetworkRequest &request){_lastRequest=request;}
     inline void setLastUrl(const QString &url){_lastUrl=url;}
     inline void setLastOperation(const QNetworkAccessManager::Operation &operation){_lastOperation=operation;}
     HeadersMap & permanentRawHeaders(){return _permanentRawHeaders;}


};

#endif // NETWORKMANAGER_H
