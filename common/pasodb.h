#ifndef PASODB_H
#define PASODB_H

#include "activity.h"
#include "course.h"
#include "data.h"
#include "entity.h"
#include "list.h"
#include "room.h"
#include "student.h"
#include "systemuser.h"
#include "teacher.h"

#include <QObject>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVariantMap>
#include <QVector>

#include <memory>

namespace paso {
namespace db {

///
/// \brief DB_NAME The database name
///
const static QString DEFAULT_DB_NAME = "paso";

///
/// \brief The DBManager class is responsible for all database operations.
///
class DBManager : public QObject {
    Q_OBJECT
public:
    ///
    /// \brief recordToVariantMap Converts SQL record to variant map.
    /// \param record The record to convert.
    /// \return The variant map containing the same data as record.
    ///
    static const QVariantMap recordToVariantMap(const QSqlRecord &record);

    ///
    /// \brief DBManager Constructs database manager that will use database with
    /// given name.
    /// \param dbName the name of database to use.
    ///
    explicit DBManager(const QString &dbName = DEFAULT_DB_NAME);

    ///
    /// \brief beginTransaction Starts database transaction.
    ///
    void beginTransaction() const;

    ///
    /// \brief commit Commits started transaction.
    /// \return The SQL error if any.
    ///
    QSqlError commit() const;

    ///
    /// \brief rollback Rollsback started transaction.
    /// \return The SQL error if any.
    ///
    void rollback() const;

    ///
    /// \brief getAllSystemUsers Returns a vector of all system users.
    /// \param [out] error SQL error if any.
    /// \return A vector of all system users.
    ///
    std::shared_ptr<std::vector<data::entity::SystemUser>>
    getAllSystemUsers(QSqlError &error) const;

    ///
    /// \brief getSystemUser Returns a system user with username \c username.
    /// \param [in]  username The user name to look for.
    /// \param [out] error SQL error if any.
    /// \return Found user or \c nullptr of none is found.
    ///
    std::shared_ptr<data::entity::SystemUser>
    getSystemUser(const QString &username, QSqlError &error) const;

    ///
    /// \brief saveSystemUser Adds a new or updates existing system user.
    /// \param [in]  user The user.
    /// \param [out] error SQL error if any.
    /// \return \c true if user is successfully saved.
    ///
    bool saveSystemUser(data::entity::SystemUser &user, QSqlError &error) const;

    ///
    /// \brief deleteSystemUser Deletes system user with given \c username.
    /// The user 'root' cannot be deleted.
    /// \param [in]  username The username.
    /// \param [out] error SQL error if any.
    /// \return \c true if user is successfully deleted.
    ///
    bool deleteSystemUser(const QString &username, QSqlError &error) const;

    ///
    /// \brief getAllRooms Returns a vector of all rooms.
    /// \param [out] error SQL error if any.
    /// \return A vector of all system users.
    ///
    std::shared_ptr<std::vector<data::entity::Room>>
    getAllRooms(QSqlError &error) const;

    ///
    /// \brief Returns room with given \c roomUUID
    /// \param roomUUID The room UUID to look for.
    /// \param error SQL error if any.
    /// \return Found room or \c nullptr if none is found.
    ///
    std::shared_ptr<data::entity::Room> getRoom(const QUuid &roomUUID,
                                                QSqlError &error) const;

    ///
    /// \brief saveRoom Adds a new or updates existing room. If adding new one
    /// the id property of the room will be updated.
    /// \param room The room.
    /// \param error SQL error if any.
    /// \return \c true if room is successfully saved.
    ///
    bool saveRoom(data::entity::Room &room, QSqlError &error) const;

    ///
    /// \brief deleteRoom Deletes the room with given UUID.
    /// \param [in]  roomUUID The UUID of the room to delete.
    /// \param [out] error SQL error if any.
    /// \return \c true of room is succesfully deleted.
    ///
    bool deleteRoom(const QUuid &roomUUID, QSqlError &error) const;

    ///
    /// \brief getCourse Returns course with given \c courseCode.
    /// \param [in]  courseCode The course code to look for.
    /// \param [out] error The SQL error if any.
    /// \return \c Found course or \c nullptr if none is found.
    ///
    std::shared_ptr<data::entity::Course> getCourse(const QString &courseCode,
                                                    QSqlError &error) const;

    ///
    /// \brief saveCourse Adds a new or updates existing course. If adding new
    /// one the id property of the course will be updated.
    /// \param course The course.
    /// \param error The SQL error if any.
    /// \return \c true if course is successfully safed.
    ///
    bool saveCourse(data::entity::Course &course, QSqlError &error) const;

    ///
    /// \brief deleteCourse Deletes course with given code.
    /// \param [in]  courseCode The code of the course to delete.
    /// \param [out] error The SQL error if any.
    /// \return \c true if course is successfully deleted.
    ///
    bool deleteCourse(const QString &courseCode, QSqlError &error) const;

    ///
    /// \brief getStudentByIndexNumber Returns student with given \c
    /// indexNumber.
    /// \param [in]  indexNumber The index number to look for.
    /// \param [out] error The SQL error if any.
    /// \return \c Found student or \c nullptr if none is found.
    ///
    std::shared_ptr<data::entity::Student>
    getStudentByIndexNumber(const QString &indexNumber, QSqlError &error) const;

    ///
    /// \brief Adds new or updates existing student. If adding new
    /// student the id property of the student will be updated.
    /// \param student The student.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully saved.
    ///
    bool saveStudent(data::entity::Student &student, QSqlError &error) const;

    ///
    /// \brief Deletes student with given \c indexNumber. Deleting
    /// non existant student will not result in error.
    /// \param indexNumber The index number of the student to delete.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully deleted.
    ///
    bool deleteStudent(const QString &indexNumber, QSqlError &error) const;

    ///
    /// \brief Adds new or updates existing teacher. If adding new teacher the
    /// id property of the teacher will be updated.
    /// \param teacher The teacher.
    /// \param error The SQL error if any.
    /// \return \c true if teacher is successfully saved.
    ///
    bool saveTeacher(data::entity::Teacher &teacher, QSqlError &error) const;

    ///
    /// \brief Deletes teacher with given \c employeeNumber. Deleting non
    /// existant teacher will not result in error.
    /// \param employeeNumber The employee number of the teacher to delete.
    /// \param error The SQL error if any.
    /// \return \c true if teacher is successfully deleted.
    ///
    bool deleteTeacher(const QString &employeeNumber, QSqlError &error) const;

    ///
    /// \brief Checks if given username is unique.
    /// \param username The username to check.
    /// \param error The SQL error if any.
    /// \return \c true if username does not exist in the database.
    ///
    bool usernameUnique(const QString &username, QSqlError &error) const;

    ///
    /// \brief Checks if given room UUID is unique.
    /// \param roomUUID The room UUID to check.
    /// \param error The SQL error if any.
    /// \return \c true if the room UUID does not exit in the database.
    ///
    bool roomUuidUnique(const QString &roomUUID, QSqlError &error) const;

    ///
    /// \brief Checks if given room number is unique.
    /// \param roomNumber The room number to check.
    /// \param error The SQL error if any.
    /// \return \c true if the room number does not exist in the database.
    ///
    bool roomNumberUnique(const QString &roomNumber, QSqlError &error) const;

    ///
    /// \brief Checks if given course code is unique.
    /// \param courseCode The course code to check.
    /// \param error The SQL error if any.
    /// \return \c true if the course code does not exist in the database.
    ///
    bool courseCodeUnique(const QString &courseCode, QSqlError &error) const;

    ///
    /// \brief Checks if given index number is unique.
    /// \param indexNumber The index number to check.
    /// \param error The SQL error if any.
    /// \return \c true if the index number does not exist in the database.
    ///
    bool indexNumberUnique(const QString &indexNumber, QSqlError &error) const;

    ///
    /// \brief Checks if given employee number is unique.
    /// \param employeeNumber The employee number to check.
    /// \param error The SQL error if any.
    /// \return \c true if the employee number does not exist in the database.
    ///
    bool employeeNumberUnique(const QString &employeeNumber,
                              QSqlError &error) const;

    ///
    /// \brief listNameUnique check is given list name is unique.
    /// \param listName The list name to check.
    /// \param error The SQL error if any.
    /// \return \c true if the list name does not exist in the database.
    ///
    bool listNameUnique(const QString &listName, QSqlError &error) const;

    ///
    /// \brief importCourse imports course from given CSV line.
    /// \param csvLine The CSV line with course data.
    /// \param error The SQL error if any.
    /// \return The course import error.
    ///
    data::CourseImportError importCourse(const QString &csvLine,
                                         QSqlError &error) const;

    ///
    /// \brief importStudent imports student from given CSV line.
    /// \param csvLine The CSV line with student data.
    /// \param error The SQL error if any.
    /// \return The student import error.
    ///
    data::StudentImportError importStudent(const QString &csvLine,
                                           QSqlError &error) const;

    ///
    /// \brief importCourseStudent imports index number from the csv line and
    /// enlists that student to the given course.
    /// \note This method has no transaction of its own and should not be used
    /// without enclosing transaction, which must be initiated by the caller.
    /// \param courseCode The course code.
    /// \param csvLine The CVS line with student data. Index number must be
    /// first.
    /// \param error The SQL error if any.
    /// \return The import error.
    ///
    data::ListStudentImportError importCourseStudent(const QString &courseCode,
                                                     const QString &csvLine,
                                                     QSqlError &error) const;

    ///
    /// \brief importListStudent imports intex number from the csv line and
    /// enlists that student to the given course.
    /// \note This method has no transaction of its own and should not be used
    /// without enclosing transaction, which must be initiated by the caller.
    /// \param listId The list id.
    /// \param csvLine The CSV line with student data. Index number must be
    /// first.
    /// \param error The SQL error if any.
    /// \return The import error.
    ///
    data::ListStudentImportError
    importListStudent(quint64 listId, const QString &csvLine, QSqlError &error);

    ///
    /// \brief getCourseStudents Returns all students that are enrolled to a
    /// course.
    /// \param courseCode The course code.
    /// \param error The SQL error if any.
    /// \return The students that are enrolled to a course.
    ///
    std::shared_ptr<std::vector<data::entity::Student>>
    getCourseStudents(const QString &courseCode, QSqlError &error) const;

    ///
    /// \brief getStudentCourses Returns all courses that student is enrolled
    /// to.
    /// \param indexNumber The index number.
    /// \param error The SQL error if any.
    /// \return The courses that student is enrolled to.
    ///
    data::entity::EntityVector getStudentCourses(const QString &indexNumber,
                                                 QSqlError &error) const;

    ///
    /// \brief studentLists Returns all lists that given student is member of.
    /// \param indexNumber The index number.
    /// \param error The SQL error if any.
    /// \return The lists that have given student as member.
    ///
    data::entity::EntityVector studentLists(const QString &indexNumber,
                                            QSqlError &error) const;

    ///
    /// \brief enlistStudentsToCourse Enlists students to the course.
    /// \param courseCode The course code.
    /// \param indexNumbers The list of students index numbers.
    /// \param error The SQL error if any.
    /// \return \c true if students are successfully enlisted.
    ///
    bool enlistStudentsToCourse(const QString &courseCode,
                                const QStringList &indexNumbers,
                                QSqlError &error) const;

    ///
    /// \brief enlistStudentToCourses Enlists student to courses.
    /// \param indexNumber The student's index number.
    /// \param courseCodes The list of course codes to enlist student to.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully enlisted.
    ///
    bool enlistStudentToCourses(const QString &indexNumber,
                                const QStringList &courseCodes,
                                QSqlError &error) const;

    ///
    /// \brief removeStudentsFromCourse Removes students from given course.
    /// \param courseCode The course code.
    /// \param indexNumbers The list of stundents index numbers.
    /// \param error The SQL error if any.
    /// \return \c true if students are successfully removed from the course.
    ///
    bool removeStudentsFromCourse(const QString &courseCode,
                                  const QStringList &indexNumbers,
                                  QSqlError &error) const;

    ///
    /// \brief removeAllStudentsFromCourse Removes all students from given
    /// course.
    /// \note This method has no transaction of its own and should not be used
    /// without enclosing transaction, which must be initiated by the caller.
    /// \param courseCode The course code.
    /// \param error The SQL error if any.
    /// \return \c true if all students are successfully removed from the
    /// course.
    ///
    bool removeAllStudentsFromCourse(const QString &courseCode,
                                     QSqlError &error);

    ///
    /// \brief removeStudentFromCourses Removes student from given courses.
    /// \param indexNumber The index number.
    /// \param courseCodes The list of course codes.
    /// \param error The SQL error if any.
    /// \return \c true if student is succesfully removed from courses.
    ///
    bool removeStudentFromCourses(const QString &indexNumber,
                                  const QStringList &courseCodes,
                                  QSqlError &error) const;

    ///
    /// Adds students from \c addIndexNumbers to the course and removes ones
    /// from \c removeIndexNumbers from the course.
    /// \param courseCode The course code.
    /// \param addIndexNumbers The index numbers of students to add to the
    /// course.
    /// \param removeIndexNumbers The index numbers of students to remove from
    /// the course.
    /// \param error The SQL error if any.
    /// \return \c true if operation was successful.
    ///
    bool updateCourseStudents(const QString &courseCode,
                              const QStringList &addIndexNumbers,
                              const QStringList &removeIndexNumbers,
                              QSqlError &error) const;

    ///
    /// \brief studentsEnlistedToCourse Returns all students that are enlisted
    /// to given course.
    /// \param courseCode The course code.
    /// \param error The SQL error if any.
    /// \return Vector of students that are enlisted to given course.
    ///
    data::entity::EntityVector
    studentsEnlistedToCourse(const QString &courseCode, QSqlError &error) const;

    ///
    /// \brief studentsNotEnlistedToCourse Returns all student that are not
    /// enlisted to given course.
    /// \param courseCode The course code.
    /// \param error The SQL error if any.
    /// \return Vector of students that are not enlisted to given course.
    ///
    data::entity::EntityVector
    studentsNotEnlistedToCourse(const QString &courseCode,
                                QSqlError &error) const;

    ///
    /// \brief addStudentsToList Adds students to the list.
    /// \param listId The list id.
    /// \param indexNumbers The list of students index numbers.
    /// \param error The SQL error if any.
    /// \return \c true if students are successfully added.
    ///
    bool addStudentsToList(quint64 listId, const QStringList &indexNumbers,
                           QSqlError &error) const;

    ///
    /// \brief removeStudentsFromList Removes students from the list.
    /// \param listId The list id.
    /// \param indexNumbers The list of students index numbers.
    /// \param error The SQL error if any.
    /// \return \c true if students are successfully removed.
    ///
    bool removeStudentsFromList(quint64 listId, const QStringList &indexNumbers,
                                QSqlError &error) const;
    ///
    /// \brief updateListStudents Adds students from \c addIndexNumbers and
    /// removes ones from \c removeIndexNumbers from the list.
    /// \param listId The list id.
    /// \param addIndexNumbers The index numbers of students to add to the list.
    /// \param removeIndexNumbers The index numbers of the students to remove
    /// from the list.
    /// \param error The SQL error if any.
    /// \return \c true if operation was successful.
    ///
    bool updateListStudents(quint64 listId, const QStringList &addIndexNumbers,
                            const QStringList &removeIndexNumbers,
                            QSqlError &error) const;
    ///
    /// \brief membersOfTheList Returns all members of the given list.
    /// \param listId The list id.
    /// \param error The SQL error if any.
    /// \return Vector of students that are members of the list.
    ///
    data::entity::EntityVector membersOfTheList(quint64 listId,
                                                QSqlError &error) const;

    ///
    /// \brief nonMembersOfTheList Returns all students that are not members of
    /// the given list.
    /// \param listId The list id.
    /// \param error The SQL error if any.
    /// \return Vector of students that are not members of the list.
    ///
    data::entity::EntityVector nonMembersOfTheList(quint64 listId,
                                                   QSqlError &error) const;

    ///
    /// \brief getList Returns list with given name.
    /// \param name The list name.
    /// \param error The SQL error if any.
    /// \return The list or \c nullptr if list with given name does not exist.
    ///
    std::shared_ptr<paso::data::entity::List> getList(const QString &name,
                                                      QSqlError &error) const;

    ///
    /// \brief Removes all students from given list.
    /// \note This method has no transaction of its own and should not be used
    /// without enclosing transaction, which must be initiated by the caller.
    /// \param listId The list id.
    /// \param error The SQL error if any.
    /// \return \c true if all students are successfully removed from the list.
    ///
    bool removeAllStudentsFromList(quint64 listId, QSqlError &error);

    ///
    /// \brief Returns activity with given id.
    /// \param activityId The activity id.
    /// \param error The SQL error if any.
    /// \return Found activity of \c nullptr if such activity does not exist.
    ///
    std::shared_ptr<data::entity::Activity> getActivity(quint64 activityId,
                                                        QSqlError &error) const;

    ///
    /// \brief Adds new or updates existing activity. If adding new activity the
    /// id property will be updated.
    /// \param activity The activity.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully saved.
    ///
    bool saveActivity(data::entity::Activity &activity, QSqlError &error) const;

    ///
    /// \brief Deletes activity with given \c activityId. Deleting non existing
    /// activity will not result in error.
    /// \param activityId The id of activity to delete.
    /// \param error The SQL error if any.
    /// \return \c true if activity is successfuly deleted.
    ///
    bool deleteActivity(quint64 activityId, QSqlError &error) const;

    ///
    /// \brief Returns all lists associated with activity with given id.
    /// \param activityId The activity id.
    /// \param error The SQL error if any.
    /// \return Vector of lists associated with activity with given id.
    ///
    data::entity::EntityVector activityLists(quint64 activityId,
                                             QSqlError &error) const;

    ///
    /// \brief Returns all lists that are not associated with activity with
    /// given id.
    /// \param activityId The activity id.
    /// \param error The SQL error if any.
    /// \return Vector of lists that are not associated with activity with given
    /// id.
    ///
    data::entity::EntityVector nonActivityLists(quint64 activityId,
                                                QSqlError &error) const;

    ///
    /// \brief Associates given lists with activity.
    /// \note Previous associations will be deleted.
    /// \param activityId The activity id.
    /// \param listIds The list of list ids.
    /// \param error The SQL error if any.
    /// \return \c true if lists are associated.
    ///
    bool setActivityLists(quint64 activityId, const QList<quint64> &listIds,
                          QSqlError &error) const;

    ///
    /// \brief Returns all rooms associated with activity with given id.
    /// \param activityId The activity id.
    /// \param error The SQL error if any.
    /// \return Vector of rooms associated with activity with given id.
    ///
    data::entity::EntityVector activityRooms(quint64 activityId,
                                             QSqlError &error) const;

    ///
    /// \brief Returns all rooms that are not associated with activity with
    /// given id.
    /// \param activityId The activity id.
    /// \param error The SQL error if any.
    /// \return Vector of rooms that are not associated with activity with given
    /// id.
    ///
    data::entity::EntityVector nonActivityRooms(quint64 activityId,
                                                QSqlError &error) const;

    ///
    /// \brief Associates given rooms with activity.
    /// \note Previous associations will be deleted.
    /// \param activityId The activity id.
    /// \param roomIds The list of room ids.
    /// \param error The SQL error if any.
    /// \return \c true if rooms are associated.
    ///
    bool setActivityRooms(quint64 activityId, const QList<quint64> &roomIds,
                          QSqlError &error) const;

    ///
    /// \brief Returns list of students barred from the given room.
    /// \param roomId The room id.
    /// \param error The SQL error if any.
    /// \return Vector of students barred from the given room.
    ///
    data::entity::EntityVector barredStudents(quint64 roomId,
                                              QSqlError &error) const;

    ///
    /// \brief Returns list of students that are potentially allowed to enter
    /// given room.
    /// \param roomId The room id.
    /// \param error The SQL error if any.
    /// \return Vector of students that are potentially allowed to enter the
    /// room.
    ///
    data::entity::EntityVector allowedStudents(quint64 roomId,
                                               QSqlError &error) const;

    ///
    /// \brief Returns all teachers RFID data to be used by controllers.
    /// \param error The SQL error if any.
    /// \return List of all teachers RFID data.
    ///
    QStringList emergencyData(QSqlError &error) const;

    ///
    /// \brief Checks whether person should be grated to the room. If user was
    /// granted with access the entry will be recorded.
    /// \param roomUUID The room UUID.
    /// \param rfid The person's RFID.
    /// \param teachersOnly Allow access only to the teachers.
    /// \param error The SQL error if any.
    /// \return \c true if access was granted.
    ///
    bool checkAccess(const QUuid &roomUUID, const QString &rfid, bool teachersOnly,
                     QSqlError &error) const;

private:
    const QString mDbName;
};
}
}

#endif // PASODB_H
