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
    explicit StableRowNumberSortFilterProxyModel(QObject *parent = nullptr);

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

};
}
}
#endif // STABLEROWNUMBERSORTFILTERPROXYMODEL_H
