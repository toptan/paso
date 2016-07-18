#ifndef ENTITYTABLEMODEL_H
#define ENTITYTABLEMODEL_H

#include "entity.h"

#include <QAbstractTableModel>
#include <QMap>
#include <QStringList>
#include <memory>
#include <vector>

namespace paso {
namespace model {

class EntityTableModel : public QAbstractTableModel {
public:
    EntityTableModel(const QStringList &columns,
                     const QMap<QString, QString> &columnNames,
                     std::shared_ptr<std::vector<data::entity::Entity>> data,
                     QObject *parent = nullptr);

    virtual int
    columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int
    rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const override;

private:
    QStringList mColumns;
    QMap<QString, QString> mColumnNames;
    std::shared_ptr<std::vector<data::entity::Entity>> mData;
};
}
}

#endif // ENTITYTABLEMODEL_H
