#include "entitytablemodel.h"

#include <QDebug>

using namespace std;

namespace paso {
namespace model {

EntityTableModel::EntityTableModel(const QStringList &columns,
                                   const QMap<QString, QString> &columnNames,
                                   const EntityVector &data, QObject *parent)
    : QAbstractTableModel(parent), mColumns(columns), mColumnNames(columnNames),
      mData(data) {
}

int EntityTableModel::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mColumns.size();
}

int EntityTableModel::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mData.size();
}

QVariant EntityTableModel::data(const QModelIndex &index, int role) const {
    return mData[index.row()]->value(mColumns[index.column()]);
}
}
}
