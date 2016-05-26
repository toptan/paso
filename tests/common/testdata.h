#ifndef TESTDATA_H
#define TESTDATA_H

#include <QTest>

class TestData : public QObject {
    Q_OBJECT

private slots:
    void testComparingObjectWithItselfIsAlwaysTrue();
    void testSystemUserSerialization();
    void testRoomSerialization();
    void testSystemRoleSerialization();
};

#endif // TESTDATA_H
