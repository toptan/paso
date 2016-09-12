#ifndef STUDENTQUERYMODEL_H
#define STUDENTQUERYMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

///
/// \brief The StudentQueryModel class handles data about students.
///
class StudentQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    StudentQueryModel(const QVariantMap &columnLabels, QSqlDatabase db,
                      QObject *parent = nullptr);

    ///
    /// \brief Overrides default behavior to support different display alignment
    /// for some columns.
    /// \param item Index of an item to be displayed.
    /// \param role The display role.
    /// \return Variant containing neccessary data for proper displaying.
    ///
    virtual QVariant data(const QModelIndex &item, int role) const override;

private:
    ///
    /// \brief QUERY The query string used to fetch the data.
    ///
    static const QString QUERY;
};
}
}

#endif // STUDENTQUERYMODEL_H
