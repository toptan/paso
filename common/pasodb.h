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

///
/// \brief getAllSystemUsers Returns a vector of all system users.
/// \param db_name Database to use.
/// \return vector of all system users
///
std::shared_ptr<QVector<QVariantMap>> getAllSystemUsers(const QString &db_name);
}
}

#endif // PASODB_H
