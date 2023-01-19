#include "networkaccessmanager.h"
#include <QNetworkReply>
#include "networkresponse.h"
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QBuffer>
#ifdef QT_HAVE_GUI
#include <QImage>
#endif
#include <QJsonArray>
#include <QJsonValue>
#include <QAuthenticator>
#include <QNetworkProxy>
#include <QRandomGenerator>
#if !defined(QT_NO_BEARERMANAGEMENT) && QT_VERSION <QT_VERSION_CHECK(6,0,0)
#include <QNetworkConfiguration>
#endif

/*!
    \fn explicit NetworkAccessManager::NetworkAccessManager(QObject *parent = nullptr)

    Constructs a NetworkAccessManager object that is the center of
    the Network Access API and sets parent as the parent object.
*/
NetworkAccessManager::NetworkAccessManager(QObject *parent)
    : QNetworkAccessManager{parent},m_ignoredErrors{
                                        QNetworkReply::ContentOperationNotPermittedError,
                                        QNetworkReply::ContentNotFoundError,
                                        QNetworkReply::NoError
                                        },
      m_ignoredSslErrors{QSslError(QSslError::SelfSignedCertificate),QSslError(QSslError::SelfSignedCertificateInChain)}
{

}

/*!
    \fn NetworkResponse * NetworkAccessManager::head(const QUrl &url)
    sends a HEAD request with the passed  url, internally creates a \a QNetworkRequset Object from the factory
    returns a pointer to the created NetworkResponse wrapper object.
*/

NetworkResponse *NetworkAccessManager::head(const QUrl &url)
{
    return this->head(createNetworkRequest(url));

}

/*!
    \fn NetworkResponse * NetworkAccessManager::head(const QNetworkRequest &request)

    sends a HEAD request using the passed \a QNetworkRequest object.
    note that the request must either be created using \a createNetworkRequest or have an ID attribute.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.

*/

NetworkResponse *NetworkAccessManager::head(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::HeadOperation,request);
}

/*!
    \fn NetworkResponse * NetworkAccessManager::get(const QUrl &url)
    sends a get request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.
*/

NetworkResponse *NetworkAccessManager::get(const QUrl &url)
{
    return this->get(createNetworkRequest(url));
}

/*!
    \fn NetworkResponse * NetworkAccessManager::get(const QNetworkRequest &request)

    sends a get request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.

*/

NetworkResponse *NetworkAccessManager::get(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::GetOperation,request);
}

/*!
    \fn NetworkResponse * NetworkAccessManager::deleteResource(const QUrl &url)
    sends a request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.
*/

NetworkResponse *NetworkAccessManager::deleteResource(const QUrl &url)
{
    return this->deleteResource(createNetworkRequest(url));

}

/*!
    \fn NetworkResponse * NetworkAccessManager::deleteResource(const QNetworkRequest &request)

    sends a request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.

*/

NetworkResponse *NetworkAccessManager::deleteResource(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::DeleteOperation,request);
}

/*!
    \fn NetworkResponse * NetworkAccessManager::post(const QUrl &url, const QVariant &data)
    sends a post request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.
*/

NetworkResponse *NetworkAccessManager::post(const QUrl &url, const QVariant &data)
{
    QNetworkRequest request = createNetworkRequest(url);
    return post(request,data);
}

/*!
    \fn NetworkResponse * NetworkAccessManager::post(const QNetworkRequest &request, const QVariant &data)

    sends a post request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.

*/


NetworkResponse *NetworkAccessManager::post(const QNetworkRequest &request, const QVariant &data)
{
    return createNewRequest(QNetworkAccessManager::PostOperation,request);
}


/*!
    \fn NetworkResponse * NetworkAccessManager::put(const QUrl &url, const QVariant &data)

    sends a put request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.
*/


NetworkResponse *NetworkAccessManager::put(const QUrl &url, const QVariant &data)
{
    QNetworkRequest request = createNetworkRequest(url);
    return put(request,data);
}


/*!
    \fn NetworkResponse * NetworkAccessManager::put(const QNetworkRequest &request, const QVariant &data)

    sends a put request with the passed url, internally creates a \a QNetworkRequset Object from the factory.
    returns a pointer to the created NetworkResponse wrapper object.
    the returned pointer must be deleted with \a deleteLater() unless it was routed with subscribe method, in which it will be deleted after the routing call.

*/

NetworkResponse *NetworkAccessManager::put(const QNetworkRequest &request, const QVariant &data)
{
    return createNewRequest(QNetworkAccessManager::PutOperation,request);
}

/*!
    \fn void NetworkAccessManager::abortAllRequests()

    aborts all requests that are being processed.
    calls  \a QNetworkReply::abort() and deletes the \a NetworkRespnse * pointers and \a QNetworkReply * pointers that was created with \a createNewRequest() and \a createRequest().
*/

void NetworkAccessManager::abortAllRequests()
{
    for(NetworkResponse *res : m_responses){
        res->networkReply()->abort();
    }
    m_responses.clear();
}

/*!
    \fn HeadersMap NetworkAccessManager::rawHeaders() const

    returns the raw headers pairs that are sent with each standard request.
*/

HeadersMap NetworkAccessManager::rawHeaders() const
{
    return m_rawHeaders;
}

/*!
    \fn QByteArray NetworkAccessManager::rawHeader(const QByteArray &header) const

    returns the header value associated with the \a header key or a default constructed \a QByteArray if not set.
*/

QByteArray NetworkAccessManager::rawHeader(const QByteArray &header) const
{
    return m_rawHeaders.value(header);
}

/*!
    \fn void NetworkAccessManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)

    adds a raw header that will be sent with each standard request
*/

void NetworkAccessManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    m_rawHeaders[headerName]=headerValue;

}

/*!
    \fn void NetworkAccessManager::removeRawHeader(const QByteArray &headerName)

    removes the header sepcified with \a headerName from the rawHeaders List.
*/


void NetworkAccessManager::removeRawHeader(const QByteArray &headerName)
{
    m_rawHeaders.remove(headerName);

}

/*!
    \fn QNetworkRequest NetworkAccessManager::createNetworkRequest(const QUrl &url)

    constructs a new \a QNetworkRequest object with an internal ID and the default configuration.
*/

QNetworkRequest NetworkAccessManager::createNetworkRequest(const QUrl &url)
{
    QNetworkRequest request;

    //merge the relative url with the base url
    QUrl requestUrl= m_baseUrl.isEmpty()? url  : m_baseUrl.resolved(url);

    request.setUrl(requestUrl);


    //add raw headers
    QMapIterator<QByteArray, QByteArray> i(m_rawHeaders);
    while (i.hasNext()) {
        i.next();
        request.setRawHeader(i.key(),i.value());
    }

    //next is ID

    qint64 random=QRandomGenerator::global()->bounded(1,1000);
    qint64 id=QDateTime::currentMSecsSinceEpoch()+random;

    request.setAttribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::IdAttribute),id);



    return request;

}

/*!
    \fn virtual NetworkResponse *NetworkAccessManager::createNewRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr)
    the default implementation calls \a NetworkAccessManager::createRequest internally
*/

NetworkResponse *NetworkAccessManager::createNewRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=createRequest(op,originalReq,outgoingData);
    NetworkResponse *res=new NetworkResponse(reply,this);
    m_responses << res;


    if(m_ignoredSslErrors.size()){
        reply->ignoreSslErrors(m_ignoredSslErrors);
    }

    if(originalReq.attribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::MonitorProgressAttribute)).toBool()){
        connect(reply,&QNetworkReply::downloadProgress,this,[=](qint64 bytesReceived, qint64 bytesTotal){
            emit this->downloadProgress(bytesReceived,bytesTotal,res);
        });
    }

    connect(reply,&QNetworkReply::errorOccurred,this,[this,res](QNetworkReply::NetworkError error){
        if(!m_ignoredErrors.contains(error)){
            emit networkError(res);
        }
    });

    emit networkActivity(originalReq.url());

    return res;
}

/*!
    \fn QNetworkReply *NetworkAccessManager::createRequest(QNetworkAccessManager::Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData = nullptr) override
    the default implementation calls \a NetworkAccessManager::createRequest internally, then adds the \a QNetworkReply * instance to the replies list.
*/

QNetworkReply *NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=QNetworkAccessManager::createRequest(op,originalReq,outgoingData);

    m_replies << reply;

    return reply;
}



QByteArray DataSerialization::serialize(const QVariant &data)
{
    QMetaType::Type type=static_cast<QMetaType::Type>(data.typeId());

        /**************************json**************************/
        if(type==QMetaType::Type::QJsonObject)
        {
            QJsonObject object=data.toJsonObject();
            QJsonDocument document;
            document.setObject(object);
            return document.toJson(QJsonDocument::Compact);
        }

        if(type==QMetaType::Type::QJsonArray)
        {
            QJsonArray array=data.toJsonArray();
            QJsonDocument document;
            document.setArray(array);
            return document.toJson(QJsonDocument::Compact);
        }

        if(type==QMetaType::Type::QJsonDocument)
        {
            return data.toJsonDocument().toJson(QJsonDocument::Compact);
        }

        if(type==QMetaType::Type::QJsonValue)
        {
            QJsonValue jsonValue=data.value<QJsonValue>();

            if(jsonValue.type()==QJsonValue::Type::Array)
            {
                QJsonArray array=jsonValue.toArray();
                QJsonDocument document;
                document.setArray(array);
                return document.toJson(QJsonDocument::Compact);
            }

            if(jsonValue.type()==QJsonValue::Type::Object)
            {
                QJsonObject object=jsonValue.toObject();
                QJsonDocument document;
                document.setObject(object);
                return document.toJson(QJsonDocument::Compact);
            }
            if(jsonValue.type()==QJsonValue::Type::String)
            {
                return jsonValue.toString().toUtf8();
            }
            if(jsonValue.type()==QJsonValue::Type::Double)
            {
                return QString::number(jsonValue.toDouble()).toUtf8();
            }
            if(jsonValue.type()==QJsonValue::Type::Bool)
            {
                return jsonValue.toBool() ? QByteArray("1") : QByteArray("0");
            }
            if(jsonValue.type()==QJsonValue::Type::Null){
                return QByteArray("");
            }
            if(jsonValue.type()==QJsonValue::Type::Undefined){
                return QByteArray();
            }

        }

        /**********************end json**************************/

        if(type==QMetaType::Type::QString)
            return data.toString().toUtf8();

        if(type==QMetaType::Type::Int)
            return QString::number(data.toInt()).toUtf8();

        if(type==QMetaType::Type::Double)
            return QString::number(data.toDouble()).toUtf8();

        if(type==QMetaType::Type::Float)
            return QString::number(data.toFloat()).toUtf8();

        if(type==QMetaType::Type::Long || type==QMetaType::Type::LongLong)
            return QString::number(data.toLongLong()).toUtf8();

        if(type==QMetaType::Type::UInt)
            return QString::number(data.toUInt()).toUtf8();

        if(type==QMetaType::Type::ULongLong)
            return QString::number(data.toULongLong()).toUtf8();

        if(type==QMetaType::Type::QByteArray)
            return data.toByteArray();
    #ifdef QT_HAVE_GUI
        if(type==QMetaType::Type::QImage)
        {
            QImage image=data.value<QImage>();
            QByteArray imageData;
            QBuffer buffer(&imageData);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer,"PNG");
            buffer.close();
            return imageData;
        }
    #endif


        if(type==QMetaType::Type::UnknownType)
            return QByteArray();

        qWarning()<<"DataSerialization::serialize : unsupported QVariant type: " << static_cast<QMetaType::Type>(data.typeId());

        return QByteArray();
}

QByteArray DataSerialization::contentType(const QMetaType::Type type)
{
//    if(m_permanentRawHeaders.contains("content-type")) //if this header already exists then return it to avoid conflicts
//        return m_permanentRawHeaders["content-type"];

    QByteArray contentType;
    //QMetaType::Type::Type type=static_cast<QMetaType::Type::Type>(data.type());
    switch ((type)) {
    case QMetaType::Type::QJsonObject  :
    case QMetaType::Type::QJsonValue   :
    case QMetaType::Type::QJsonArray   :
    case QMetaType::Type::QJsonDocument: contentType = "application/json"; break;
    case QMetaType::Type::QImage       : contentType = "image/png";        break;
    case QMetaType::Type::QString      : contentType = "text/plain";       break;

    default                      :                                         break;
    }
    return contentType;
}

/*!
    \fn void NetworkAccessManager::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
    this is a signal handler for QNetworkAccessManager::proxyAuthenticationRequired.
    the default implementation passes the stored proxy credentials to the authenticator.
*/


void NetworkAccessManager::onProxyAuthenticationRequired(const QNetworkProxy &proxy, QAuthenticator *authenticator)
{
    Q_UNUSED(proxy);
    authenticator->setUser(m_proxyAuthenticationCredentials.first);
    authenticator->setPassword(m_proxyAuthenticationCredentials.second);
}

/*!
    \fn NetworkAccessManager::routeReply(NetworkResponse *res)
    this is method will be called when a \a NetworkResponse registered with \a NetworkResponse::subscribe is finished aka emits (\a NetworkResponse::finished) signal.
    the default implementation calls the route if the response has no errors or the network error is within the ignored errors list.
*/


void NetworkAccessManager::routeReply(NetworkResponse *res)
{
    //we use the QNetworkReply::NoError just in case someone fucks up the original list
    if(m_ignoredErrors.contains(res->error()) || res->error()==QNetworkReply::NoError){
        this->route(res);
    }

}

/*!
    \fn void NetworkAccessManager::ignoredErrors(const QList<QNetworkReply::NetworkError> &newIgnoredErrors)

    returns a list of the errors that are ignored by \a networkError() signal.
*/

QList<QNetworkReply::NetworkError> NetworkAccessManager::ignoredErrors() const
{
    return m_ignoredErrors;
}

/*!
    \fn void NetworkAccessManager::setIgnoredErrors(const QList<QNetworkReply::NetworkError> &newIgnoredErrors)

    sets the list for network errors that will be ignored by \a networkError() signal.
*/

void NetworkAccessManager::setIgnoredErrors(const QList<QNetworkReply::NetworkError> &newIgnoredErrors)
{
    m_ignoredErrors = newIgnoredErrors;
}

/*!
    \fn QList<QSslError> NetworkAccessManager::ignoredSslErrors() const

    returns a list of the ssl errors that are passed to \a QNetworkReply::ignoreSslErrors().
*/


QList<QSslError> NetworkAccessManager::ignoredSslErrors() const
{
    return m_ignoredSslErrors;
}

/*!
    \fn void NetworkAccessManager::setIgnoredSslErrors(const QList<QSslError> &newIgnoredSslErrors)

    sets the list for ssl errors that will be passed to \a QNetworkReply::ignoreSslErrors().
*/

void NetworkAccessManager::setIgnoredSslErrors(const QList<QSslError> &newIgnoredSslErrors)
{
    m_ignoredSslErrors = newIgnoredSslErrors;
}

/*!
    \fn const QPair<QString, QString> &NetworkAccessManager::proxyAuthenticationCredentials() const

    returns the credentials used for proxy authentication, or a default constructed object if not set.
*/

const QPair<QString, QString> &NetworkAccessManager::proxyAuthenticationCredentials() const
{
    return m_proxyAuthenticationCredentials;
}

/*!
    \fn void NetworkAccessManager::setProxyAuthenticationCredentials(const QPair<QString, QString> &newProxyAuthenticationCredentials)

    sets the credentials that will be used for proxy authentication.
*/


void NetworkAccessManager::setProxyAuthenticationCredentials(const QPair<QString, QString> &newProxyAuthenticationCredentials)
{
    m_proxyAuthenticationCredentials = newProxyAuthenticationCredentials;
    connect(this,&NetworkAccessManager::proxyAuthenticationRequired,this,&NetworkAccessManager::onProxyAuthenticationRequired);
}
