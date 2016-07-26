#ifndef TESTPASODB_H
#define TESTPASODB_H

#include <QTest>

class TestPasoDB : public QObject {
    Q_OBJECT

public:
    TestPasoDB();

private slots:

    void initTestCase();
    void init();
    void cleanup();

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

    void testEnlistingStudentToCourses();
    void testEnlistingStudentsToCourse();
    void testRemovingStudentFromCourses();
    void testRemovingStudentsFromCourse();
    void testUpdatingCourseStudents();

private:
    const QString dbName;
    QStringList usernames;
    QStringList roomUUIDs;
};

#endif // TESTPASODB_H
