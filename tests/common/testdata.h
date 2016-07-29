#ifndef TESTDATA_H
#define TESTDATA_H

#include "testbase.h"
#include <QTest>

class TestData : public TestBase {
    Q_OBJECT

public:
    TestData();

private slots:
    void testComparingObjectWithItselfIsAlwaysTrue();
    void testComparingStudentWithPersonOrProfessorIsAlwaysFalse();
    void testSystemUserSerialization();
    void testRoomSerialization();
    void testSystemRoleSerialization();
    void testCourseSerialization();
    void testStudentSerialization();
    void testListSerialization();
    void testConversionToVariantMap();
    void testPropertyValues();
};

#endif // TESTDATA_H
