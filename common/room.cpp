#include "room.h"

#include "data.h"

#include <QTextStream>
#include <QVariantMap>

namespace paso {
namespace data {
namespace entity {

Room::Room(const QString &roomUUID, const QString &name, const QString &number,
           quint64 id)
    : Entity(id), mRoomUUID(roomUUID), mName(name), mNumber(number),
      mBarredIds() {}

Room::Room(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mRoomUUID(map["ROOM_UUID"].toString()),
      mName(map["NAME"].toString()), mNumber(map["ROOM_NUMBER"].toString()),
      mBarredIds(
          jsonArrayStringToVariantList(map["BARRED_STUDENTS"].toString())) {}

bool Room::operator==(const Room &other) const {
    if (this == &other) {
        return true;
    }

    return id() == other.id() && mRoomUUID == other.mRoomUUID &&
           mName == other.mName && mNumber == other.mNumber;
}

QString Room::roomUUID() const { return mRoomUUID; }

QString Room::name() const { return mName; }

void Room::setName(const QString &name) { mName = name; }

QString Room::number() const { return mNumber; }

void Room::setNumber(const QString &number) { mNumber = number; }

QVariantMap Room::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("ROOM_UUID", mRoomUUID);
    retVal.insert("NAME", mName);
    retVal.insert("ROOM_NUMBER", mNumber);
    retVal.insert("BARRED_STUDENTS", mBarredIds);
    return retVal;
}

QVariant Room::value(const QString &property) const {
    if (property == "ROOM_UUID") {
        return mRoomUUID;
    } else if (property == "NAME") {
        return mName;
    } else if (property == "ROOM_NUMBER") {
        return mNumber;
    } else if (property == "BARRED_STUDENTS") {
        return mBarredIds;
    }

    return Entity::value(property);
}

void Room::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mRoomUUID = jsonObject["ROOM_UUID"].toString();
    mName = jsonObject["NAME"].toString();
    mNumber = jsonObject["ROOM_NUMBER"].toString();
    auto tmp = jsonObject["BARRED_STUDENTS"].toString();
    mBarredIds = jsonArrayStringToVariantList(tmp);
}

void Room::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["ROOM_UUID"] = mRoomUUID;
    jsonObject["NAME"] = mName;
    jsonObject["ROOM_NUMBER"] = mNumber;
    jsonObject["BARRED_STUDENTS"] = variantListToJsonArrayString(mBarredIds);
}

QSqlQuery Room::insertQuery(const QSqlDatabase &database, const Room &room) {
    QSqlQuery query(database);
    query.prepare(
        "SELECT insert_room(:uuid, :name, :number, :barredStudents) AS ID");
    query.bindValue(":uuid", room.roomUUID());
    query.bindValue(":name", room.name());
    query.bindValue(":number", room.number());
    QString strIds;
    QTextStream ts(&strIds);
    for (auto i = 0; i < room.barredIds().size(); i++) {
        ts << room.barredIds()[i].toInt() << " ";
    }
    query.bindValue(":barredStudents", strIds.trimmed());
    return query;
}

QSqlQuery Room::updateQuery(const QSqlDatabase &database, const Room &room) {
    QSqlQuery query(database);
    query.prepare(
        "SELECT update_room(:id, :uuid, :name, :number, :barredStudents)");
    query.bindValue(":id", room.id());
    query.bindValue(":uuid", room.roomUUID());
    query.bindValue(":name", room.name());
    query.bindValue(":number", room.number());
    QString strIds;
    QTextStream ts(&strIds);
    for (auto i = 0; i < room.barredIds().size(); i++) {
        ts << room.barredIds()[i].toInt() << " ";
    }
    query.bindValue(":barredStudents", strIds.trimmed());
    return query;
}

QSqlQuery Room::findByUuidQuery(const QSqlDatabase &database,
                                const QUuid &uuid) {
    QSqlQuery query(database);
    query.prepare("SELECT R.*, "
                  "       (SELECT array_agg(ID_STUDENT::text) "
                  "         FROM BARRED_STUDENTS "
                  "         WHERE ID_ROOM = R.ID) AS BARRED_STUDENTS "
                  "  FROM ROOM R WHERE R.ROOM_UUID = :uuid");
    query.bindValue(":uuid", uuid);
    return query;
}

QSqlQuery Room::findByNumberQuery(const QSqlDatabase &database,
                                  const QString &number) {
    QSqlQuery query(database);
    query.prepare("SELECT R.*, "
                  "       (SELECT array_agg(ID_STUDENT::text) "
                  "          FROM BARRED_STUDENTS "
                  "         WHERE ID_ROOM = R.ID) AS BARRED_STUDENTS "
                  "  FROM ROOM R WHERE R.ROOM_NUMBER = :number");
    query.bindValue(":number", number);
    return query;
}

QSqlQuery Room::findAllQuery(const QSqlDatabase &database) {
    QSqlQuery query(database);
    query.prepare("SELECT R.*, (SELECT array_agg(ID_STUDENT::text) "
                  "               FROM BARRED_STUDENTS "
                  "              WHERE ID_ROOM = R.ID) AS BARRED_STUDENTS "
                  "  FROM ROOM R");
    return query;
}

QSqlQuery Room::deleteByUuidQuery(const QSqlDatabase &database,
                                  const QUuid &uuid) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM ROOM WHERE ROOM_UUID = :uuid");
    query.bindValue(":uuid", uuid);
    return query;
}

QSqlQuery Room::barredStudentsQuery(const QSqlDatabase &database,
                                    quint64 roomId) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM BARRED_STUDENTS_VIEW WHERE ID_ROOM = :roomId");
    query.bindValue(":roomId", roomId);
    return query;
}

QSqlQuery Room::allowedStudentsQuery(const QSqlDatabase &database,
                                     quint64 roomId) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM BARRED_STUDENTS_VIEW "
                  " WHERE COALESCE(ID_ROOM, -1) <> :roomId1 "
                  "  AND ID NOT IN (SELECT ID FROM ENLISTED_STUDENTS WHERE "
                  " ID_ROOM = :roomId2)");
    query.bindValue(":roomId1", roomId);
    query.bindValue(":roomId2", roomId);
    return query;
}

QVariantList Room::barredIds() const { return mBarredIds; }

void Room::setBarredIds(const QVariantList &barredIds) {
    mBarredIds = barredIds;
}
}
}
}
