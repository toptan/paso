#ifndef PASODB_H
#define PASODB_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QString>
#include <QVector>
#include <QVariantMap>

#include <memory>

namespace paso {
namespace db {

/// \brief getAllSystemUsers Returns a vector of all system users.
/// \param [in]  dbName Database to use.
/// \param [out] error SQL error if any.
/// \return A vector of all system users.
std::shared_ptr<QVector<QVariantMap>> getAllSystemUsers(const QString &dbName,
                                                        QSqlError &error);

/// \brief getSystemUser Returns a variant map with data for user 'username'.
/// \param [in]  dbName Database to use.
/// \param [in]  username The user name to look for.
/// \param [out] error SQL error if any.
/// \return A variant map with user's data. If user is not found empty map is returned.
QVariantMap getSystemUser(const QString &dbName, const QString &username,
                          QSqlError &error);
}
}

#endif // PASODB_H
