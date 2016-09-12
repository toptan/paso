#ifndef REFRESHABLESQLQUERYMODEL_H
#define REFRESHABLESQLQUERYMODEL_H

#include <QSortFilterProxyModel>
#include <QSqlQuery>
#include <QSqlQueryModel>

namespace paso {
namespace model {

///
/// \brief The RefreshableSqlQueryModel class is a modification of SQL query
/// model that implemetns similar behavior like table model by supporting
/// transparent data refreshing.
///
class RefreshableSqlQueryModel : public QSqlQueryModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given query string and the name of the
    /// database connection.
    /// \param strQuery The query string.
    /// \param databaseName The database connection name.
    /// \param parent The parent object.
    ///
    RefreshableSqlQueryModel(const QString &strQuery,
                             const QString &databaseName,
                             QObject *parent = nullptr);

    ///
    /// \brief Reexecutes the query to refresh the data.
    /// \return \c true if query was executed without errors.
    ///
    bool select();

private:
    QString mStrQuery;     //!< The query string.
    QString mDatabaseName; //!< The database to use.
};
}
}

#endif // REFRESHABLESQLQUERYMODEL_H
