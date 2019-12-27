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
#include <QImage>
#include <QDomDocument>
class NetworkResponse
{
public:
    NetworkResponse(QNetworkReply *reply);
    ~NetworkResponse();
    QNetworkReply::NetworkError error() const;
    QJsonValue json(QString key);
    QJsonValue json();
    QByteArray binaryData() const{return _data;}

    bool isJson();
    bool isImage();
    QString contentTypeHeader() const;
    QUrl url(){return _reply->url();}
    QNetworkAccessManager::Operation operation() const {return _reply->operation();}
    QByteArray rawHeader(const QByteArray &headerName) const{return _reply->rawHeader(headerName);}
    QVariant attribute(QNetworkRequest::Attribute code) const{return _reply->attribute(code);}
    int status() const {return _reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();}
    QNetworkReply *networkReply() const {return _reply;}
    friend QDebug operator <<(QDebug dbg, const NetworkResponse &res);
    const QJsonObject jsonObject() const {return _replyData.toJsonObject();}
    const QJsonArray jsonArray() const {return _replyData.toJsonArray();}
    QImage image() const {return _image;}

    operator bool();
    QDomDocument xmlDocument(){return _domDoc;}

private:
    QNetworkReply *_reply;
    QByteArray _data;
    //QJsonObject _object;
    //QJsonArray _array;
    //QJsonDocument _document;
    QImage _image;
    QVariant _replyData;
    QDomDocument _domDoc;
    void setNetworkReply(QNetworkReply *reply){_reply=reply;}
    void setBinaryData(const QByteArray &data){_data=data;}
    //void setJsonObject(const QJsonObject &object){_object=object;}
    //void setJsonArray(const QJsonArray &array){_array=array;}
    //void setJsonDocument(const QJsonDocument &document){_document=document;}

    void processReply();


};

typedef NetworkResponse Response;

QDebug operator <<(QDebug dbg, const NetworkResponse &res);
QDebug operator <<(QDebug dbg, const NetworkResponse *res);

#endif // NETWORKRESPONSE_H
