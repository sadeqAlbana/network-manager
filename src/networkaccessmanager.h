#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
class NetworkResponse *res;
class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    /*!
        Constructs a NetworkAccessManager object that is the center of
        the Network Access API and sets \a parent as the parent object.
    */
    explicit NetworkAccessManager(QObject *parent = nullptr);


    /*!
        aborts all requests that are being processed
        calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointer that was created with \a createRequest()
    */
    void abortAllRequests();

signals:
    void networkActivity(QUrl url);
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
