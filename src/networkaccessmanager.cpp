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

NetworkAccessManager::NetworkAccessManager(QObject *parent)
    : QNetworkAccessManager{parent}
{

}

NetworkResponse *NetworkAccessManager::head(const QUrl &url)
{
    return this->head(createNetworkRequest(url));

}

NetworkResponse *NetworkAccessManager::head(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::HeadOperation,request);
}

NetworkResponse *NetworkAccessManager::get(const QUrl &url)
{
    return this->get(createNetworkRequest(url));
}

NetworkResponse *NetworkAccessManager::get(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::GetOperation,request);
}

NetworkResponse *NetworkAccessManager::deleteResource(const QUrl &url)
{
    return this->deleteResource(createNetworkRequest(url));

}

NetworkResponse *NetworkAccessManager::deleteResource(const QNetworkRequest &request)
{
    return createNewRequest(QNetworkAccessManager::DeleteOperation,request);
}

NetworkResponse *NetworkAccessManager::post(const QUrl &url, const QVariant &data)
{
    QNetworkRequest request = createNetworkRequest(url);
    return post(request,data);
}


NetworkResponse *NetworkAccessManager::post(const QNetworkRequest &request, const QVariant &data)
{
    return createNewRequest(QNetworkAccessManager::PostOperation,request);
}

NetworkResponse *NetworkAccessManager::put(const QUrl &url, const QVariant &data)
{
    QNetworkRequest request = createNetworkRequest(url);
    return put(request,data);
}


NetworkResponse *NetworkAccessManager::put(const QNetworkRequest &request, const QVariant &data)
{
    return createNewRequest(QNetworkAccessManager::PutOperation,request);
}


void NetworkAccessManager::abortAllRequests()
{
    for(NetworkResponse *res : m_responses){
        res->networkReply()->abort();
    }
    m_responses.clear();
}

HeadersMap NetworkAccessManager::rawHeaders()
{
    return m_rawHeaders;
}

QByteArray NetworkAccessManager::rawHeader(const QByteArray &header) const
{
    return m_rawHeaders.value(header);
}

void NetworkAccessManager::setRawHeader(const QByteArray &headerName, const QByteArray &headerValue)
{
    m_rawHeaders[headerName]=headerValue;

}

void NetworkAccessManager::removeRawHeader(const QByteArray &headerName)
{
    m_rawHeaders.remove(headerName);

}

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

    //next is attempts count
    if(m_attempts>1){
        request.setAttribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::AttemptsCount),m_attempts);
    }

    //next is ID

    qint64 random=QRandomGenerator::global()->bounded(1,1000);
    qint64 id=QDateTime::currentMSecsSinceEpoch()+random;

    request.setAttribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::IdAttribute),id);



    return request;

}


NetworkResponse *NetworkAccessManager::createNewRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=createRequest(op,originalReq,outgoingData);
    NetworkResponse *res=new NetworkResponse(reply,this);
    m_responses << res;
    if(originalReq.attribute(static_cast<QNetworkRequest::Attribute>(RequstAttribute::MonitorProgressAttribute)).toBool()){
        connect(reply,&QNetworkReply::downloadProgress,this,[=](qint64 bytesReceived, qint64 bytesTotal){
            emit this->downloadProgress(bytesReceived,bytesTotal,res);
        });
    }

    m_lastResponse=res;
    connect(res,&NetworkResponse::finished,this,[this,res](){this->onResponseFinished(res);});
    emit networkActivity(originalReq.url());

    return res;
}

QNetworkReply *NetworkAccessManager::createRequest(Operation op, const QNetworkRequest &originalReq, QIODevice *outgoingData)
{
    QNetworkReply *reply=QNetworkAccessManager::createRequest(op,originalReq,outgoingData);

    m_replies << reply;
    return reply;
}

void NetworkAccessManager::onResponseFinished(NetworkResponse *res)
{
    this->route(res);
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

    default                      :                                   break;
    }
    return contentType;
}


