#include "teacher.h"

#include <typeinfo>

namespace paso {
namespace data {
namespace entity {

Teacher::Teacher(const QString &firstName, const QString &lastName,
                 const QString &email, const QString &employeeNumber,
                 const QString &office, quint64 id, const QString &rfid)
    : Person(firstName, lastName, email, id, rfid),
      mEmployeeNumber(employeeNumber), mOffice(office) {}

Teacher::Teacher(const QVariantMap &map)
    : Person(map), mEmployeeNumber(map["EMPLOYEE_NUMBER"].toString()),
      mOffice(map["OFFICE"].toString()) {}

bool Teacher::operator==(const Person &other) const {
    try {
        const Teacher &otherTeacher = dynamic_cast<const Teacher &>(other);
        return Person::operator==(other) &&
               mEmployeeNumber == otherTeacher.mEmployeeNumber &&
               mOffice == otherTeacher.mOffice;
    } catch (std::bad_cast &) {
        return false;
    }
}

QString Teacher::employeeNumber() const { return mEmployeeNumber; }

void Teacher::setEmployeeNumber(const QString &employeeNumber) {
    mEmployeeNumber = employeeNumber;
}

QString Teacher::office() const { return mOffice; }

void Teacher::setOffice(const QString &office) { mOffice = office; }

QVariantMap Teacher::toVariantMap() const {
    auto retVal = Person::toVariantMap();
    retVal.insert("EMPLOYEE_NUMBER", mEmployeeNumber);
    retVal.insert("OFFICE", mOffice);
    return retVal;
}

QVariant Teacher::value(const QString &property) const {
    if (property == "EMPLOYEE_NUMBER") {
        return mEmployeeNumber;
    } else if (property == "OFFICE") {
        return mOffice;
    }

    return Person::value(property);
}

void Teacher::read(const QJsonObject &jsonObject) {
    Person::read(jsonObject);
    mEmployeeNumber = jsonObject["EMPLOYEE_NUMBER"].toString();
    mOffice = jsonObject["OFFICE"].toString();
}

void Teacher::write(QJsonObject &jsonObject) const {
    Person::write(jsonObject);
    jsonObject["EMPLOYEE_NUMBER"] = mEmployeeNumber;
    jsonObject["OFFICE"] = mOffice;
}

QSqlQuery Teacher::insertQuery(const QSqlDatabase &database,
                               const Teacher &teacher) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO TEACHER (ID, EMPLOYEE_NUMBER, OFFICE) "
                  "VALUES (:id, :employee_number, :office)");
    query.bindValue(":id", teacher.id());
    query.bindValue(":employee_number", teacher.employeeNumber());
    query.bindValue(":office", teacher.office());
    return query;
}

QSqlQuery Teacher::updateQuery(const QSqlDatabase &database,
                               const Teacher &teacher) {
    QSqlQuery query(database);
    query.prepare("UPDATE TEACHER SET "
                  "EMPLOYEE_NUMBER = :employee_number, "
                  "OFFICE = :office "
                  "WHERE ID = :id");
    query.bindValue(":id", teacher.id());
    query.bindValue(":employee_number", teacher.employeeNumber());
    query.bindValue(":office", teacher.office());
    return query;
}

QSqlQuery Teacher::deleteQuery(const QSqlDatabase &database,
                               const QString &employeeNumber) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM PERSON"
                  " WHERE ID = (SELECT ID"
                  "               FROM TEACHER"
                  "              WHERE EMPLOYEE_NUMBER = :employee_number)");
    query.bindValue(":employee_number", employeeNumber);
    return query;
}

QSqlQuery Teacher::findByEmployeeNumberQuery(const QSqlDatabase &database,
                                             const QString &employeeNumber) {
    QSqlQuery query(database);
    query.prepare("SELECT P.ID, P.LAST_NAME, P.FIRST_NAME, P.EMAIL, P.RFID, "
                  "T.EMPLOYEE_NUMBER, T.OFFICE "
                  "FROM PERSON P JOIN TEACHER T USING (ID) "
                  "WHERE T.EMPLOYEE_NUMBER = :employee_number");
    query.bindValue(":employee_number", employeeNumber);
    return query;
}

QSqlQuery Teacher::emergencyDataQuery(const QSqlDatabase &database) {
    QSqlQuery query(database);
    query.prepare("SELECT P.RFID "
                  "  FROM PERSON P "
                  "  JOIN TEACHER T USING(ID) "
                  " WHERE P.RFID IS NOT NULL");
    return query;
}
}
}
}
