#ifndef TESTCLASS_H
#define TESTCLASS_H
#include "networkresponse.h"

class TestClass
{
public:
    TestClass();
    void processData(NetworkResponse *res);
};

#endif // TESTCLASS_H
