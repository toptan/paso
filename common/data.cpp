#include "data.h"

#include <QJsonDocument>

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

void SystemUser::read(const QString &jsonString) {
    QJsonObject jsonObject =
        QJsonDocument::fromJson(jsonString.toUtf8()).object();
    read(jsonObject);
}

void SystemUser::read(const QJsonObject &jsonObject) {
    mUsername = jsonObject["USERNAME"].toString();
    mPassword = jsonObject["PASSWORD"].toString();
    mFirstName = jsonObject["FIRST_NAME"].toString();
    mLastName = jsonObject["LAST_NAME"].toString();
    mEmail = jsonObject["EMAIL"].toString();
    auto strRole = jsonObject["ROLE"].toString();
    mRole = SystemRole::ADMINISTRATOR;
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

QString SystemUser::write() const {
    QJsonObject jsonObject;
    write(jsonObject);
    QJsonDocument doc;
    doc.setObject(jsonObject);
    return doc.toJson();
}

void SystemUser::write(QJsonObject &jsonObject) const {
    jsonObject["USERNAME"] = mUsername;
    jsonObject["PASSWORD"] = mPassword;
    jsonObject["FIRST_NAME"] = mFirstName;
    jsonObject["LAST_NAME"] = mLastName;
    jsonObject["EMAIL"] = mEmail;
    switch (mRole) {
    case SystemRole::ADMINISTRATOR:
        jsonObject["ROLE"] = "ADMINISTRATOR";
        break;
    case SystemRole::ROOM_MANAGER:
        jsonObject["ROLE"] = "ROOM_MANAGER";
        break;
    case SystemRole::MANAGER:
        jsonObject["ROLE"] = "MANAGER";
        break;
    case SystemRole::SCHEDULER:
        jsonObject["ROLE"] = "SCHEDULER";
        break;
    case SystemRole::SUPER_USER:
        jsonObject["ROLE"] = "SUPER_USER";
        break;
    }
}

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

void Room::read(const QString &jsonString) {
    QJsonObject jsonObject =
        QJsonDocument::fromJson(jsonString.toUtf8()).object();
    read(jsonObject);
}

void Room::read(const QJsonObject &jsonObject) {
    mRoomUUID = jsonObject["ROOM_UUID"].toString();
    mName = jsonObject["NAME"].toString();
    mNumber = jsonObject["NUMBER"].toString();
}

QString Room::write() const {
    QJsonObject jsonObject;
    write(jsonObject);
    QJsonDocument doc;
    doc.setObject(jsonObject);
    return doc.toJson();
}

void Room::write(QJsonObject &jsonObject) const {
    jsonObject["ROOM_UUID"] = mRoomUUID;
    jsonObject["NAME"] = mName;
    jsonObject["NUMBER"] = mNumber;
}
}
}
