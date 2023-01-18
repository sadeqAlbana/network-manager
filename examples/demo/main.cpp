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


    //callback to lambdas and non-member functions
//    manager.get(QUrl("https://reqres.in/api/users?page=1"))->subcribe([](NetworkResponse *res)
//    {
//        qDebug()<<res->json();
//    });


    TestClass *testClass = new TestClass();

    //callbacks to member functions of any class type
    manager.get(QUrl("https://reqres.in/api/users?page=1"))->subcribe(testClass,&TestClass::processData);

//    //synchronus call
//    NetworkResponse res=manager.getSynch("https://reqres.in/api/users?page=1");

//    qDebug()<<res.json("page").toInt();

//    QJsonObject object{{"testField","testValue"}};

//    manager.post("url",object); //content type will be mapped to according to QVariant type unless specified manually

//    manager.post("url",QByteArray("some text"),"text/raw");

    return a.exec();
}
