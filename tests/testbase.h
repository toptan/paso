#ifndef TESTBASE_H
#define TESTBASE_H

#include <QTest>

class TestBase : public QObject {
    Q_OBJECT

public:
    static void printTestStats();

    TestBase();
    virtual ~TestBase();

protected slots:
    virtual void initTestCase();
    virtual void init();
    virtual void cleanup();
    virtual void cleanupTestCase();

protected:
    const QString dbName;
    static int m_executed;
    static int m_failed;
};

#endif // TESTBASE_H
