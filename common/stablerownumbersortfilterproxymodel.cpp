#include "stablerownumbersortfilterproxymodel.h"

#include <QDebug>

namespace paso {
namespace model {

StableRowNumberSortFilterProxyModel::StableRowNumberSortFilterProxyModel(
    QObject *parent)
    : QSortFilterProxyModel(parent) {}

QVariant StableRowNumberSortFilterProxyModel::headerData(
    int section, Qt::Orientation orientation, int role) const {

    if (orientation == Qt::Vertical && role == Qt::DisplayRole) {
        return section + 1;
    }

    return QSortFilterProxyModel::headerData(section, orientation, role);
}
}
}
