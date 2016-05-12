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
/// \brief The DBManager class is responsible for all database operations.
///
class DBManager {
public:
    ///
    /// \brief DBManager Constructs database manager that will use database with
    /// given name.
    /// \param dbName the name of database to use.
    ///
    explicit DBManager(const QString &dbName);

    /// \brief getAllSystemUsers Returns a vector of all system users.
    /// \param [out] error SQL error if any.
    /// \return A vector of all system users.
    std::shared_ptr<std::vector<data::SystemUser>>
    getAllSystemUsers(QSqlError &error);

    /// \brief getSystemUser Returns a variant map with data for user
    /// 'username'.
    /// \param [in]  username The user name to look for.
    /// \param [out] error SQL error if any.
    /// \return Found user with all the data or if user is not found user with
    /// empty
    /// username.
    std::shared_ptr<data::SystemUser> getSystemUser(const QString &username,
                                                    QSqlError &error);

    /// \brief saveSystemUser Adds a new or updates existing system user.
    /// \param [in]  user The user.
    /// \param [out] error SQL error if any.
    /// \return \code true if user is successfully saved.
    bool saveSystemUser(const data::SystemUser &user, QSqlError &error);

private:
    const QString mDbName;
};
}
}

#endif // PASODB_H
