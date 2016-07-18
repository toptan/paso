#include "entitytablemodel.h"

#include <QDebug>

using namespace std;

namespace paso {
namespace model {

template <class T>
EntityTableModel<T>::EntityTableModel(const QStringList &columns,
                                      const QMap<QString, QString> &columnNames,
                                      shared_ptr<vector<T>> data,
                                      QObject *parent)
    : QAbstractTableModel(parent), mColumns(columns), mColumnNames(columnNames),
      mData(data) {
    qDebug() << __PRETTY_FUNCTION__ << ":" << mData->size();
}

template <class T>
int EntityTableModel<T>::columnCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mColumns.size();
}

template <class T>
int EntityTableModel<T>::rowCount(const QModelIndex &parent) const {
    return parent.isValid() ? 0 : mData->size();
}

template <class T>
QVariant EntityTableModel<T>::data(const QModelIndex &index, int role) const {
    qDebug() << __PRETTY_FUNCTION__ << ":" << index;
    auto entity = (*mData)[index.row()];
    auto column = mColumns[index.column()];
    qDebug() << __PRETTY_FUNCTION__ << ":" << column;
    auto retVal = entity.value(column);
    return retVal;
    //    return (*mData)[index.row()].value(mColumns[index.column()]);
}
}
}
