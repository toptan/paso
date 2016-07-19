#ifndef ENTITYTABLEMODEL_H
#define ENTITYTABLEMODEL_H

#include "entity.h"

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <functional>
#include <memory>
#include <vector>

namespace paso {
namespace model {
using EntityVector = std::vector<std::shared_ptr<paso::data::entity::Entity>>;

class EntityTableModel : public QAbstractTableModel {
public:
    EntityTableModel(const QStringList &columns,
                     const QMap<QString, QString> &columnNames,
                     const EntityVector &data, QObject *parent = nullptr);

    virtual int
    columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int
    rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const override;

private:
    QStringList mColumns;
    QMap<QString, QString> mColumnNames;
    EntityVector mData;
};
}
}

#endif // ENTITYTABLEMODEL_H
