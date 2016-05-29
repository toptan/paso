#ifndef SYSTEMUSERTABLEMODEL_H
#define SYSTEMUSERTABLEMODEL_H

#include <QObject>
#include <QSqlTableModel>
#include <QVariant>

namespace paso {
namespace admin {
class SystemUserTableModel : public QSqlTableModel {
    Q_OBJECT
public:
    explicit SystemUserTableModel(QObject *parent = nullptr,
                                  QSqlDatabase db = QSqlDatabase());

    virtual QVariant data(const QModelIndex &idx,
                          int role = Qt::DisplayRole) const override;
};
}
}

#endif // SYSTEMUSERTABLEMODEL_H
