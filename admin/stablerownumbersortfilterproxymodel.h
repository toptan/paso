#ifndef STABLEROWNUMBERSORTFILTERPROXYMODEL_H
#define STABLEROWNUMBERSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>
#include <QVariant>

namespace paso {
namespace admin {

class StableRowNumberSortFilterProxyModel : public QSortFilterProxyModel {
public:
    explicit StableRowNumberSortFilterProxyModel(QObject *parent = nullptr);

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;
};
}
}
#endif // STABLEROWNUMBERSORTFILTERPROXYMODEL_H
