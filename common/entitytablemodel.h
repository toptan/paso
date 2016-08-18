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

class EntityTableModel : public QAbstractTableModel {
    Q_OBJECT

public:
    EntityTableModel(const QStringList &columns,
                     const QMap<QString, QString> &columnNames,
                     const data::entity::EntityVector &data,
                     QObject *parent = nullptr);

    virtual int
    columnCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual int
    rowCount(const QModelIndex &parent = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index,
                          int role = Qt::DisplayRole) const override;

    virtual QVariant headerData(int section, Qt::Orientation orientation,
                                int role = Qt::DisplayRole) const override;

    std::shared_ptr<paso::data::entity::Entity> entity(size_t position) const;

    void insertEntity(size_t position,
                      std::shared_ptr<paso::data::entity::Entity> entity);

    void removeEntity(size_t position);
    void
    removeEntity(const std::shared_ptr<paso::data::entity::Entity> &entity);

    void setEntityData(const data::entity::EntityVector &newData);
    void setEntityData(const QStringList &columns,
                       const QMap<QString, QString> &columnNames,
                       const data::entity::EntityVector &newData);

signals:
    void rowCountChanged(size_t newRowCount) const;

private:
    QStringList mColumns;
    QMap<QString, QString> mColumnNames;
    data::entity::EntityVector mData;
};
}
}

#endif // ENTITYTABLEMODEL_H
