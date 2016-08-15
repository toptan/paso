#include "course.h"

#include <QDebug>
#include <QSqlError>
#include <QString>
#include <QVariantMap>

namespace paso {
namespace data {
namespace entity {

Course::Course(const QString &code, const QString &name, uint64_t id)
    : Entity(id), mCode(code), mName(name) {}

Course::Course(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mCode(map["CODE"].toString()),
      mName(map["NAME"].toString()) {}

bool Course::operator==(const Course &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mCode == other.mCode && mName == other.mName;
}

QString Course::code() const { return mCode; }

void Course::setCode(const QString &code) { mCode = code; }

QString Course::name() const { return mName; }

void Course::setName(const QString &name) { mName = name; }

QVariantMap Course::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("CODE", mCode);
    retVal.insert("NAME", mName);
    return retVal;
}

QVariant Course::value(const QString &property) const {
    if (property == "CODE") {
        return mCode;
    } else if (property == "NAME") {
        return mName;
    }

    return Entity::value(property);
}

void Course::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mCode = jsonObject["CODE"].toString();
    mName = jsonObject["NAME"].toString();
}

void Course::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["CODE"] = mCode;
    jsonObject["NAME"] = mName;
}

QSqlQuery Course::insertQuery(const QSqlDatabase &database,
                              const Course &course) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO COURSE (CODE, NAME) VALUES (:code, :name)");
    query.bindValue(":code", course.code());
    query.bindValue(":name", course.name());
    return query;
}

QSqlQuery Course::updateQuery(const QSqlDatabase &database,
                              const Course &course) {
    QSqlQuery query(database);
    query.prepare(
        "UPDATE COURSE SET CODE = :code, NAME = :name WHERE ID = :id");
    query.bindValue(":id", static_cast<quint64>(course.id()));
    query.bindValue(":code", course.code());
    query.bindValue(":name", course.name());
    return query;
}

QSqlQuery Course::deleteByCodeQuery(const QSqlDatabase &database,
                                    const QString &code) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM COURSE WHERE CODE = :code");
    query.bindValue(":code", code);
    return query;
}

QSqlQuery Course::findByCodeQuery(const QSqlDatabase &database,
                                  const QString &code) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM COURSE WHERE CODE = :code");
    query.bindValue(":code", code);
    return query;
}

QSqlQuery Course::findCourseStudentsQuery(const QSqlDatabase &database,
                                          const QString &code) {
    QSqlQuery query(database);
    query.prepare("SELECT P.ID, P.LAST_NAME, P.FIRST_NAME, P.EMAIL, P.RFID, "
                  "       S.INDEX_NUMBER, S.YEAR_OF_STUDY "
                  "  FROM PERSON P JOIN STUDENT S USING(ID) "
                  "  JOIN ENLISTED E ON E.ID_STUDENT = S.ID "
                  "  JOIN COURSE C ON C.ID = E.ID_COURSE"
                  " WHERE C.CODE = :code");
    query.bindValue(":code", code);
    return query;
}

QSqlQuery Course::enlistStudentToCourseQuery(const QSqlDatabase &database,
                                             const QString &code,
                                             const QString &indexNumber) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO ENLISTED(ID_COURSE, ID_STUDENT) "
                  "SELECT C.ID, S.ID "
                  "  FROM COURSE C, STUDENT S "
                  " WHERE C.CODE = :code_1 "
                  "   AND S.INDEX_NUMBER = :index_number_1 "
                  "   AND (SELECT COUNT(1) "
                  "          FROM ENLISTED E "
                  "         WHERE E.ID_COURSE = C.ID "
                  "           AND E.ID_STUDENT = S.ID "
                  "           AND C.CODE = :code_2 "
                  "           AND S.INDEX_NUMBER = :index_number_2) = 0");
    query.bindValue(":code_1", code);
    query.bindValue(":index_number_1", indexNumber);
    query.bindValue(":code_2", code);
    query.bindValue(":index_number_2", indexNumber);
    return query;
}

QSqlQuery Course::removeStudentFromCourseQuery(const QSqlDatabase &database,
                                               const QString &code,
                                               const QString &indexNumber) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM ENLISTED"
                  " WHERE ID_COURSE = (SELECT ID"
                  "                      FROM COURSE"
                  "                     WHERE CODE = :code)"
                  "   AND ID_STUDENT = (SELECT ID"
                  "                       FROM STUDENT"
                  "                      WHERE INDEX_NUMBER = :index_number)");
    query.bindValue(":code", code);
    query.bindValue(":index_number", indexNumber);
    return query;
}

QSqlQuery Course::removeAllStudentsFromCourseQuery(const QSqlDatabase &database,
                                                   const QString &code) {
    QSqlQuery query(database);
    query.prepare(
        "DELETE FROM ENLISTED"
        " WHERE ID_COURSE = (SELECT ID"
        "					     FROM COURSE"
        "						WHERE CODE = :code)");
    query.bindValue(":code", code);
    return query;
}

QSqlQuery Course::enlistedStudents(const QSqlDatabase &database,
                                   const QString &code) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM ENLISTED_STUDENTS WHERE CODE = :code");
    query.bindValue(":code", code);

    return query;
}

QSqlQuery Course::notEnlistedStudents(const QSqlDatabase &database,
                                      const QString &code) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM ENLISTED_STUDENTS "
                  " WHERE COALESCE(CODE, 'X') <> :code_1 "
                  "  AND ID NOT IN (SELECT ID FROM ENLISTED_STUDENTS WHERE "
                  "CODE = :code_2)");
    query.bindValue(":code_1", code);
    query.bindValue(":code_2", code);

    return query;
}
}
}
}
