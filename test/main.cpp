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


    //callback to lambdas or normal functions
    manager.get("https://reqres.in/api/users?page=1")->subcribe([](NetworkResponse *res)
    {
        qDebug()<<res->json();
    });


    TestClass *testClass = new TestClass();

    //callbacks to member functions of any class
    manager.get("https://reqres.in/api/users?page=1")->subcribe(testClass,&TestClass::processData);

    NetworkResponse *res=manager.getSynch("https://reqres.in/api/users?page=1");

    qDebug()<<res->json("page").toInt();




    return a.exec();
}
