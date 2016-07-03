#include "student.h"

namespace paso {
namespace data {
namespace entity {

Student::Student(const QString &firstName, const QString &lastName,
                 const QString &email, const QString &indexNumber,
                 uint8_t yearOfStudy, uint64_t id, const QString &rfid)
    : Person(firstName, lastName, email, id, rfid), mIndexNumber(indexNumber),
      mYearOfStudy(yearOfStudy) {}

Student::Student(const QVariantMap &map)
    : Person(map), mIndexNumber(map["INDEX_NUMBER"].toString()),
      mYearOfStudy(map["YEAR_OF_STUDY"].toInt()) {}

bool Student::operator==(const Person &other) const {
    try {
        const Student &otherStudent = dynamic_cast<const Student &>(other);
        return Person::operator==(other) &&
               mIndexNumber == otherStudent.mIndexNumber &&
               mYearOfStudy == otherStudent.mYearOfStudy;
    } catch (std::bad_cast &) {
        return false;
    }
}

QString Student::indexNumber() const { return mIndexNumber; }

void Student::setIndexNumber(const QString &indexNumber) {
    mIndexNumber = indexNumber;
}

int Student::yearOfStudy() const { return mYearOfStudy; }

void Student::setYearOfStudy(const int yearOfStudy) {
    mYearOfStudy = yearOfStudy;
}

void Student::read(const QJsonObject &jsonObject) {
    Person::read(jsonObject);
    mIndexNumber = jsonObject["INDEX_NUMBER"].toString();
    mYearOfStudy = jsonObject["YEAR_OF_STUDY"].toInt();
}

void Student::write(QJsonObject &jsonObject) const {
    Person::write(jsonObject);
    jsonObject["INDEX_NUMBER"] = mIndexNumber;
    jsonObject["YEAR_OF_STUDY"] = mYearOfStudy;
}

QSqlQuery Student::insertQuery(const QSqlDatabase &database,
                               const Student &student) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO STUDENT (ID, INDEX_NUMBER, YEAR_OF_STUDY) "
                  "VALUES (:id, :index_number, :year_of_study)");
    query.bindValue("id:", student.id());
    query.bindValue(":index_number", student.indexNumber());
    query.bindValue(":year_of_study", student.yearOfStudy());
    return query;
}

QSqlQuery Student::updateQuery(const QSqlDatabase &database,
                               const Student &student) {
    QSqlQuery query(database);
    query.prepare("UPDATE STUDENT SET "
                  "INDEX_NUMBER = :index_number, "
                  "YEAR_OF_STUDY = :year_of_study "
                  "WHERE ID = :id");
    query.bindValue(":id", student.id());
    query.bindValue(":index_number", student.indexNumber());
    query.bindValue(":year_of_study", student.yearOfStudy());
    return query;
}

QSqlQuery Student::findByIndexNumberQuery(const QSqlDatabase &database,
                                          const QString &indexNumber) {
    QSqlQuery query(database);
    query.prepare("SELECT P.ID, P.FIRST_NAME, P.LAST_NAME, P.EMAIL, P.RFID, "
                  "S.INDEX_NUMBER, S.YEAR_OF_STUDY "
                  "FROM PERSON P JOIN STUDENT S USING (ID) "
                  "WHERE S.INDEX_NUMBER = :index_number");
    query.bindValue(":index_number", indexNumber);
    return query;
}
}
}
}
