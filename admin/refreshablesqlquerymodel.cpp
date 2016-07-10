#include "refreshablesqlquerymodel.h"

#include <QSqlError>

namespace paso {
namespace admin {

RefreshableSqlQueryModel::RefreshableSqlQueryModel(const QString &strQuery,
                                                   const QString &databaseName,
                                                   QObject *parent)
    : QSqlQueryModel(parent), mStrQuery(strQuery), mDatabaseName(databaseName) {
    setQuery(mStrQuery, QSqlDatabase::database(mDatabaseName));
}

bool RefreshableSqlQueryModel::select() {
    setQuery(mStrQuery, QSqlDatabase::database(mDatabaseName));
    return lastError().type() == QSqlError::NoError;
}
}
}
