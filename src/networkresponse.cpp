#include "networkresponse.h"

NetworkResponse::NetworkResponse(QNetworkReply *reply)
{
    setNetworkReply(reply);
    setBinaryData(networkReply()->readAll());
    processReply();
}

NetworkResponse::~NetworkResponse()
{
    _reply->deleteLater();
}

QNetworkReply::NetworkError NetworkResponse::error() const
{
    return networkReply()->error();
}

QJsonValue NetworkResponse::json(QString key)
{
    return jsonObject().value(key);
}

QJsonValue NetworkResponse::json()
{
    if(_replyData.type()==QMetaType::QJsonObject)
        return _replyData.toJsonObject();
    else if(_replyData.type()==QMetaType::QJsonArray)
        return _replyData.toJsonArray();
    else
        return QJsonValue();
}

bool NetworkResponse::isJson()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("application/json");
}

bool NetworkResponse::isImage()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("image/");
}

QString NetworkResponse::contentTypeHeader() const
{
    return networkReply()->rawHeader("content-type");
}

void NetworkResponse::processReply()
{
    QString contentType=contentTypeHeader();
    if(contentType=="application/json")
    {
        QJsonDocument doc=QJsonDocument::fromJson(binaryData());
        if(!doc.isNull())
        {
            if(doc.isArray())
                _replyData=doc.array();
            if(doc.isObject())
                _replyData=doc.object();
        }
        return;
    }
    if(contentType.contains("image/"))
    {
        QImage img=QImage::fromData(binaryData());
        if(!img.isNull())
            _replyData=img;
    }
    if(contentType.contains("text/"))
    {
        _replyData=QString(binaryData());
    }
}


NetworkResponse::operator bool()
{
    return !error();
}

QDebug operator <<(QDebug dbg, const NetworkResponse &res)
{
        QDebugStateSaver saver(dbg);
        dbg.noquote() <<"Network response: \n";
        dbg.noquote() << "Headers: \n";
        for (auto pair : res.networkReply()->rawHeaderPairs())
        {
            dbg.noquote() << pair.first <<" : " << pair.second << "\n";
        }
        dbg.noquote() << "body: \n";
        dbg.noquote() << "content type:" << res.contentTypeHeader() <<"\n";
        dbg.noquote() << "status: "      <<      res.status() <<"\n";
        dbg.noquote() << res.binaryData();
        return dbg.maybeQuote();
}


QDebug operator <<(QDebug dbg, const NetworkResponse *res)
{
    return operator <<(dbg,*res);
}
