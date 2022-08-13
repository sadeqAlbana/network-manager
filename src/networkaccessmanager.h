#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QNetworkReply>
class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = nullptr);
    void abortAllRequests();

signals:
    void networkActivity(QUrl url);
    void finishedNetworkActivity(QUrl url);


protected:
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr);

private:
    QList<QNetworkReply *> m_replies;
};

#endif // NETWORKACCESSMANAGER_H
