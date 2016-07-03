#ifndef TESTDATA_H
#define TESTDATA_H

#include <QTest>

class TestData : public QObject {
    Q_OBJECT

private slots:
    void testComparingObjectWithItselfIsAlwaysTrue();
    void testComparingStudentWithPersonOrProfessorIsAlwaysFalse();
    void testSystemUserSerialization();
    void testRoomSerialization();
    void testSystemRoleSerialization();
    void testCourseSerialization();
    void testStudentSerialization();
};

#endif // TESTDATA_H
