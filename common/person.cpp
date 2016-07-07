#include "person.h"

namespace paso {
namespace data {
namespace entity {

Person::Person(const QString &firstName, const QString &lastName,
               const QString &email, uint64_t id, const QString &rfid)
    : Entity(id), mFirstName(firstName), mLastName(lastName), mEmail(email),
      mRFID(rfid) {}

Person::Person(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mFirstName(map["FIRST_NAME"].toString()),
      mLastName(map["LAST_NAME"].toString()), mEmail(map["EMAIL"].toString()),
      mRFID(map["RFID"].toString()) {}

bool Person::operator==(const Person &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mFirstName == other.firstName() &&
           mLastName == other.mLastName && mEmail == other.email() &&
           mRFID == other.mRFID;
}

QString Person::firstName() const { return mFirstName; }

void Person::setFirstName(const QString &firstName) { mFirstName = firstName; }

QString Person::lastName() const { return mLastName; }

void Person::setLastName(const QString &lastName) { mLastName = lastName; }

QString Person::email() const { return mEmail; }

void Person::setEmail(const QString &email) { mEmail = email; }

QString Person::rfid() const { return mRFID; }

void Person::setRfid(const QString &rfid) { mRFID = rfid; }

void Person::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mFirstName = jsonObject["FIRST_NAME"].toString();
    mLastName = jsonObject["LAST_NAME"].toString();
    mEmail = jsonObject["EMAIL"].toString();
    mRFID = jsonObject["RFID"].toString();
}

void Person::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["FIRST_NAME"] = mFirstName;
    jsonObject["LAST_NAME"] = mLastName;
    jsonObject["EMAIL"] = mEmail;
    jsonObject["RFID"] = mRFID;
}

QSqlQuery Person::insertQuery(const QSqlDatabase &database,
                              const Person &person) {
    QSqlQuery query(database);
    query.prepare(
        "INSERT INTO PERSON (FIRST_NAME, LAST_NAME, EMAIL, RFID) VALUES "
        "(:first_name, :last_name, :email, :rfid)");
    query.bindValue(":first_name", person.firstName());
    query.bindValue(":last_name", person.lastName());
    query.bindValue(":email", person.email());
    if (person.rfid().isEmpty()) {
        query.bindValue(":rfid", QVariant(QVariant::String));
    } else {
        query.bindValue(":rfid", person.rfid());
    }
    return query;
}

QSqlQuery Person::updateQuery(const QSqlDatabase &database,
                              const Person &person) {
    QSqlQuery query(database);
    query.prepare("UPDATE PERSON SET "
                  "FIRST_NAME = :first_name, "
                  "LAST_NAME = :last_name, "
                  "EMAIL = :email, "
                  "RFID = :rfid "
                  "WHERE ID = :id");
    query.bindValue(":id", static_cast<quint64>(person.id()));
    query.bindValue(":first_name", person.firstName());
    query.bindValue(":last_name", person.lastName());
    query.bindValue(":email", person.email());
    if (person.rfid().isEmpty()) {
        query.bindValue(":rfid", QVariant(QVariant::String));
    } else {
        query.bindValue(":rfid", person.rfid());
    }
    return query;
}

QSqlQuery Person::deleteQuery(const QSqlDatabase &database, uint64_t id) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM PERSON WHERE ID = :id");
    query.bindValue(":id", static_cast<quint64>(id));
    return query;
}

}
}
}
