#include "pasodb.h"

#include <QDebug>
#include <QSqlRecord>
#include <QSqlError>

namespace paso {
namespace db {

using namespace std;
using namespace paso::data;

const QVariantMap recordToVariantMap(const QSqlRecord &record) {
    QVariantMap retVal;
    for (auto i = 0; i < record.count(); i++) {
        retVal.insert(record.fieldName(i), record.value(i));
    }
    return retVal;
}

const QString roleToString(data::SystemRole role) {
    switch (role) {
    case SystemRole::ADMINISTRATOR:
        return "ADMINISTRATOR";
    case SystemRole::MANAGER:
        return "MANAGER";
    case SystemRole::ROOM_MANAGER:
        return "ROOM_MANAGER";
    case SystemRole::SCHEDULER:
        return "SCHEDULER";
    case SystemRole::SUPER_USER:
        return "SUPER_USER";
    }
}

DBManager::DBManager(const QString &dbName) : mDbName(dbName) {
    QSqlError error;
    auto root = getSystemUser("root", error);
    if (!root) {
        root =
            make_shared<SystemUser>("root", "root_password", "Root", "Rootovic",
                                    "root@paso.system", SystemRole::SUPER_USER);
        saveSystemUser(*root, error);
    }
}

shared_ptr<vector<SystemUser>> DBManager::getAllSystemUsers(QSqlError &error) {
    auto db = QSqlDatabase::database(mDbName);
    auto query = db.exec("SELECT * FROM SYSTEM_USER ORDER BY USERNAME");
    error = query.lastError();
    auto retVal = make_shared<vector<SystemUser>>();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<SystemUser> DBManager::getSystemUser(const QString &username,
                                                QSqlError &error) {
    auto db = QSqlDatabase::database(mDbName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<SystemUser>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveSystemUser(const SystemUser &user, QSqlError &error) {
    QString strQuery = getSystemUser(user.username(), error)
                           ? "UPDATE SYSTEM_USER SET"
                             " PASSWORD = :password,"
                             " FIRST_NAME = :firstName,"
                             " LAST_NAME = :lastName,"
                             " EMAIL = :email,"
                             " ROLE = :role "
                             "WHERE USERNAME = :username"
                           : "INSERT INTO"
                             " SYSTEM_USER(USERNAME, PASSWORD, FIRST_NAME, "
                             "LAST_NAME, EMAIL, ROLE) VALUES(:username, "
                             ":password, :firstName, :lastName, :email, :role)";

    if (error.type() != QSqlError::NoError) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare(strQuery);
    query.bindValue(":username", user.username());
    query.bindValue(":password", user.password());
    query.bindValue(":firstName", user.firstName());
    query.bindValue(":lastName", user.lastName());
    query.bindValue(":email", user.email());
    query.bindValue(":role", roleToString(user.role()));
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        return true;
    }

    return false;
}
}
}
