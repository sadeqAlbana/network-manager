#include <QCoreApplication>
#include <networkaccessmanager.h>
#include <QJsonValue>
#include <QJsonObject>
#include <testclass.h>
#include <functional>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    NetworkAccessManager manager;
    manager.setAttemptsCount(10);
    manager.setTransferTimeout(1000);


    QNetworkRequest request=manager.createNetworkRequest(QUrl("https://reqares.in/api/users?page=1"));
    request.setAttribute(static_cast<QNetworkRequest::Attribute>(NetworkAccessManager::RequstAttribute::OverrideErrorHandling),true);

//    //callback to lambdas and regular functions
    manager.get(request)->subscribe([](NetworkResponse *res)
    {
        qDebug()<<"routed";
        //qDebug()<<res->json();
    });

//    QObject::connect(&manager,&NetworkAccessManager::networkError,[=](NetworkResponse *res){


//        qDebug()<<res->error();
//    });


    TestClass testClass = TestClass();

//    //callbacks to member functions of any class type
    //manager.get(QUrl("https://reqres.in/api/users?page=1"))->subscribe(&testClass,&TestClass::processData);

//    //synchronus call
//    NetworkResponse *res=manager.get(QUrl("https://reqres.in/api/users?page=1"));
//    res->waitForFinished();
//    qDebug()<<res->json();

//    qDebug()<<res.json("page").toInt();

//    QJsonObject object{{"testField","testValue"}};

//    manager.post("url",object); //content type will be mapped to according to QVariant type unless specified manually

//    manager.post("url",QByteArray("some text"),"text/raw");


    qDebug()<<"test end";
    return a.exec();
}
