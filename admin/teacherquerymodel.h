#ifndef TEACHERQUERYMODEL_H
#define TEACHERQUERYMODEL_H

#include "refreshablesqlquerymodel.h"

#include <QObject>
#include <QVariantMap>

namespace paso {
namespace model {

///
/// \brief The TeacherQueryModel class handles data about teachers.
///
class TeacherQueryModel : public RefreshableSqlQueryModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    TeacherQueryModel(const QVariantMap &columnLabels, QSqlDatabase db,
                      QObject *parent = nullptr);

private:
    ///
    /// \brief The query string used to fetch the data.
    ///
    static const QString QUERY;
};
}
}
#endif // TEACHERQUERYMODEL_H
