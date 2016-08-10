#ifndef LISTTABLEMODEL_H
#define LISTTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariantMap>

namespace paso {
namespace admin {

class ListTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit ListTableModel(const QVariantMap &columnLabels,
                            QSqlDatabase db = QSqlDatabase(),
                            QObject *parent = nullptr);

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;
};
}
}

#endif // LISTTABLEMODEL_H
