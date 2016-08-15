#include "pasodb.h"

#include <QDate>
#include <QDebug>
#include <QRegExp>
#include <QSqlError>
#include <QSqlRecord>
#include <QSqlResult>
#include <QUuid>

namespace paso {
namespace db {

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;

const QVariantMap DBManager::recordToVariantMap(const QSqlRecord &record) {
    QVariantMap retVal;
    for (auto i = 0; i < record.count(); i++) {
        retVal.insert(record.fieldName(i).toUpper(), record.value(i));
    }
    return retVal;
}

DBManager::DBManager(const QString &dbName)
    : QObject(nullptr), mDbName(dbName) {
    QSqlError error;
    auto root = getSystemUser("root", error);
    if (!root) {
        root =
            make_shared<SystemUser>("root", "root_password", "Root", "Rootovic",
                                    "root@paso.com", SystemRole::SUPER_USER);
        saveSystemUser(*root, error);
    }
}

void DBManager::beginTransaction() const {
    QSqlDatabase::database(mDbName).transaction();
}

QSqlError DBManager::commit() const {
    auto db = QSqlDatabase::database(mDbName);
    db.commit();
    return db.lastError();
}

void DBManager::rollback() const { QSqlDatabase::database(mDbName).rollback(); }

shared_ptr<vector<SystemUser>>
DBManager::getAllSystemUsers(QSqlError &error) const {
    auto query = SystemUser::findAllQuery(QSqlDatabase::database(mDbName));
    query.exec();
    error = query.lastError();
    auto retVal = make_shared<vector<SystemUser>>();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<SystemUser> DBManager::getSystemUser(const QString &username,
                                                QSqlError &error) const {
    auto query = SystemUser::findByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<SystemUser>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveSystemUser(SystemUser &user, QSqlError &error) const {
    beginTransaction();
    auto query =
        user.id() == 0
            ? SystemUser::insertQuery(QSqlDatabase::database(mDbName), user)
            : SystemUser::updateQuery(QSqlDatabase::database(mDbName), user);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    if (user.id() == 0) {
        user.setId(query.lastInsertId().toULongLong());
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteSystemUser(const QString &username,
                                 QSqlError &error) const {
    if (username == "root") {
        return false;
    }
    beginTransaction();
    auto query = SystemUser::deleteByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    error = commit();
    return error.type() == QSqlError::NoError;
}

shared_ptr<vector<Room>> DBManager::getAllRooms(QSqlError &error) const {
    auto query = Room::findAllQuery(QSqlDatabase::database(mDbName));
    auto retVal = make_shared<vector<Room>>();
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<Room> DBManager::getRoom(const QUuid &roomUUID,
                                    QSqlError &error) const {
    auto query =
        Room::findByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Room>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveRoom(Room &room, QSqlError &error) const {
    auto query = room.id() == 0
                     ? Room::insertQuery(QSqlDatabase::database(mDbName), room)
                     : Room::updateQuery(QSqlDatabase::database(mDbName), room);
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    if (room.id() == 0) {
        room.setId(query.lastInsertId().toULongLong());
    }
    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteRoom(const QUuid &roomUUID, QSqlError &error) const {
    auto query =
        Room::deleteByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    error = commit();
    return error.type() == QSqlError::NoError;
}

shared_ptr<Course> DBManager::getCourse(const QString &courseCode,
                                        QSqlError &error) const {
    auto query =
        Course::findByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Course>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveCourse(Course &course, QSqlError &error) const {
    auto query =
        course.id() == 0
            ? Course::insertQuery(QSqlDatabase::database(mDbName), course)
            : Course::updateQuery(QSqlDatabase::database(mDbName), course);
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    if (course.id() == 0) {
        course.setId(query.lastInsertId().toULongLong());
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteCourse(const QString &courseCode,
                             QSqlError &error) const {
    auto query =
        Course::deleteByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::usernameUnique(const QString &username,
                               QSqlError &error) const {
    auto query = SystemUser::findByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::roomUuidUnique(const QString &roomUUID,
                               QSqlError &error) const {
    auto query =
        Room::findByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::roomNumberUnique(const QString &roomNumber,
                                 QSqlError &error) const {
    auto query =
        Room::findByNumberQuery(QSqlDatabase::database(mDbName), roomNumber);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::courseCodeUnique(const QString &courseCode,
                                 QSqlError &error) const {
    auto query =
        Course::findByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::indexNumberUnique(const QString &indexNumber,
                                  QSqlError &error) const {
    auto query = Student::findByIndexNumberQuery(
        QSqlDatabase::database(mDbName), indexNumber);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::listNameUnique(const QString &listName,
                               QSqlError &error) const {
    auto query =
        List::findByNameQuery(QSqlDatabase::database(mDbName), listName);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

shared_ptr<Student>
DBManager::getStudentByIndexNumber(const QString &indexNumber,
                                   QSqlError &error) const {
    auto query = Student::findByIndexNumberQuery(
        QSqlDatabase::database(mDbName), indexNumber);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Student>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveStudent(Student &student, QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    auto newStudent = student.id() == 0;
    auto query = newStudent ? Person::insertQuery(db, student)
                            : Person::updateQuery(db, student);
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    if (newStudent) {
        student.setId(query.lastInsertId().toULongLong());
    }
    query = newStudent ? Student::insertQuery(db, student)
                       : Student::updateQuery(db, student);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        if (newStudent) {
            student.setId(0);
        }
        rollback();
        return false;
    }
    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteStudent(const QString &indexNumber,
                              QSqlError &error) const {
    auto student = getStudentByIndexNumber(indexNumber, error);
    if (error.type() != QSqlError::NoError) {
        return false;
    }
    if (!student) {
        return true;
    }

    auto query =
        Person::deleteQuery(QSqlDatabase::database(mDbName), student->id());
    beginTransaction();
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        rollback();
        return false;
    }
    error = commit();
    return error.type() == QSqlError::NoError;
}

shared_ptr<vector<Student>>
DBManager::getCourseStudents(const QString &courseCode,
                             QSqlError &error) const {
    auto query = Course::findCourseStudentsQuery(
        QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    auto retVal = make_shared<vector<Student>>();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<vector<Course>>
DBManager::getStudentCourses(const QString &indexNumber,
                             QSqlError &error) const {
    auto query = Student::findStudentCoursesQuery(
        QSqlDatabase::database(mDbName), indexNumber);
    query.exec();
    error = query.lastError();
    auto retVal = make_shared<vector<Course>>();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

bool DBManager::enlistStudentsToCourse(const QString &courseCode,
                                       const QStringList &indexNumbers,
                                       QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    beginTransaction();
    for (const auto &indexNumber : indexNumbers) {
        auto query =
            Course::enlistStudentToCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::enlistStudentToCourses(const QString &indexNumber,
                                       const QStringList &courseCodes,
                                       QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    beginTransaction();
    for (const auto &courseCode : courseCodes) {
        auto query =
            Course::enlistStudentToCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::updateCourseStudents(const QString &courseCode,
                                     const QStringList &addIndexNumbers,
                                     const QStringList &removeIndexNumbers,
                                     QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    beginTransaction();
    for (const auto &indexNumber : addIndexNumbers) {
        auto query =
            Course::enlistStudentToCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    for (const auto &indexNumber : removeIndexNumbers) {
        auto query =
            Course::removeStudentFromCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::removeStudentFromCourses(const QString &indexNumber,
                                         const QStringList &courseCodes,
                                         QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    beginTransaction();
    for (const auto &courseCode : courseCodes) {
        auto query =
            Course::removeStudentFromCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::removeStudentsFromCourse(const QString &courseCode,
                                         const QStringList &indexNumbers,
                                         QSqlError &error) const {
    auto db = QSqlDatabase::database(mDbName);
    beginTransaction();
    for (const auto &indexNumber : indexNumbers) {
        auto query =
            Course::removeStudentFromCourseQuery(db, courseCode, indexNumber);
        query.exec();
        error = query.lastError();
        if (error.type() != QSqlError::NoError) {
            rollback();
            return false;
        }
    }

    error = commit();
    return error.type() == QSqlError::NoError;
}

bool DBManager::removeAllStudentsFromCourse(const QString &courseCode,
                                            QSqlError &error) {
    auto db = QSqlDatabase::database(mDbName);
    auto query = Course::removeAllStudentsFromCourseQuery(db, courseCode);
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

CourseImportError DBManager::importCourse(const QString &csvLine,
                                          QSqlError &error) const {
    QString noQuotes = csvLine;
    noQuotes.replace("\"", "");
    auto segments = noQuotes.split(",");
    if (segments.length() < 2) {
        return CourseImportError::INVALID_LINE;
    }
    auto code = segments[0].trimmed();
    auto name = segments[1].trimmed();
    if (code.isEmpty()) {
        return CourseImportError::NO_CODE;
    }
    if (code.length() > 8) {
        return CourseImportError::CODE_TOO_LONG;
    }
    if (name.isEmpty()) {
        return CourseImportError::NO_NAME;
    }
    if (name.length() > 64) {
        return CourseImportError::NAME_TOO_LONG;
    }
    auto course = getCourse(code, error);
    if (error.type() != QSqlError::NoError) {
        return CourseImportError::DB_ERROR;
    }
    if (course) {
        course->setName(name);
    } else {
        course = make_shared<Course>(code, name);
    }
    if (!saveCourse(*course, error)) {
        return CourseImportError::DB_ERROR;
    }
    return CourseImportError::NO_ERROR;
}

StudentImportError DBManager::importStudent(const QString &csvLine,
                                            QSqlError &error) const {
    QString noQuotes = csvLine;
    noQuotes.replace("\"", "");
    auto segments = noQuotes.split(",");
    if (segments.length() < 5) {
        return StudentImportError::INVALID_LINE;
    }
    auto indexNumber = segments[0].trimmed();
    auto lastName = segments[1].trimmed();
    auto firstName = segments[2].trimmed();
    auto email = segments[3].trimmed();
    auto yearOfStudy = segments[4].trimmed();

    if (indexNumber.isEmpty()) {
        return StudentImportError::NO_INDEX_NUMBER;
    }
    if (lastName.isEmpty()) {
        return StudentImportError::NO_LAST_NAME;
    }
    if (lastName.size() > 32) {
        return StudentImportError::LAST_NAME_TOO_LONG;
    }
    if (firstName.isEmpty()) {
        return StudentImportError::NO_FIRST_NAME;
    }
    if (firstName.size() > 32) {
        return StudentImportError::FIRST_NAME_TOO_LONG;
    }
    if (!email.isEmpty()) {
        QRegExp regExp("\\b[A-Z0-9._%+-]+@[A-Z0-9.-]+\\.[A-Z]{2,4}\\b",
                       Qt::CaseInsensitive);
        if (!regExp.exactMatch(email)) {
            return StudentImportError::BAD_EMAIL;
        }
    }
    if (yearOfStudy.isEmpty()) {
        return StudentImportError::NO_YEAR_OF_STUDY;
    }

    if (indexNumber.size() != 9 || indexNumber.count("/") != 1) {
        return StudentImportError::BAD_INDEX_NUMBER;
    }
    auto indexSegments = indexNumber.split("/");
    if (indexSegments[0].size() != 4 || indexSegments[1].size() != 4) {
        return StudentImportError::BAD_INDEX_NUMBER;
    }
    if (indexSegments[0].toInt() < 1990 ||
        indexSegments[0].toInt() > QDate::currentDate().year() ||
        indexSegments[1].toInt() < 1) {
        return StudentImportError::BAD_INDEX_NUMBER;
    }
    if (yearOfStudy.toInt() < 1 || yearOfStudy.toInt() > 7) {
        return StudentImportError::BAD_YEAR_OF_STUDY;
    }

    auto student = getStudentByIndexNumber(indexNumber, error);
    if (error.type() != QSqlError::NoError) {
        return StudentImportError::DB_ERROR;
    }
    if (student) {
        student->setFirstName(firstName);
        student->setLastName(lastName);
        student->setEmail(email);
        student->setYearOfStudy(yearOfStudy.toInt());
    } else {
        student = make_shared<Student>(firstName, lastName, email, indexNumber,
                                       yearOfStudy.toInt());
    }

    if (!saveStudent(*student, error)) {
        return StudentImportError::DB_ERROR;
    }

    return StudentImportError::NO_ERROR;
}

ListStudentImportError DBManager::importCourseStudent(const QString &courseCode,
                                                      const QString &csvLine,
                                                      QSqlError &error) const {
    QString noQuotes = csvLine;
    noQuotes.replace("\"", "");
    auto segments = noQuotes.split(",");
    auto indexNumber = segments[0].trimmed();
    if (indexNumber.isEmpty()) {
        return ListStudentImportError::BAD_INDEX_NUMBER;
    }
    if (indexNumber.size() != 9 || indexNumber.count("/") != 1) {
        return ListStudentImportError::BAD_INDEX_NUMBER;
    }
    auto indexSegments = indexNumber.split("/");
    if (indexSegments[0].size() != 4 || indexSegments[1].size() != 4) {
        return ListStudentImportError::BAD_INDEX_NUMBER;
    }
    if (indexSegments[0].toInt() < 1990 ||
        indexSegments[0].toInt() > QDate::currentDate().year() ||
        indexSegments[1].toInt() < 1) {
        return ListStudentImportError::BAD_INDEX_NUMBER;
    }

    auto student = getStudentByIndexNumber(indexNumber, error);
    if (error.type() != QSqlError::NoError) {
        return ListStudentImportError::DB_ERROR;
    }
    if (!student) {
        return ListStudentImportError::NON_EXISTING_STUDENT;
    }

    auto db = QSqlDatabase::database(mDbName);
    auto query =
        Course::enlistStudentToCourseQuery(db, courseCode, indexNumber);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError) {
        return ListStudentImportError::DB_ERROR;
    }

    return ListStudentImportError::NO_ERROR;
}

EntityVector DBManager::studentsEnlistedToCourse(const QString &courseCode,
                                                 QSqlError &error) const {
    auto query =
        Course::enlistedStudents(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    EntityVector retVal;
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal.emplace_back(
                make_shared<Student>(recordToVariantMap(query.record())));
        }
    }
    return retVal;
}

EntityVector DBManager::studentsNotEnlistedToCourse(const QString &courseCode,
                                                    QSqlError &error) const {
    auto query = Course::notEnlistedStudents(QSqlDatabase::database(mDbName),
                                             courseCode);
    query.exec();
    error = query.lastError();
    EntityVector retVal;
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal.emplace_back(
                make_shared<Student>(recordToVariantMap(query.record())));
        }
    }
    return retVal;
}
}
}
