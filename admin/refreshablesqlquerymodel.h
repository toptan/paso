#ifndef REFRESHABLESQLQUERYMODEL_H
#define REFRESHABLESQLQUERYMODEL_H

#include <QSortFilterProxyModel>
#include <QSqlQueryModel>
#include <QSqlQuery>

namespace paso {
namespace admin {

class RefreshableSqlQueryModel : public QSqlQueryModel {
    Q_OBJECT
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
