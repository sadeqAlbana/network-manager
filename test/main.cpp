#include <QCoreApplication>
#include "networkmanager.h"
#include <QFile>
#include <QImage>
#include <QJsonValue>
#include <QJsonObject>
#include <testclass.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    NetworkManager manager;


    //callback to lambdas and non-member functions
    manager.get("https://reqres.in/api/users?page=1")->subcribe([](NetworkResponse *res)
    {
        qDebug()<<res->json();
    });


    TestClass *testClass = new TestClass();

    //callbacks to member functions of any class type
    manager.get("https://reqres.in/api/users?page=1")->subcribe(testClass,&TestClass::processData);

    //synchronus call
    NetworkResponse *res=manager.getSynch("https://reqres.in/api/users?page=1");

    qDebug()<<res->json("page").toInt();

    QJsonObject object{{"testField","testValue"}};

    manager.post("url",object); //content type will be mapped to according to QVariant type unless specified manually

    manager.post("url",QByteArray("some data"),"text/raw");




    return a.exec();
}
