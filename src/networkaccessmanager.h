#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
class NetworkResponse *res;
class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:

    enum RequstAttribute{
      IdAttribute               = 1001, //a unique identifier for each request
      MonitorProgressAttribute =  1002  //if set to true, the download \a progress() signal is emitted each time a chunck of data is received
    };

    /*!
        Constructs a NetworkAccessManager object that is the center of
        the Network Access API and sets \a parent as the parent object.
    */
    explicit NetworkAccessManager(QObject *parent = nullptr);



    /*!
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory
    */
    NetworkAccessManager *get(const QUrl &url);

    /*!
        sends a request with the passed \a url, internally creates a \a QNetworkRequset Object from the factory

    */
    NetworkAccessManager *get(const QNetworkRequest &request);

    /*!
        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */
    void abortAllRequests();

    QNetworkRequest createNetworkRequest(const QUrl &url);

signals:
    /*!
        this signal is emitted after the start of each network request
    */
    void networkActivity(QUrl url);
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal, int requestId);

    /*!
        this signal is emitted after the end of each network request, the request's response may not be completed !
    */
    void finishedNetworkActivity(QUrl url);


protected:
    /*!
        calls \a QNetworkAccessManager::createRequest internally

    */
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr);


private:
    QList<QNetworkReply *> m_replies;

};

#endif // NETWORKACCESSMANAGER_H
