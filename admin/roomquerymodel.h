#ifndef ROOMTABLEMODEL_H
#define ROOMTABLEMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

///
/// \brief The RoomQueryModel class handles data about rooms.
///
class RoomQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    explicit RoomQueryModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);

private:
    ///
    /// \brief QUERY The query string used to fetch the data.
    ///
    static const QString QUERY;
};
}
}
#endif // ROOMTABLEMODEL_H
