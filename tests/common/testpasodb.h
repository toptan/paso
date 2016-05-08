#ifndef TESTPASODB_H
#define TESTPASODB_H

#include <QTest>

class TestPasoDB : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void testGetAllSystemUsers();
};

#endif // TESTPASODB_H
