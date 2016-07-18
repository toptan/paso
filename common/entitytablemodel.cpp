#include "entitytablemodel.h"

namespace paso {
namespace model {
EntityTableModel::EntityTableModel(
    const QStringList &columns, const QMap<QString, QString> &columnNames,
    std::shared_ptr<std::vector<data::entity::Entity>> data, QObject *parent)
    : QAbstractTableModel(parent), mColumns(columns), mColumnNames(columnNames),
      mData(data) {}

int EntityTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mColumns.size();
}

int EntityTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mData->size();
}

QVariant EntityTableModel::data(const QModelIndex &index, int role) const {
    auto &entity = (*mData)[index.row()];
    auto column = mColumns[index.column()];
    auto retVal = entity.value(column);
    return retVal;
//    return (*mData)[index.row()].value(mColumns[index.column()]);
}
}
}
