#ifndef REFRESHABLESQLQUERYMODEL_H
#define REFRESHABLESQLQUERYMODEL_H

#include <QSqlQueryModel>
#include <QSqlQuery>

namespace paso {
namespace admin {

class RefreshableSqlQueryModel : public QSqlQueryModel {
public:
    RefreshableSqlQueryModel(const QString &strQuery,
                             const QString &databaseName,
                             QObject *parent = nullptr);

    bool select();

private:
    QString mStrQuery;     //!< The query string.
    QString mDatabaseName; //!< The database to use.
};
}
}

#endif // REFRESHABLESQLQUERYMODEL_H
