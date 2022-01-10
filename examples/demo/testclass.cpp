#include "testclass.h"
#include <QJsonValue>
TestClass::TestClass()
{

}

void TestClass::processData(NetworkResponse *res)
{
    qDebug()<<res->json();
}
