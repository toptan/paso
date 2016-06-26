#include "pasodb.h"

#include <QDebug>
#include <QSqlRecord>
#include <QSqlResult>
#include <QSqlError>
#include <QUuid>

namespace paso {
namespace db {

using namespace std;
using namespace paso::data;
using namespace paso::data::entity;

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
    auto query = SystemUser::findAllQuery(QSqlDatabase::database(mDbName));
    query.exec();
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
    auto query = SystemUser::findByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<SystemUser>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveSystemUser(SystemUser &user, QSqlError &error) {
    auto query =
        user.id() == 0
            ? SystemUser::insertQuery(QSqlDatabase::database(mDbName), user)
            : SystemUser::updateQuery(QSqlDatabase::database(mDbName), user);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError && user.id() == 0) {
        user.setId(query.lastInsertId().toULongLong());
    }

    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteSystemUser(const QString &username, QSqlError &error) {
    if (username == "root") {
        return false;
    }

    auto query = SystemUser::deleteByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

shared_ptr<vector<Room>> DBManager::getAllRooms(QSqlError &error) {
    auto query = Room::findAllQuery(QSqlDatabase::database(mDbName));
    auto retVal = make_shared<vector<Room>>();
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        while (query.next()) {
            retVal->emplace_back(recordToVariantMap(query.record()));
        }
    }
    return retVal;
}

shared_ptr<Room> DBManager::getRoom(const QUuid &roomUUID, QSqlError &error) {
    auto query =
        Room::findByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Room>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveRoom(Room &room, QSqlError &error) {
    auto query = room.id() == 0
                     ? Room::insertQuery(QSqlDatabase::database(mDbName), room)
                     : Room::updateQuery(QSqlDatabase::database(mDbName), room);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError && room.id() == 0) {
        room.setId(query.lastInsertId().toULongLong());
    }

    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteRoom(const QUuid &roomUUID, QSqlError &error) {
    auto query =
        Room::deleteByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

shared_ptr<Course> DBManager::getCourse(const QString &courseCode,
                                        QSqlError &error) {
    auto query =
        Course::findByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError) {
        if (query.next()) {
            return make_shared<Course>(recordToVariantMap(query.record()));
        }
    }
    return nullptr;
}

bool DBManager::saveCourse(Course &course, QSqlError &error) {
    auto query =
        course.id() == 0
            ? Course::insertQuery(QSqlDatabase::database(mDbName), course)
            : Course::updateQuery(QSqlDatabase::database(mDbName), course);
    query.exec();
    error = query.lastError();
    if (error.type() == QSqlError::NoError && course.id() == 0) {
        course.setId(query.lastInsertId().toULongLong());
    }
    return error.type() == QSqlError::NoError;
}

bool DBManager::deleteCourse(const QString &courseCode, QSqlError &error) {
    auto query =
        Course::deleteByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    return error.type() == QSqlError::NoError;
}

bool DBManager::usernameUnique(const QString &username,
                               QSqlError &error) const {
    auto query = SystemUser::findByUsernameQuery(
        QSqlDatabase::database(mDbName), username);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::roomUuidUnique(const QString &roomUUID,
                               QSqlError &error) const {
    auto query =
        Room::findByUuidQuery(QSqlDatabase::database(mDbName), roomUUID);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::roomNumberUnique(const QString &roomNumber,
                                 QSqlError &error) const {
    auto query =
        Room::findByNumberQuery(QSqlDatabase::database(mDbName), roomNumber);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}

bool DBManager::courseCodeUnique(const QString &courseCode,
                                 QSqlError &error) const {
    auto query =
        Course::findByCodeQuery(QSqlDatabase::database(mDbName), courseCode);
    query.exec();
    error = query.lastError();
    if (error.type() != QSqlError::NoError || query.next()) {
        return false;
    }
    return true;
}
}
}
