#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QMap>
#include "router.h"
namespace DataSerialization {
    QByteArray serialize(const QVariant &data);
    QByteArray contentType(const QMetaType::Type type);

};

using HeadersMap= QMap<QByteArray,QByteArray>;

class NetworkResponse;
class NetworkAccessManager : public QNetworkAccessManager, protected SNetworkManager::Router
{
    Q_OBJECT
public:
    friend class NetworkResponse;

    enum RequstAttribute{
      IdAttribute                = 1001, /**< a unique identifier for each request. */
      MonitorProgressAttribute   = 1002,  /**< if set to true, the download \a progress() signal is emitted each time a chunck of data is received. */
      NotifyActivity             = 1003
    };


    explicit NetworkAccessManager(QObject *parent = nullptr);


    NetworkResponse *head(const QUrl &url);


    NetworkResponse *head(const QNetworkRequest &request);


    NetworkResponse *get(const QUrl &url);


    NetworkResponse *get(const QNetworkRequest &request);


    NetworkResponse *deleteResource(const QUrl &url);


    NetworkResponse *deleteResource(const QNetworkRequest &request);



    NetworkResponse *post(const QUrl &url, const QVariant &data);


    NetworkResponse *post(const QNetworkRequest &request, const QVariant &data);


    NetworkResponse *put(const QUrl &url, const QVariant &data);


    NetworkResponse *put(const QNetworkRequest &request, const QVariant &data);



    void abortAllRequests();


    HeadersMap rawHeaders() const;
    QByteArray rawHeader(const QByteArray &header)const;
    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);
    void removeRawHeader(const QByteArray &headerName);

    QNetworkRequest createNetworkRequest(const QUrl &url, const QVariant &data=QVariant());



    const QPair<QString, QString> &proxyAuthenticationCredentials() const;
    void setProxyAuthenticationCredentials(const QPair<QString, QString> &newProxyAuthenticationCredentials);

    QList<QSslError> ignoredSslErrors() const;
    void setIgnoredSslErrors(const QList<QSslError> &newIgnoredSslErrors);

    QList<QNetworkReply::NetworkError> ignoredErrors() const;
    void setIgnoredErrors(const QList<QNetworkReply::NetworkError> &newIgnoredErrors);

    const QUrl &baseUrl() const;
    void setBaseUrl(const QUrl &newBaseUrl);

    int monitoredRequestCount() const;

    void setRequestAttribute(QNetworkRequest::Attribute code, const QVariant &value);
    void removeRequsetAttribute(QNetworkRequest::Attribute code);



signals:
    /*!
        \fn void NetworkAccessManager::networkActivity(QUrl url)
        this signal is emitted after the start of each network request
    */
    void networkActivity(QUrl url);

    /*!
        \fn void NetworkAccessManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal, NetworkResponse *res)

        this signal is a wrapper around \a QNetworkReply::downloadProgress, with the addition of \a NetworkResponse *
    */
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, NetworkResponse *res);

    /*!
        \fn void NetworkAccessManager::networkError(NetworkResponse *res)

        this signal is emitted in case a network error occurs, note that network errors inside \a ignoredErrors() will be ignored !
    */
    void networkError(NetworkResponse *res);


    void monitoredRequestCountChanged();


protected:
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr) override;

    virtual NetworkResponse *createNewRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr);
    NetworkResponse *createNewRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, const QByteArray &data); ;

    virtual void onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator);
    virtual void routeReply(NetworkResponse *response);
    void setMonitoredRequestCount(int newMonitoredRequestCount);


protected:
    QList<NetworkResponse *> m_responses; /**< stores a list of the current responses */
    QList<QNetworkReply *> m_replies; /**< stores a list of the current replies */

    QUrl m_baseUrl; /**< if set, this url will be prepending before every request, unless the passed request url contains a complete url */
    HeadersMap m_rawHeaders; /**< stores a list of header pairs that will be used on each standard request */
    QPair<QString,QString> m_proxyAuthenticationCredentials;

    QList<QNetworkReply::NetworkError> m_ignoredErrors;
    QList<QSslError> m_ignoredSslErrors={QSslError(QSslError::NoError)};
    int m_monitoredRequestCount=0;
    QMap<QNetworkRequest::Attribute,QVariant> m_defaultRequestAttributes;
private:
    Q_PROPERTY(int monitoredRequestCount READ monitoredRequestCount NOTIFY monitoredRequestCountChanged)
};

#endif // NETWORKACCESSMANAGER_H
