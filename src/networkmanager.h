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

    NetworkManager *get(QString url);
    NetworkManager *post(const QString url, const QVariant data, QByteArray contentType=QByteArray());
    NetworkManager * put (const QString url, const QVariant data, QByteArray contentType=QByteArray());

    NetworkResponse getSynch(QString url);
    NetworkResponse postSynch(const QString url, const QVariant data, QByteArray contentType=QByteArray());
    NetworkResponse putSynch(const QString url, const QVariant data, QByteArray contentType=QByteArray());


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
    inline QNetworkAccessManager* manager(){return &m_manager;}

    int attemptsCount() const;
    void setAttemptsCount(int attempts);
    void setAuthenticationCredentails(const QString &user, const QString &password);

#if !defined(QT_NO_BEARERMANAGEMENT)
    void setConfiguration(const QNetworkConfiguration &config);
    QNetworkConfiguration configuration() const;
#endif
    void setProxy(const QNetworkProxy &proxy);
    QNetworkProxy proxy() const;

    void onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);

    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    void removeRawHeader(const QByteArray &headerName);

    bool isIgnoringSslErrors() const;
    void ignoreSslErrors(bool ignore);

    void onSSLError(QNetworkReply *reply, const QList<QSslError> &errors);

#ifndef QT_NO_SSL
    void connectToHostEncrypted(const QString &hostName, quint16 port = 443, const QSslConfiguration &sslConfiguration = QSslConfiguration::defaultConfiguration());
#endif

#if QT_VERSION >=QT_VERSION_CHECK(5,15,0)
    void setTransferTimeout(int timeout = QNetworkRequest::DefaultTransferTimeoutConstant);
#endif

    QNetworkReply *lastReply() const;

signals:
    void networkActivity(QString url);
    void finishedNetworkActivity(QString url);

protected slots:
    virtual void routeReply(QNetworkReply *reply);

protected:

    QNetworkRequest createRequest(const QString &url);
    QByteArray mapContentType(const QVariant::Type type);
    QByteArray rawData(const QVariant &data);

protected:
    SNetworkManager::Router router;
    QNetworkAccessManager m_manager;
    QNetworkAccessManager synchronousManager;
    QEventLoop eventLoop;
    QString baseUrl;
    QNetworkReply* _lastReply;
    HeadersMap _permanentRawHeaders;

    bool _usingBaseUrl=false;
    bool _allowRedirect=false;
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void setLastReply(QNetworkReply *reply);
    HeadersMap & permanentRawHeaders(){return _permanentRawHeaders;}
    int _attempts;
    QPair<QString,QString> authenticationCredentials;
    QPair<QString,QString> proxyAuthenticationCredentials;

    bool _ignoreSslErrors;
};

#endif // NETWORKMANAGER_H
