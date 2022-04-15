/*
 * Copyright (C) 2022 Sadeq Albana
 *
 * Licensed under the GNU Lesser General Public License v3.0 :
 * https://www.gnu.org/licenses/lgpl-3.0.html
 */

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

    bool isConnectionError(QNetworkReply::NetworkError error);
    HeadersMap  permanentRawHeaders(){return m_permanentRawHeaders;} //chaning this methods signiture will cause a disaster to apps using this library, it's changed !
    QByteArray rawHeader(const QByteArray &header)const {return m_permanentRawHeaders.value(header);}

    void subcribe(Callback cb);
    template <class T>
    void subcribe(T *instance,void (T::*ptr)(NetworkResponse *))
    {
        m_router.registerRoute(m_lastReply,instance,ptr);
    }

    void setBaseUrl(QString url){m_baseUrl=url;}
    bool usingBaseUrl(){return !m_baseUrl.isEmpty();}
    void allowRedirect(bool allow){m_allowRedirect=allow;}
    inline bool redirectAllowed() const{return m_allowRedirect;}
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
    static QByteArray rawData(const QVariant &data);

signals:
    void networkActivity(QString url);
    void finishedNetworkActivity(QString url);

protected slots:
    virtual void routeReply(QNetworkReply *reply);

protected:

    QNetworkRequest createRequest(const QString &url);
    QByteArray mapContentType(const QVariant::Type type);

protected:
    Router m_router;
    QNetworkAccessManager m_manager;
    QNetworkAccessManager m_synchronousManager;
    QEventLoop m_eventLoop;
    QString m_baseUrl;
    QNetworkReply* m_lastReply;
    HeadersMap m_permanentRawHeaders;

    bool m_allowRedirect=false;
    void onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator);
    void setLastReply(QNetworkReply *reply);
    int m_attempts;
    QPair<QString,QString> authenticationCredentials;
    QPair<QString,QString> proxyAuthenticationCredentials;

    bool m_ignoreSslErrors;
};

#endif // NETWORKMANAGER_H
