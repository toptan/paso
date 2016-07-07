#include "room.h"

#include <QVariantMap>

namespace paso {
namespace data {
namespace entity {

Room::Room(const QString &roomUUID, const QString &name, const QString &number,
           uint64_t id)
    : Entity(id), mRoomUUID(roomUUID), mName(name), mNumber(number) {}

Room::Room(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mRoomUUID(map["ROOM_UUID"].toString()),
      mName(map["NAME"].toString()), mNumber(map["ROOM_NUMBER"].toString()) {}

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

void Room::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mRoomUUID = jsonObject["ROOM_UUID"].toString();
    mName = jsonObject["NAME"].toString();
    mNumber = jsonObject["ROOM_NUMBER"].toString();
}

void Room::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["ROOM_UUID"] = mRoomUUID;
    jsonObject["NAME"] = mName;
    jsonObject["ROOM_NUMBER"] = mNumber;
}

QSqlQuery Room::insertQuery(const QSqlDatabase &database, const Room &room) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO ROOM (ROOM_UUID, NAME, ROOM_NUMBER) VALUES "
                  "(:uuid, :name, :number)");
    query.bindValue(":uuid", room.roomUUID());
    query.bindValue(":name", room.name());
    query.bindValue(":number", room.number());
    return query;
}

QSqlQuery Room::updateQuery(const QSqlDatabase &database, const Room &room) {
    QSqlQuery query(database);
    query.prepare("UPDATE ROOM SET ROOM_UUID = :uuid, NAME = :name, "
                  "ROOM_NUMBER = :number WHERE ID = :id");
    query.bindValue(":id", static_cast<quint64>(room.id()));
    query.bindValue(":uuid", room.roomUUID());
    query.bindValue(":name", room.name());
    query.bindValue(":number", room.number());
    return query;
}

QSqlQuery Room::findByUuidQuery(const QSqlDatabase &database,
                                const QUuid &uuid) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM ROOM WHERE ROOM_UUID = :uuid");
    query.bindValue(":uuid", uuid);
    return query;
}

QSqlQuery Room::findByNumberQuery(const QSqlDatabase &database,
                                  const QString &number) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM ROOM WHERE ROOM_NUMBER = :number");
    query.bindValue(":number", number);
    return query;
}

QSqlQuery Room::findAllQuery(const QSqlDatabase &database) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM ROOM");
    return query;
}

QSqlQuery Room::deleteByUuidQuery(const QSqlDatabase &database,
                                  const QUuid &uuid) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM ROOM WHERE ROOM_UUID = :uuid");
    query.bindValue(":uuid", uuid);
    return query;
}
}
}
}
