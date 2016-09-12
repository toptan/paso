#ifndef COURSETABLEMODEL_H
#define COURSETABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

///
/// \brief The CourseTableModel class handles data about courses.
///
class CourseTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    explicit CourseTableModel(const QVariantMap &columnLabels,
                              QSqlDatabase db = QSqlDatabase(),
                              QObject *parent = nullptr);
};
}
}

#endif // COURSETABLEMODEL_H
