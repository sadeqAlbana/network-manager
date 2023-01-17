///*
// * Copyright (C) 2022 Sadeq Albana
// *
// * Licensed under the GNU Lesser General Public License v3.0 :
// * https://www.gnu.org/licenses/lgpl-3.0.html
// */

//#include "networkmanager.h"
//#include <QNetworkAccessManager>
//#include <QJsonDocument>
//#include <QJsonObject>
//#include <QBuffer>
//#ifdef QT_HAVE_GUI
//#include <QImage>
//#endif
//#include <QJsonArray>
//#include <QJsonValue>
//#include <QAuthenticator>
//#include <QNetworkProxy>
//#if !defined(QT_NO_BEARERMANAGEMENT) && QT_VERSION <QT_VERSION_CHECK(6,0,0)
//#include <QNetworkConfiguration>
//#endif
//NetworkManager::NetworkManager(QObject *parent) : QObject (parent),
//    m_manager(new NetworkAccessManager(this)),
//    m_synchronousManager(new NetworkAccessManager(this)),
//    m_eventLoop(new QEventLoop(this)),
//    m_attempts(3)

//{
//    QObject::connect(m_manager,&NetworkAccessManager::finished,this,&NetworkManager::routeReply);
//    QObject::connect(m_synchronousManager,&QNetworkAccessManager::finished,m_eventLoop,&QEventLoop::quit);

//    QObject::connect(m_manager,&NetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);
//    QObject::connect(m_synchronousManager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);


//    QObject::connect(m_manager,&NetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);
//    QObject::connect(m_synchronousManager,&QNetworkAccessManager::proxyAuthenticationRequired,this,&NetworkManager::onProxyAuthenticationRequired);


//    ;

//}


//NetworkManager* NetworkManager::get(QString url, const bool monitorProgress)
//{
//    this->get(createNetworkRequest(url),monitorProgress);
//    return this;
//}

//NetworkManager *NetworkManager::get(const QNetworkRequest &request, const bool monitorProgress)
//{
//    this->createRequest(QNetworkAccessManager::GetOperation,request,QByteArray(),QByteArray(),monitorProgress);
//    return this;
//}

//NetworkManager* NetworkManager::post(const QString url, const QVariant data, QByteArray contentType)
//{
//    QNetworkRequest request = createNetworkRequest(url);
//    if(contentType.isNull())
//        contentType=mapContentType(static_cast<QMetaType::Type>(data.typeId()));

//    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
//    setLastReply(manager()->post(request,rawData(data)));
//    return this;
//}

//NetworkManager *NetworkManager::put(const QString url, const QVariant data, QByteArray contentType)
//{
//    QNetworkRequest request = createNetworkRequest(url);

//    if(contentType.isNull())
//        contentType=mapContentType(static_cast<QMetaType::Type>(data.typeId()));

//    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
//    setLastReply(manager()->put(request,rawData(data)));

//    return this;
//}

//NetworkResponse NetworkManager::getSynch(QString url)
//{
//    QNetworkReply *reply;
//    int attemps=1;
//    do{
//        reply= m_synchronousManager->get(createNetworkRequest(url));

//        if(isIgnoringSslErrors())
//            reply->ignoreSslErrors();

//        m_eventLoop->exec();
//        if(reply->error()==QNetworkReply::NoError || !isConnectionError(reply->error())){
//            break;
//        }
//        else{
//            attemps++;
//        }
//    }
//    while(attemps<=attemptsCount());

//    emit finishedNetworkActivity(reply->url());

//    return NetworkResponse(reply);
//}

//NetworkResponse NetworkManager::postSynch(const QString url, const QVariant data, QByteArray contentType)
//{
//    QNetworkRequest request=createNetworkRequest(url);

//    if(contentType.isNull())
//        contentType=mapContentType(static_cast<QMetaType::Type>(data.typeId()));

//    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
//    QNetworkReply *reply;
//    int attemps=1;
//    do{
//        reply= m_synchronousManager->post(request,rawData(data));
//        if(isIgnoringSslErrors())
//            reply->ignoreSslErrors();

//        m_eventLoop->exec();
//        if(reply->error()==QNetworkReply::NoError || !isConnectionError(reply->error())){
//            break;
//        }
//        else{
//            attemps++;
//        }
//    }
//    while(attemps<=attemptsCount());

//    emit finishedNetworkActivity(reply->url());

//    return NetworkResponse(reply);
//}

//NetworkResponse NetworkManager::putSynch(const QString url, const QVariant data, QByteArray contentType)
//{
//    QNetworkRequest request=createNetworkRequest(url);

//    if(contentType.isNull())
//        contentType=mapContentType(static_cast<QMetaType::Type>(data.typeId()));

//    request.setHeader(QNetworkRequest::ContentTypeHeader,contentType);
//    QNetworkReply *reply;
//    int attemps=1;
//    do{
//        reply= m_synchronousManager->put(request,rawData(data));
//        if(isIgnoringSslErrors() || !isConnectionError(reply->error()))
//            reply->ignoreSslErrors();

//        m_eventLoop->exec();
//        if(reply->error()==QNetworkReply::NoError){
//            break;
//        }
//        else{
//            attemps++;
//        }
//    }
//    while(attemps<=attemptsCount());

//    emit finishedNetworkActivity(reply->url());
//    return NetworkResponse(reply);
//}

//bool NetworkManager::isConnectionError(QNetworkReply::NetworkError error)
//{
//    switch (error) {
//    case QNetworkReply::ConnectionRefusedError: return true;
//    case QNetworkReply::RemoteHostClosedError: return true;
//    case QNetworkReply::HostNotFoundError: return true;
//    case QNetworkReply::TimeoutError: return true;
//    case QNetworkReply::NetworkSessionFailedError: return true;
//    default: return false;
//    }
//}



//void NetworkManager::subcribe(Callback cb)
//{
//    m_router.registerRoute(m_lastReply,cb);
//}

//bool NetworkManager::isIgnoringSslErrors() const
//{
//    return m_ignoreSslErrors;

//}

//void NetworkManager::ignoreSslErrors(bool ignore)
//{
//    m_ignoreSslErrors = ignore;
//}

//void NetworkManager::onSSLError(QNetworkReply *reply, const QList<QSslError> &errors)
//{
//    Q_UNUSED(errors)
//    reply->ignoreSslErrors();
//}




//#ifndef QT_NO_SSL
//void NetworkManager::connectToHostEncrypted(const QString &hostName, quint16 port, const QSslConfiguration &sslConfiguration)
//{
//    manager()->connectToHostEncrypted(hostName,port,sslConfiguration);
//    m_synchronousManager->connectToHostEncrypted(hostName,port,sslConfiguration);
//}
//#endif


//QNetworkRequest NetworkManager::createNetworkRequest(const QString &url)
//{
//    QNetworkRequest req;

//    qDebug()<<"creating request: " << url;
//    if(redirectAllowed()){
//        req.setAttribute(QNetworkRequest::RedirectionTargetAttribute,true);
//        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,QNetworkRequest::SameOriginRedirectPolicy);
//    }

//    QString requestUrl= usingBaseUrl() ? m_baseUrl+url : url;
//    req.setUrl(requestUrl);

//    for (const QByteArray & headerName : m_permanentRawHeaders.keys()) {
//        req.setRawHeader(headerName,m_permanentRawHeaders[headerName]);
//    }

//    //emit networkActivity(url);

//    return  req;
//}


//QNetworkReply *NetworkManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, const QByteArray &data, const QByteArray &verb, const bool monitorProgress)
//{
//    QNetworkReply *reply=nullptr;
//    switch(op){

//    case QNetworkAccessManager::GetOperation: reply=m_manager->get(originalReq);  break;
//    case QNetworkAccessManager::PostOperation: reply= m_manager->post(originalReq,data);  break;
//    case QNetworkAccessManager::PutOperation: reply= m_manager->put(originalReq,data);  break;
//    case QNetworkAccessManager::DeleteOperation: reply= m_manager->deleteResource(originalReq);  break;
//    case QNetworkAccessManager::HeadOperation: reply= m_manager->head(originalReq); break;
//    case QNetworkAccessManager::CustomOperation: reply= m_manager->sendCustomRequest(originalReq,verb,data); break;
//    case QNetworkAccessManager::UnknownOperation: return nullptr;;
//    }

//    if(monitorProgress){
//        connect(reply,&QNetworkReply::downloadProgress,this,[this,reply](qint64 bytesReceived, qint64 bytesTotal){
//            emit downloadProgress(reply->request().url(),bytesReceived,bytesTotal);
//        });
//    }

//    setLastReply(reply);

//    return reply;

//}

//QNetworkReply *NetworkManager::lastReply() const
//{
//    return m_lastReply;
//}

//void NetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
//{
//    Q_UNUSED(reply);
//    authenticator->setUser(authenticationCredentials.first);
//    authenticator->setPassword(authenticationCredentials.second);
//}

//void NetworkManager::setLastReply(QNetworkReply *reply)
//{
//    m_lastReply=reply;

//    if(isIgnoringSslErrors())
//        m_lastReply->ignoreSslErrors();
//}



//void NetworkManager::setAuthenticationCredentails(const QString &user, const QString &password)
//{
//    authenticationCredentials.first=user;
//    authenticationCredentials.second=password;
//}
//#if !defined(QT_NO_BEARERMANAGEMENT) && QT_VERSION <QT_VERSION_CHECK(6,0,0)
//void NetworkManager::setConfiguration(const QNetworkConfiguration &config)
//{
//    m_manager->setConfiguration(config);
//    m_synchronousManager->setConfiguration(config);
//}

//QNetworkConfiguration NetworkManager::configuration() const
//{
//    return m_manager->configuration();
//}
//#endif



//void NetworkManager::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
//{
//    Q_UNUSED(proxy);
//    authenticator->setUser(proxyAuthenticationCredentials.first);
//    authenticator->setPassword(proxyAuthenticationCredentials.second);
//}

//void NetworkManager::routeReply(QNetworkReply *reply)
//{
//    emit finishedNetworkActivity(reply->url());
//    NetworkResponse *response=new NetworkResponse(reply);
//    m_router.route(response);
//    reply->deleteLater();
//    delete response;
//}




