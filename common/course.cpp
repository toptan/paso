#include "course.h"

#include <QDebug>
#include <QString>
#include <QSqlError>
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
    query.bindValue(":id", course.id());
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

QSqlQuery Course::findByCodeQuery(const QSqlDatabase &database, const QString &code) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM COURSE WHERE CODE = :code");
    query.bindValue(":code", code);
    return query;
}

}
}
}
