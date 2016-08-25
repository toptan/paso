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
    void testAcivityTypeSerialization();
    void testCourseSerialization();
    void testStudentSerialization();
    void testListSerialization();
    void testActivitySerialization();
    void testConversionToVariantMap();
    void testPropertyValues();
    void testDifferenceInMonths();
    void testScheduledDates();
};

#endif // TESTDATA_H
