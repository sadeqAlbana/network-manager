#include "networkresponse.h"

NetworkResponse::NetworkResponse(QNetworkReply *reply)
{
    setNetworkReply(reply);
    setBinaryData(networkReply()->readAll());
    if(isJson())
    {
         setJsonDocument(QJsonDocument::fromJson(binaryData()));
        if(jsonDocument().isObject())
        setJsonObject(jsonDocument().object());
        else if(jsonDocument().isArray())
            setJsonArray(jsonDocument().array());
    }

}

NetworkResponse::~NetworkResponse()
{

}

QJsonValue NetworkResponse::json(QString key)
{
    return jsonObject().value(key);
}

QJsonValue NetworkResponse::json()
{
    if(jsonDocument().isObject())
        return jsonObject();
    else if(jsonDocument().isArray())
        return jsonArray();
    else
        return QJsonValue();
}



bool NetworkResponse::isJson()
{
    return networkReply()->header(QNetworkRequest::ContentTypeHeader).toString().contains("application/json");
}

QString NetworkResponse::contentType() const
{
    return networkReply()->rawHeader("content-type");
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