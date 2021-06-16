
**a simple http client with routing capabilities**

```
#include <QCoreApplication>
#include "networkmanager.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc,argv);

    NetworkManager manager;

    manager.get("https://reqres.in/api/users?page=2")->subcribe([](NetworkResponse *res)
    {
        qDebug()<<res->json();
    });

    return a.exec();
}
```
