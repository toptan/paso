#include "pasodb.h"

#include <QDebug>
#include <QSqlRecord>
#include <QSqlError>
#include <QUuid>

namespace paso {
namespace db {

using namespace std;
using namespace paso::data;

const QVariantMap recordToVariantMap(const QSqlRecord &record) {
    QVariantMap retVal;
    for (auto i = 0; i < record.count(); i++) {
        retVal.insert(record.fieldName(i).toUpper(), record.value(i));
    }
    return retVal;
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
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteSystemUser(const QString &username, QSqlError &error) {
    if (username == "root") {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare("DELETE FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

shared_ptr<vector<Room>> DBManager::getAllRooms(QSqlError &error) {
    auto db = QSqlDatabase::database(mDbName);
    auto query = db.exec("SELECT * FROM ROOM ORDER BY NAME");
    error = query.lastError();
    auto retVal = make_shared<vector<Room>>();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<Room> DBManager::getRoom(const QUuid &roomUUID, QSqlError &error) {
    auto db = QSqlDatabase::database(mDbName);
    QSqlQuery query(db);
    query.prepare("SELECT * FROM ROOM WHERE ROOM_UUID = :room_uuid");
    query.bindValue(":room_uuid", roomUUID);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Room>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveRoom(const Room &room, QSqlError &error) {
    QUuid roomUUID(room.roomUUID());
    QString strQuery = getRoom(roomUUID, error)
                           ? "UPDATE ROOM SET"
                             " NAME = :name,"
                             " ROOM_NUMBER = :number "
                             "WHERE ROOM_UUID = :room_uuid"
                           : "INSERT INTO"
                             " ROOM (ROOM_UUID, NAME, ROOM_NUMBER)"
                             " VALUES(:room_uuid, :name, :number)";

    if (error.type() != QSqlError::NoError) {
        return false;
    }

    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare(strQuery);
    query.bindValue(":room_uuid", roomUUID.toString());
    query.bindValue(":name", room.name());
    query.bindValue(":number", room.number());
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteRoom(const QUuid &roomUUID, QSqlError &error) {
    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare("DELETE FROM ROOM WHERE ROOM_UUID = :room_uuid");
    query.bindValue(":room_uuid", roomUUID.toString());
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

bool DBManager::usernameUnique(const QString &username,
                               QSqlError &error) const {
    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare(
        "SELECT COUNT(1) FROM SYSTEM_USER WHERE USERNAME = :username");
    query.bindValue(":username", username);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return query.record().value(0) == 0;
        }
    }
    return false;
}

bool DBManager::roomUuidUnique(const QString &roomUUID,
                               QSqlError &error) const {
    QUuid uuid(roomUUID);
    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare("SELECT COUNT(1) FROM ROOM WHERE ROOM_UUID = :uuid");
    query.bindValue(":uuid", uuid);
    query.exec();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return query.record().value(0) == 0;
        }
    }
    return false;
}

bool DBManager::roomNumberUnique(const QString &roomNumber,
                                 QSqlError &error) const {
    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare("SELECT COUNT(1) FROM ROOM WHERE ROOM_NUMBER = :room_number");
    query.bindValue(":room_number", roomNumber);
    query.exec();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return query.record().value(0) == 0;
        }
    }
    return false;
}

bool DBManager::courseCodeUnique(const QString &courseCode,
                                 QSqlError &error) const {
    QSqlQuery query(QSqlDatabase::database(mDbName));
    query.prepare("SELECT COUNT(1) FROM COURSE WHERE CODE = :code");
    query.bindValue(":code", courseCode);
    query.exec();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return query.record().value(0) == 0;
        }
    }
    return false;
}
}
}
