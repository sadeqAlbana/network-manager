#ifndef NETWORKRESPONSE_H
#define NETWORKRESPONSE_H

#include <QByteArray>
#include <QJsonDocument>
#include <QNetworkReply>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QString>
#include <QDebug>

class NetworkResponse
{
public:
    NetworkResponse(QNetworkReply *reply);
    ~NetworkResponse();
    QJsonValue json(QString key);
    QJsonValue json();
    QByteArray binaryData() const{return _data;}

    bool isJson();
    QString contentType() const;
    QUrl url(){return _reply->url();}
    QNetworkAccessManager::Operation operation() const {return _reply->operation();}
    QByteArray rawHeader(const QByteArray &headerName) const{return _reply->rawHeader(headerName);}
    QVariant attribute(QNetworkRequest::Attribute code) const{return _reply->attribute(code);}
    QVariant status() const {return _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);}
    QNetworkReply *networkReply() const {return _reply;}
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    const QJsonDocument& jsonDocument() const{return _document;}
    const QJsonObject& jsonObject() const {return _object;}
    const QJsonArray& jsonArray() const {return _array;}

private:
    QNetworkReply *_reply;
    QByteArray _data;
    QJsonObject _object;
    QJsonArray _array;
    QJsonDocument _document;
    void setNetworkReply(QNetworkReply *reply){_reply=reply;}
    void setBinaryData(const QByteArray &data){_data=data;}
    void setJsonObject(const QJsonObject &object){_object=object;}
    void setJsonArray(const QJsonArray &array){_array=array;}
    void setJsonDocument(const QJsonDocument &document){_document=document;}

};

typedef NetworkResponse Response;

QDebug operator <<(QDebug dbg, const NetworkResponse &res);
QDebug operator <<(QDebug dbg, const NetworkResponse *res);

#endif // NETWORKRESPONSE_H