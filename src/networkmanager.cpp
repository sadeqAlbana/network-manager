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
//    m_attempts(3)

//{

//    QObject::connect(m_manager,&NetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);
//    QObject::connect(m_synchronousManager,&QNetworkAccessManager::authenticationRequired,this,&NetworkManager::onAuthenticationRequired);


//    ;

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








//void NetworkManager::onAuthenticationRequired(QNetworkReply *reply, QAuthenticator *authenticator)
//{
//    Q_UNUSED(reply);
//    authenticator->setUser(authenticationCredentials.first);
//    authenticator->setPassword(authenticationCredentials.second);
//}


//void NetworkManager::setAuthenticationCredentails(const QString &user, const QString &password)
//{
//    authenticationCredentials.first=user;
//    authenticationCredentials.second=password;
//}



//void NetworkManager::routeReply(QNetworkReply *reply)
//{
//    emit finishedNetworkActivity(reply->url());
//    NetworkResponse *response=new NetworkResponse(reply);
//    m_router.route(response);
//    reply->deleteLater();
//    delete response;
//}




