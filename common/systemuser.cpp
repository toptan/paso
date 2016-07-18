#include "systemuser.h"

namespace paso {
namespace data {
namespace entity {

SystemUser::SystemUser(const QVariantMap &map)
    : Entity(map["ID"].toULongLong()), mUsername(map["USERNAME"].toString()),
      mPassword(map["PASSWORD"].toString()),
      mFirstName(map["FIRST_NAME"].toString()),
      mLastName(map["LAST_NAME"].toString()), mEmail(map["EMAIL"].toString()),
      mRole(stringToRole(map["ROLE"].toString())) {}

SystemUser::SystemUser(const QString &username, const QString &password,
                       const QString &firstName, const QString &lastName,
                       const QString &email, SystemRole role, uint64_t id)
    : Entity(id), mUsername(username), mPassword(password),
      mFirstName(firstName), mLastName(lastName), mEmail(email), mRole(role) {}

SystemUser::SystemUser(const SystemUser &other)
    : Entity(other), mUsername(other.mUsername), mPassword(other.mPassword),
      mFirstName(other.mFirstName), mLastName(other.mLastName),
      mEmail(other.mEmail), mRole(other.mRole) {}

bool SystemUser::operator==(const SystemUser &other) const {
    if (this == &other) {
        return true;
    }
    return id() == other.id() && mUsername == other.mUsername &&
           mPassword == other.mPassword && mFirstName == other.mFirstName &&
           mLastName == other.mLastName && mEmail == other.mEmail &&
           mRole == other.mRole;
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

QVariantMap SystemUser::toVariantMap() const {
    auto retVal = Entity::toVariantMap();
    retVal.insert("USERNAME", mUsername);
    retVal.insert("PASSWORD", mPassword);
    retVal.insert("FIRST_NAME", mFirstName);
    retVal.insert("LAST_NAME", mLastName);
    retVal.insert("EMAIL", mEmail);
    retVal.insert("ROLE", roleToString(mRole));

    return retVal;
}

QVariant SystemUser::value(const QString &property) const {
    if (property == "USERNAME") {
        return mUsername;
    } else if (property == "PASSWORD") {
        return mPassword;
    } else if (property == "FIRST_NAME") {
        return mFirstName;
    } else if (property == "LAST_NAME") {
        return mLastName;
    } else if (property == "EMAIL") {
        return mEmail;
    } else if (property == "ROLE") {
        return roleToString(mRole);
    }

    return Entity::value(property);
}

void SystemUser::read(const QJsonObject &jsonObject) {
    setId(jsonObject["ID"].toVariant().toULongLong());
    mUsername = jsonObject["USERNAME"].toString();
    mPassword = jsonObject["PASSWORD"].toString();
    mFirstName = jsonObject["FIRST_NAME"].toString();
    mLastName = jsonObject["LAST_NAME"].toString();
    mEmail = jsonObject["EMAIL"].toString();
    auto strRole = jsonObject["ROLE"].toString();
    mRole = stringToRole(strRole);
}

void SystemUser::write(QJsonObject &jsonObject) const {
    jsonObject["ID"] = static_cast<qint64>(id());
    jsonObject["USERNAME"] = mUsername;
    jsonObject["PASSWORD"] = mPassword;
    jsonObject["FIRST_NAME"] = mFirstName;
    jsonObject["LAST_NAME"] = mLastName;
    jsonObject["EMAIL"] = mEmail;
    jsonObject["ROLE"] = roleToString(mRole);
}

QSqlQuery SystemUser::insertQuery(const QSqlDatabase &database,
                                  const SystemUser &user) {
    QSqlQuery query(database);
    query.prepare("INSERT INTO SYSTEM_USER (USERNAME, PASSWORD, FIRST_NAME, "
                  "LAST_NAME, EMAIL, ROLE) VALUES (:username, :password, "
                  ":first_name, :last_name, :email, :role)");
    query.bindValue(":username", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name", user.lastName());
    query.bindValue(":email", user.email());
    query.bindValue(":role", roleToString(user.role()));
    return query;
}

QSqlQuery SystemUser::updateQuery(const QSqlDatabase &database,
                                  const SystemUser &user) {
    QSqlQuery query(database);
    query.prepare("UPDATE SYSTEM_USER SET USERNAME = :username, PASSWORD = "
                  ":password, FIRST_NAME = :first_name, LAST_NAME = "
                  ":last_name, EMAIL = :email, ROLE = :role WHERE ID = :id");
    query.bindValue(":id", static_cast<quint64>(user.id()));
    query.bindValue(":username", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":first_name", user.firstName());
    query.bindValue(":last_name", user.lastName());
    query.bindValue(":email", user.email());
    query.bindValue(":role", roleToString(user.role()));
    return query;
}

QSqlQuery SystemUser::findByUsernameQuery(const QSqlDatabase &database,
                                          const QString &username) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    return query;
}

QSqlQuery SystemUser::findAllQuery(const QSqlDatabase &database) {
    QSqlQuery query(database);
    query.prepare("SELECT * FROM SYSTEM_USER");
    return query;
}

QSqlQuery SystemUser::deleteByUsernameQuery(const QSqlDatabase &database,
                                            const QString &username) {
    QSqlQuery query(database);
    query.prepare("DELETE FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    return query;
}
}
}
}
