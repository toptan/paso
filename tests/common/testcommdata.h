#ifndef TESTCOMMDATA_H
#define TESTCOMMDATA_H

#include <QTest>

class TestCommData : public QObject {
    Q_OBJECT

private slots:

    void testBaseSerialization();
    void testLoginRequestCreation();
    void testLoginRequestSerialization();
    void testLoginResponseCreation();
    void testLoginResponseSerialization();
};

#endif // TESTCOMMDATA_H
