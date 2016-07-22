#include "entitytablemodel.h"

#include <QDebug>
#include <algorithm>

using namespace std;

using namespace paso::data::entity;

namespace paso {
namespace model {

EntityTableModel::EntityTableModel(const QStringList &columns,
                                   const QMap<QString, QString> &columnNames,
                                   const data::entity::EntityVector &data,
                                   QObject *parent)
    : QAbstractTableModel(parent), mColumns(columns), mColumnNames(columnNames),
      mData(data) {}

int EntityTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mColumns.size();
}

int EntityTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mData.size();
}

QVariant EntityTableModel::data(const QModelIndex &index, int role) const {
    if (role == Qt::DisplayRole) {
        return mData[index.row()]->value(mColumns[index.column()]);
    }
    return QVariant();
}

QVariant EntityTableModel::headerData(int section, Qt::Orientation orientation,
                                      int role) const {
    if (role == Qt::DisplayRole && orientation == Qt::Orientation::Horizontal &&
        section < mColumns.size()) {
        return mColumnNames[mColumns[section]];
    }
    return QAbstractTableModel::headerData(section, orientation, role);
}

shared_ptr<Entity> EntityTableModel::entity(size_t position) const {
    return mData[position];
}

void EntityTableModel::insertEntity(size_t position,
                                    shared_ptr<Entity> entity) {
    if (position < mData.size()) {
        emit beginInsertRows(QModelIndex(), position, position);
        mData.insert(mData.begin() + position, entity);
    } else {
        emit beginInsertRows(QModelIndex(), mData.size(), mData.size());
        mData.push_back(entity);
    }
    emit endInsertRows();
    emit rowCountChanged(mData.size());
}

void EntityTableModel::removeEntity(size_t position) {
    if (position < mData.size()) {
        emit beginRemoveRows(QModelIndex(), position, position);
        mData.erase(mData.begin() + position);
        emit endRemoveRows();
        emit rowCountChanged(mData.size());
    }
}

void EntityTableModel::removeEntity(const shared_ptr<Entity> entity) {
    auto position = find(mData.begin(), mData.end(), entity);
    if (position == mData.end()) {
        return;
    }
    emit beginRemoveRows(QModelIndex(), position - mData.begin(),
                         position - mData.begin());
    mData.erase(position);
    emit endRemoveRows();
    emit rowCountChanged(mData.size());
}

void EntityTableModel::setEntityData(const EntityVector &newData) {
    emit beginResetModel();
    mData.erase(mData.begin(), mData.end());
    mData.insert(mData.begin(), newData.begin(), newData.end());
    emit endResetModel();
    emit rowCountChanged(mData.size());
}

void EntityTableModel::setEntityData(const QStringList &columns,
                               const QMap<QString, QString> &columnNames,
                               const EntityVector &newData) {
    emit beginResetModel();
    mColumns = columns;
    mColumnNames = columnNames;
    mData.erase(mData.begin(), mData.end());
    mData.insert(mData.begin(), newData.begin(), newData.end());
    emit endResetModel();
    emit rowCountChanged(mData.size());
}
}
}
