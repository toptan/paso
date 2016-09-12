#ifndef STABLEROWNUMBERSORTFILTERPROXYMODEL_H
#define STABLEROWNUMBERSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVariant>

namespace paso {
namespace model {

///
/// \brief The StableRowNumberSortFilterProxyModel class fixes vertical header
/// on table views that use SQL query based models to have stable row numbers.
///
class StableRowNumberSortFilterProxyModel : public QSortFilterProxyModel {
public:
    ///
    /// \brief Constructs model with given parent.
    /// \param parent The parent object.
    ///
    explicit StableRowNumberSortFilterProxyModel(QObject *parent = nullptr);

    ///
    /// \brief Overrides default behavior to have stable row numbers displayed.
    /// \param section The headers section.
    /// \param orientation The header orientation.
    /// \param role The display role.
    /// \return Variant containing data for proper displaying of the row number.
    ///
    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;
};
}
}
#endif // STABLEROWNUMBERSORTFILTERPROXYMODEL_H
