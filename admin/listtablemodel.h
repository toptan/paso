#ifndef LISTTABLEMODEL_H
#define LISTTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

///
/// \brief The ListTableModel class handles data about lists.
///
class ListTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    ///
    /// \brief Constructs model with given column labels and database.
    /// \param columnLabels The column labels.
    /// \param db The database to use.
    /// \param parent The parent object.
    ///
    explicit ListTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);

    ///
    /// \brief Overrides default behavior to support custom data display.
    /// \param idx Index of an item to be displayed.
    /// \param role The display role.
    /// \return Variant containing neccessary data for proper displaying.
    ///
    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;
};
}
}

#endif // LISTTABLEMODEL_H
