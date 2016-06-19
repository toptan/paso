#ifndef PASODB_H
#define PASODB_H

#include "data.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <QVariantMap>

#include <memory>

namespace paso {
namespace db {

///
/// \brief DB_NAME The database name
///
const static QString DEFAULT_DB_NAME = "paso";

///
/// \brief The DBManager class is responsible for all database operations.
///
class DBManager {
public:
    ///
    /// \brief DBManager Constructs database manager that will use database with
    /// given name.
    /// \param dbName the name of database to use.
    ///
    explicit DBManager(const QString &dbName = DEFAULT_DB_NAME);

    ///
    /// \brief getAllSystemUsers Returns a vector of all system users.
    /// \param [out] error SQL error if any.
    /// \return A vector of all system users.
    ///
    std::shared_ptr<std::vector<data::SystemUser>>
    getAllSystemUsers(QSqlError &error);

    ///
    /// \brief getSystemUser Returns a system user with username \code username.
    /// \param [in]  username The user name to look for.
    /// \param [out] error SQL error if any.
    /// \return Found user or nullptr of none is found.
    ///
    std::shared_ptr<data::SystemUser> getSystemUser(const QString &username,
                                                    QSqlError &error);

    ///
    /// \brief saveSystemUser Adds a new or updates existing system user.
    /// \param [in]  user The user.
    /// \param [out] error SQL error if any.
    /// \return \code true if user is successfully saved.
    ///
    bool saveSystemUser(const data::SystemUser &user, QSqlError &error);

    ///
    /// \brief deleteSystemUser Deletes system user with given \code username.
    /// The user 'root' cannot be deleted.
    /// \param [in]  username The username.
    /// \param [out] error SQL error if any.
    /// \return \code true if user is succesdfully deleted.
    ///
    bool deleteSystemUser(const QString &username, QSqlError &error);

    ///
    /// \brief getAllRooms Returns a vector of all rooms.
    /// \param [out] error SQL error if any.
    /// \return A vector of all system users.
    ///
    std::shared_ptr<std::vector<data::Room>> getAllRooms(QSqlError &error);

    ///
    /// \brief getRoom Returns room with given \code roomUUID
    /// \param [in]  roomUUID The room UUID to look for.
    /// \param [out] error SQL error if any.
    /// \return Found
    ///
    std::shared_ptr<data::Room> getRoom(const QUuid &roomUUID,
                                        QSqlError &error);

    ///
    /// \brief saveRoom Adds a new or updates existing room.
    /// \param [in]  room The room.
    /// \param [out] error SQL error if any.
    /// \return \code true if room is successfully saved.
    ///
    bool saveRoom(const data::Room &room, QSqlError &error);

    ///
    /// \brief deleteRoom Deletes the room with given UUID.
    /// \param [in]  roomUUID The UUID of the room to delete.
    /// \param [out] error SQL error if any.
    /// \return \code true of room is succesfully deleted.
    ///
    bool deleteRoom(const QUuid &roomUUID, QSqlError &error);

    ///
    /// \brief usernameUnique checks if given username is unique.
    /// \param [in] username The username to check.
    /// \param [out] error The sql error if any.
    /// \return \code true if username does not exist in the database.
    ///
    bool usernameUnique(const QString &username, QSqlError &error) const;

    ///
    /// \brief roomUuidUnique checks if given room UUID is unique.
    /// \param [in] roomUUID The room UUID to check.
    /// \param [out] error The sql error if any.
    /// \return \code true if the room UUID does not exit in the database.
    ///
    bool roomUuidUnique(const QString &roomUUID, QSqlError &error) const;

    ///
    /// \brief roomNumberUnique checks if given room number is unique.
    /// \param [in] roomNumber The room number to check.
    /// \param [out] error The sql error if any.
    /// \return \code true if the room number does not exist in the database.
    ///
    bool roomNumberUnique(const QString &roomNumber, QSqlError &error) const;

    ///
    /// \brief courseCodeUnique checks if given course code is unique.
    /// \param [in] courseCode The course code to check.
    /// \param [out] error The sql error if any.
    /// \return \code true if the course code does not exist in the database.
    ///
    bool courseCodeUnique(const QString &courseCode, QSqlError &error) const;

private:
    const QString mDbName;
};
}
}

#endif // PASODB_H
