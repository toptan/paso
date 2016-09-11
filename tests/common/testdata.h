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
    void testActivityTypeSerialization();
    void testActivityScheduleTypeSerialization();
    void testCourseSerialization();
    void testStudentSerialization();
    void testTeacherSerialization();
    void testListSerialization();
    void testActivitySerialization();
    void testConversionToVariantMap();
    void testPropertyValues();
    void testDifferenceInMonths();
    void testScheduledDates();
    void testJsonArrayStringToVariantList();
    void testVariantListToJsonArrayString();
};

#endif // TESTDATA_H
