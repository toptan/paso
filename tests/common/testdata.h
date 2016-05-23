#ifndef TESTDATA_H
#define TESTDATA_H

#include <QTest>

class TestData : public QObject {
    Q_OBJECT

private slots:
    void testComparingObjectWithItselfIsAlwaysTrue();
    void testSystemUserSerialization();
    void testRoomSerialization();
};

#endif // TESTDATA_H
