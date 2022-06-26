#ifndef NETWORKACCESSMANAGER_H
#define NETWORKACCESSMANAGER_H

#include <QNetworkAccessManager>
#include <QObject>
class NetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT
public:
    explicit NetworkAccessManager(QObject *parent = nullptr);
    void abortAllRequets();
protected:
    QNetworkReply *createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr);
signals:
    void networkActivity(QUrl url);
    void finishedNetworkActivity(QUrl url);

private:
    QList<QNetworkReply *> m_replies;
};

#endif // NETWORKACCESSMANAGER_H
