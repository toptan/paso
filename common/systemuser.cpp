#include "systemuser.h"

namespace paso {
namespace data {

SystemUser::SystemUser(const QVariantMap &map)
    : mUsername(map["USERNAME"].toString()),
      mPassword(map["PASSWORD"].toString()),
      mFirstName(map["FIRST_NAME"].toString()),
      mLastName(map["LAST_NAME"].toString()), mEmail(map["EMAIL"].toString()),
      mRole(stringToRole(map["ROLE"].toString())) {}

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

void SystemUser::read(const QJsonObject &jsonObject) {
    mUsername = jsonObject["USERNAME"].toString();
    mPassword = jsonObject["PASSWORD"].toString();
    mFirstName = jsonObject["FIRST_NAME"].toString();
    mLastName = jsonObject["LAST_NAME"].toString();
    mEmail = jsonObject["EMAIL"].toString();
    auto strRole = jsonObject["ROLE"].toString();
    mRole = stringToRole(strRole);
}

void SystemUser::write(QJsonObject &jsonObject) const {
    jsonObject["USERNAME"] = mUsername;
    jsonObject["PASSWORD"] = mPassword;
    jsonObject["FIRST_NAME"] = mFirstName;
    jsonObject["LAST_NAME"] = mLastName;
    jsonObject["EMAIL"] = mEmail;
    jsonObject["ROLE"] = roleToString(mRole);
}
}
}
