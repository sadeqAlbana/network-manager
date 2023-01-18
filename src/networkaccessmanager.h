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
class NetworkAccessManager : public QNetworkAccessManager, private SNetworkManager::Router
{
    Q_OBJECT
public:

    enum RequstAttribute{
      IdAttribute                = 1001, //a unique identifier for each request
      MonitorProgressAttribute   = 1002,  //if set to true, the download \a progress() signal is emitted each time a chunck of data is received
      AttemptsCount              = 1003 //a unique identifier for each request

    };

    /*!
        \fn explicit NetworkAccessManager::NetworkAccessManager(QObject *parent = nullptr)

        Constructs a NetworkAccessManager object that is the center of
        the Network Access API and sets \a parent as the parent object.
    */
    explicit NetworkAccessManager(QObject *parent = nullptr);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::head(const QUrl &url)
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
        and returns a pointer to the NetworkAccessManager
    */
    NetworkAccessManager *head(const QUrl &url);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::head(const QNetworkRequest &request)

        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *head(const QNetworkRequest &request);

    /*!
        \fn void NetworkAccessManager::abortAllRequests()

        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */



    /*!
        \fn NetworkAccessManager * NetworkAccessManager::get(const QUrl &url)
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
        and returns a pointer to the NetworkAccessManager
    */
    NetworkAccessManager *get(const QUrl &url);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::get(const QNetworkRequest &request)

        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *get(const QNetworkRequest &request);

    /*!
        \fn void NetworkAccessManager::abortAllRequests()

        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::deleteResource(const QUrl &url)
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
        and returns a pointer to the NetworkAccessManager
    */
    NetworkAccessManager *deleteResource(const QUrl &url);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::deleteResource(const QNetworkRequest &request)

        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *deleteResource(const QNetworkRequest &request);

    /*!
        \fn void NetworkAccessManager::abortAllRequests()

        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::post(const QUrl &url, const QVariant &data)
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
        and returns a pointer to the NetworkAccessManager
    */
    NetworkAccessManager *post(const QUrl &url, const QVariant &data);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::post(const QNetworkRequest &request, const QVariant &data)

        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *post(const QNetworkRequest &request, const QVariant &data);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::put(const QUrl &url, const QVariant &data)
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
        and returns a pointer to the NetworkAccessManager
    */
    NetworkAccessManager *put(const QUrl &url, const QVariant &data);

    /*!
        \fn NetworkAccessManager * NetworkAccessManager::put(const QNetworkRequest &request, const QVariant &data)

        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *put(const QNetworkRequest &request, const QVariant &data);

    /*!
        \fn void NetworkAccessManager::abortAllRequests()

        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */

    /*!
        \fn void NetworkAccessManager::abortAllRequests()

        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */

    void abortAllRequests();

    /*!
        \fn HeadersMap NetworkAccessManager::NetworkAccessManager::rawHeaders()

        returns the raw headers pairs that are sent with each standard request.
    */
    HeadersMap  rawHeaders();

    /*!
        \fn QByteArray NetworkAccessManager::rawHeader(const QByteArray &header)const

        returns the header value associated with the \a header key or a default constructed \a QByteArray if not set.
    */
    QByteArray rawHeader(const QByteArray &header)const;

    /*!
        \fn void NetworkAccessManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)

        adds a raw header that will be sent with each standard request
    */

    void setRawHeader(const QByteArray &headerName, const QByteArray &headerValue);

    /*!
        \fn void NetworkAccessManager::removeRawHeader(const QByteArray &headerName)

        removes the header sepcified with \a headerName from the rawHeaders List.
    */

    void removeRawHeader(const QByteArray &headerName);

    QNetworkRequest createNetworkRequest(const QUrl &url);


    NetworkAccessManager * subcribe(Callback cb);
    template <class T>
    NetworkAccessManager * subcribe(T *instance,void (T::*ptr)(NetworkResponse *))
    {
        registerRoute(m_lastResponse,instance,ptr);
        return this;
    }

signals:
    /*!
        \fn void networkActivity(QUrl url)
        this signal is emitted after the start of each network request
    */
    void networkActivity(QUrl url);

    /*!
        \fn void NetworkAccessManager::downloadProgress(qint64 bytesReceived, qint64 bytesTotal, NetworkResponse *res)

        this signal is a wrapper around \a QNetworkReply::downloadProgress, with the addition of \a NetworkResponse *
    */
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, NetworkResponse *res);


protected:
    /*!
        calls \a QNetworkAccessManager::createRequest internally

    */
    virtual NetworkResponse *createNewRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr);
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr) override;


private:
    QList<NetworkResponse *> m_responses; /**< stores a list of the current responses */
    QList<QNetworkReply *> m_replies; /**< stores a list of the current replies */

    QUrl m_baseUrl; /**< if set, this url will be prepending before every request, unless the passed request url contains a complete url */
    HeadersMap m_rawHeaders; /**< stores a list of header pairs that will be used on each standard request */
    int m_attempts = 1; /**< the number of rerequest attempts in case the request failed */
    NetworkResponse* m_lastResponse;  /**< returns a pointer to the last created NetworkResponse object */
    void onResponseFinished(NetworkResponse *res);
};

#endif // NETWORKACCESSMANAGER_H
