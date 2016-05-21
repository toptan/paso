#include "data.h"

namespace paso {
namespace data {

// SystemUser methods

SystemUser::SystemUser(const QVariantMap &map)
    : mUsername(map["USERNAME"].toString()),
      mPassword(map["PASSWORD"].toString()),
      mFirstName(map["FIRST_NAME"].toString()),
      mLastName(map["LAST_NAME"].toString()), mEmail(map["EMAIL"].toString()) {
    mRole = SystemRole::ADMINISTRATOR;
    QString strRole = map["ROLE"].toString();
    if (strRole == "ADMINISTRATOR") {
        mRole = SystemRole::ADMINISTRATOR;
    } else if (strRole == "ROOM_MANAGER") {
        mRole = SystemRole::ROOM_MANAGER;
    } else if (strRole == "MANAGER") {
        mRole = SystemRole::MANAGER;
    } else if (strRole == "SCHEDULER") {
        mRole = SystemRole::SCHEDULER;
    } else if (strRole == "SUPER_USER") {
        mRole = SystemRole::SUPER_USER;
    }
}

SystemUser::SystemUser(const QString &username, const QString &password,
                       const QString &firstName, const QString &lastName,
                       const QString &email, SystemRole role)
    : mUsername(username), mPassword(password), mFirstName(firstName),
      mLastName(lastName), mEmail(email), mRole(role) {}

SystemUser::SystemUser(const SystemUser &other)
    : mUsername(other.mUsername), mPassword(other.mPassword),
      mFirstName(other.mFirstName), mLastName(other.mLastName),
      mEmail(other.mEmail), mRole(other.mRole) {}

bool SystemUser::operator==(const SystemUser &other) const {
    if (this == &other) {
        return true;
    }
    return mUsername == other.mUsername && mPassword == other.mPassword &&
           mFirstName == other.mFirstName && mLastName == other.mLastName &&
           mEmail == other.mEmail && mRole == other.mRole;
}

QString SystemUser::username() const { return mUsername; }

QString SystemUser::password() const { return mPassword; }

void SystemUser::setPassword(const QString &password) { mPassword = password; }

QString SystemUser::firstName() const { return mFirstName; }

void SystemUser::setFirstName(const QString &firstName) {
    mFirstName = firstName;
}

QString SystemUser::lastName() const { return mLastName; }

void SystemUser::setLastName(const QString &lastName) { mLastName = lastName; }

QString SystemUser::email() const { return mEmail; }

void SystemUser::setEmail(const QString &email) { mEmail = email; }

SystemRole SystemUser::role() const { return mRole; }

void SystemUser::setRole(const SystemRole &role) { mRole = role; }

// Room methods

Room::Room(const QString &roomUUID, const QString &name, const QString &number)
    : mRoomUUID(roomUUID), mName(name), mNumber(number) {}

Room::Room(const QVariantMap &map)
    : mRoomUUID(map["ROOM_UUID"].toString()), mName(map["NAME"].toString()),
      mNumber(map["NUMBER"].toString()) {}

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
}
}
