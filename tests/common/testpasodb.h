#ifndef TESTPASODB_H
#define TESTPASODB_H

#include "testbase.h"
#include <QTest>

class TestPasoDB : public TestBase {
    Q_OBJECT

public:
    TestPasoDB();

protected slots:
    virtual void initTestCase() override;

private slots:
    void testOperationsShouldFailOnDatabaseErrors();

    void testGetAllSystemUsers();
    void testGetSystemUser();
    void testSaveSystemUser();
    void testDeleteSystemUser();

    void testGetAllRooms();
    void testSaveRoom();
    void testGetRoom();
    void testDeleteRoom();

    void testSaveCourse();
    void testGetCourse();
    void testDeleteCourse();

    void testGetList();

    void testUsernameUnique();
    void testRoomUuidUnique();
    void testRoomNumberUnique();
    void testCourseCodeUnique();
    void testIndexNumberUnique();
    void testListNameUnique();

    void testSaveStudent();
    void testGetStudent();
    void testDeleteStudent();

    void testCourseImport();
    void testStudentImport();
    void testImportCourseStudent();
    void testImportListStudent();

    void testEnlistingStudentToCourses();
    void testEnlistingStudentsToCourse();
    void testRemovingStudentFromCourses();
    void testRemovingStudentsFromCourse();
    void testRemovingAllStudentsFromCourse();
    void testUpdatingCourseStudents();

    void testAddingStudentsToTheList();
    void testRemovingStudentsFromTheList();
    void testUpdatingListStudents();
    void testRemovingAllStudentsFromList();

    void testAssociateListsWithActivity();
    void testAssociateRoomsWithActivity();

private:
    QStringList usernames;
    QStringList roomUUIDs;
};

#endif // TESTPASODB_H
