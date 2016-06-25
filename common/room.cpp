#include "room.h"

#include <QVariantMap>

namespace paso {
namespace data {

Room::Room(const QString &roomUUID, const QString &name, const QString &number)
    : mRoomUUID(roomUUID), mName(name), mNumber(number) {}

Room::Room(const QVariantMap &map)
    : mRoomUUID(map["ROOM_UUID"].toString()), mName(map["NAME"].toString()),
      mNumber(map["ROOM_NUMBER"].toString()) {}

Room::Room(const Room &other)
    : mRoomUUID(other.mRoomUUID), mName(other.mName), mNumber(other.number()) {}

bool Room::operator==(const Room &other) const {
    if (this == &other) {
        return true;
    }

    return mRoomUUID == other.mRoomUUID && mName == other.mName &&
           mNumber == other.mNumber;
}

QString Room::roomUUID() const { return mRoomUUID; }

QString Room::name() const { return mName; }

void Room::setName(const QString &name) { mName = name; }

QString Room::number() const { return mNumber; }

void Room::setNumber(const QString &number) { mNumber = number; }

void Room::read(const QJsonObject &jsonObject) {
    mRoomUUID = jsonObject["ROOM_UUID"].toString();
    mName = jsonObject["NAME"].toString();
    mNumber = jsonObject["ROOM_NUMBER"].toString();
}

void Room::write(QJsonObject &jsonObject) const {
    jsonObject["ROOM_UUID"] = mRoomUUID;
    jsonObject["NAME"] = mName;
    jsonObject["ROOM_NUMBER"] = mNumber;
}
}
}
