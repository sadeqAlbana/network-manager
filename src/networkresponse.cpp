#include "networkresponse.h"

NetworkResponse::NetworkResponse(QNetworkReply *reply)
{
    _reply=reply;
    _data=reply->readAll();
    if(isJson())
    {
         _document=QJsonDocument::fromJson(_data);
        if(_document.isObject())
            _object=_document.object();
        else if(_document.isArray())
            _array=_document.array();
    }

}

NetworkResponse::~NetworkResponse()
{

}

QJsonValue NetworkResponse::json(QString key)
{
    return _object.value(key);
}

QJsonValue NetworkResponse::json()
{
    if(_document.isObject())
        return _object;
    else if(_document.isArray())
        return _array;
    else
        return QJsonValue();
}

QByteArray NetworkResponse::binaryData() const
{
    return _data;
}

bool NetworkResponse::isJson()
{
    return _reply->header(QNetworkRequest::ContentTypeHeader).toString().contains("application/json");
}

QString NetworkResponse::contentType() const
{
    return _reply->rawHeader("content-type");
}


QDebug operator <<(QDebug dbg, const NetworkResponse &res)
{
        QDebugStateSaver saver(dbg);
        dbg.noquote() <<"Network response: \n";
        dbg.noquote() << "Headers: \n";
        foreach (auto pair,res.networkReply()->rawHeaderPairs())
        {
            dbg.noquote() << pair.first <<" : " << pair.second << "\n";
        }
        dbg.noquote() << "body: \n";
        dbg.noquote() << "content type:" << res.contentType() <<"\n";
        dbg.noquote() << "status: "      <<      res.status() <<"\n";
        dbg.noquote() << res.binaryData();
        return dbg.maybeQuote();
}


QDebug operator <<(QDebug dbg, const NetworkResponse *res)
{
    return operator <<(dbg,*res);
}
