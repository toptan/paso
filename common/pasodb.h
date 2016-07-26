#ifndef PASODB_H
#define PASODB_H

#include "course.h"
#include "data.h"
#include "entity.h"
#include "list.h"
#include "room.h"
#include "student.h"
#include "systemuser.h"

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

enum class CourseImportError {
    NO_ERROR,      //!< All fine, no error.
    INVALID_LINE,  //!< The CSV line is illformed.
    NO_CODE,       //!< Missing course code.
    CODE_TOO_LONG, //!< Course code too long.
    NO_NAME,       //!< Missing course name.
    NAME_TOO_LONG, //!< Course name too long.
    DB_ERROR       //!< Generic database error.
};

enum class StudentImportError {
    NO_ERROR,            //!< All fine, no error.
    INVALID_LINE,        //!< The CSV line is illformed.
    NO_INDEX_NUMBER,     //!< Missing index number.
    BAD_INDEX_NUMBER,    //!< The index number is wrong.
    NO_FIRST_NAME,       //!< Missing first name.
    FIRST_NAME_TOO_LONG, //!< First name is too long.
    NO_LAST_NAME,        //!< Missing last name.
    LAST_NAME_TOO_LONG,  //!< Last name is too long.
    BAD_EMAIL,           //!< The email is bad.
    NO_YEAR_OF_STUDY,    //!< Year of study missing.
    BAD_YEAR_OF_STUDY,   //<! The year of study is bad.
    DB_ERROR             //!< Generic database error.
};

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
    /// \brief getRoom Returns room with given \c roomUUID
    /// \param [in]  roomUUID The room UUID to look for.
    /// \param [out] error SQL error if any.
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
    /// \brief saveStudent Adds new or updates existing student. If adding new
    /// student the id property of the student will be updated.
    /// \param student The student.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully saved.
    ///
    bool saveStudent(data::entity::Student &student, QSqlError &error) const;

    ///
    /// \brief deleteStudent Deletes student with given \c indexNumber. Deleting
    /// non existant student will not result in error.
    /// \param indexNumber The index number of the student to delete.
    /// \param error The SQL error if any.
    /// \return \c true if student is successfully deleted.
    ///
    bool deleteStudent(const QString &indexNumber, QSqlError &error) const;

    ///
    /// \brief usernameUnique checks if given username is unique.
    /// \param [in] username The username to check.
    /// \param [out] error The SQL error if any.
    /// \return \c true if username does not exist in the database.
    ///
    bool usernameUnique(const QString &username, QSqlError &error) const;

    ///
    /// \brief roomUuidUnique checks if given room UUID is unique.
    /// \param [in] roomUUID The room UUID to check.
    /// \param [out] error The SQL error if any.
    /// \return \c true if the room UUID does not exit in the database.
    ///
    bool roomUuidUnique(const QString &roomUUID, QSqlError &error) const;

    ///
    /// \brief roomNumberUnique checks if given room number is unique.
    /// \param [in] roomNumber The room number to check.
    /// \param [out] error The SQL error if any.
    /// \return \c true if the room number does not exist in the database.
    ///
    bool roomNumberUnique(const QString &roomNumber, QSqlError &error) const;

    ///
    /// \brief courseCodeUnique checks if given course code is unique.
    /// \param [in] courseCode The course code to check.
    /// \param [out] error The SQL error if any.
    /// \return \c true if the course code does not exist in the database.
    ///
    bool courseCodeUnique(const QString &courseCode, QSqlError &error) const;

    ///
    /// \brief indexNumberUnique checks if given index number is unique.
    /// \param indexNumber The index number to check.
    /// \param error The SQL error if any.
    /// \return \c true if the index number does not exist in the database.
    ///
    bool indexNumberUnique(const QString &indexNumber, QSqlError &error) const;

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
    CourseImportError importCourse(const QString &csvLine,
                                   QSqlError &error) const;

    ///
    /// \brief importStudent imports student from given CSV line.
    /// \param csvLine The CSV line with student data.
    /// \param error The SQL error if any.
    /// \return The student import error.
    ///
    StudentImportError importStudent(const QString &csvLine,
                                     QSqlError &error) const;

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
    std::shared_ptr<std::vector<data::entity::Course>>
    getStudentCourses(const QString &indexNumber, QSqlError &error) const;

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
    /// Adds students from \c addIndexNumber to the course and removes ones from
    /// \c removeIndexNumbers from the course.
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

private:
    const QString mDbName;
};
}
}

#endif // PASODB_H
