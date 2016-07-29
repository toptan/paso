#ifndef TESTBASE_H
#define TESTBASE_H

#include <QTest>

class TestBase : public QObject {
    Q_OBJECT

public:
    TestBase();

protected slots:
    virtual void initTestCase();
    virtual void init();
    virtual void cleanup();
    virtual void cleanupTestCase();

protected:
    const QString dbName;
};

#endif // TESTBASE_H
