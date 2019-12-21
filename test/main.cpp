#include <QCoreApplication>
#include "networkmanager.h"
#include <QFile>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    NetworkManager manager;

//    manager.get("https://reqres.in/api/users?page=2")->subcribe([](NetworkResponse *res)
//    {
//        qDebug()<<res->json();
//    });

    qDebug()<<"before";

    NetworkResponse *res=manager.getSynch("http://www.mitelove.com/image/catalog/2019/05/mite-love-boyundan-askili-seksi-vucut-corabi.jpg");

    res->image().save("/tmp/test2.jpg");


    qDebug()<<res;

    QFile file("/tmp/test.jpg");
    file.open(QIODevice::WriteOnly);
    file.write(res->binaryData());
    file.close();

    qDebug()<<"after";



    return a.exec();
}
