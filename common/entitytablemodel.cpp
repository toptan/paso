#include "entitytablemodel.h"

#include <QDebug>

using namespace std;

using namespace paso::data::entity;

namespace paso {
namespace model {

EntityTableModel::EntityTableModel(const QStringList &columns,
                                   const QMap<QString, QString> &columnNames,
                                   const data::entity::EntityVector &data, QObject *parent)
    : QAbstractTableModel(parent), mColumns(columns), mColumnNames(columnNames),
      mData(data) {}

int EntityTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mColumns.size();
}

int EntityTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mData->size();
}

QVariant EntityTableModel::data(const QModelIndex &index, int role) const {
    return (*mData)[index.row()]->value(mColumns[index.column()]);
}

shared_ptr<Entity> EntityTableModel::entity(size_t position) const {
    return mData[position];
}

void EntityTableModel::insertEntity(size_t position,
                                    std::shared_ptr<Entity> entity) {
    emit beginInsertRows(QModelIndex(), position, position);
    if (position < mData.size()) {
        mData.insert(mData.begin() + position, entity);
    } else {
        mData.push_back(entity);
    }
    emit endInsertRows();
}

void EntityTableModel::removeEntity(size_t position) {
    if (position < mData.size()) {
        emit beginRemoveRows(QModelIndex(), position, position);
        mData.erase(mData.begin() + position);
        emit endRemoveRows();
    }
}
}
}
