#include <QtTest>

// add necessary includes here

class networkmanager : public QObject
{
    Q_OBJECT

public:
    networkmanager();
    ~networkmanager();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

networkmanager::networkmanager()
{

}

networkmanager::~networkmanager()
{

}

void networkmanager::initTestCase()
{

}

void networkmanager::cleanupTestCase()
{

}

void networkmanager::test_case1()
{

}

QTEST_APPLESS_MAIN(networkmanager)

#include "tst_networkmanager.moc"

